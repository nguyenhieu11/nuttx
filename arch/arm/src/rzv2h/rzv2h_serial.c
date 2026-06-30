/****************************************************************************
 * arch/arm/src/rzv2h/rzv2h_serial.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/serial/serial.h>

#include "arm_internal.h"
#include "hardware/rzv2h_memorymap.h"
#include "hardware/rzv2h_sci.h"
#include "rzv2h_lowputc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Ref: Zephyr PR #91394 dts/arm/renesas/rz/rzv/r9a09g057_cr8.dtsi
 * interrupts = <GIC_SPI 114 ... (eri)>,
 *              <GIC_SPI 115 ... (rxi)>,
 *              <GIC_SPI 116 ... (txi)>,
 *              <GIC_SPI 117 ... (tei)>;
 * GIC SPI interrupts map to global IRQs at offset 32.
 */
#define RZV2H_SCI0_RXI_IRQ    (115 + 32) /* RX Data Full */
#define RZV2H_SCI0_TXI_IRQ    (116 + 32) /* TX Data Empty */

#define RZV2H_RXBUFSIZE       256
#define RZV2H_TXBUFSIZE       256

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct rzv2h_dev_s
{
  uintptr_t scibase;  /* SCI register base address */
  uint8_t   rxi_irq;  /* RX IRQ number */
  uint8_t   txi_irq;  /* TX IRQ number */
  uint32_t  baud;     /* Configured baudrate */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  rzv2h_setup(struct uart_dev_s *dev);
static void rzv2h_shutdown(struct uart_dev_s *dev);
static int  rzv2h_attach(struct uart_dev_s *dev);
static void rzv2h_detach(struct uart_dev_s *dev);
static int  rzv2h_ioctl(struct file *filep, int cmd, unsigned long arg);
static int  rzv2h_receive(struct uart_dev_s *dev, unsigned int *status);
static void rzv2h_rxint(struct uart_dev_s *dev, bool enable);
static bool rzv2h_rxavailable(struct uart_dev_s *dev);
static void rzv2h_send(struct uart_dev_s *dev, int ch);
static void rzv2h_txint(struct uart_dev_s *dev, bool enable);
static bool rzv2h_txready(struct uart_dev_s *dev);
static bool rzv2h_txempty(struct uart_dev_s *dev);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct uart_ops_s g_uart_ops =
{
  .setup          = rzv2h_setup,
  .shutdown       = rzv2h_shutdown,
  .attach         = rzv2h_attach,
  .detach         = rzv2h_detach,
  .ioctl          = rzv2h_ioctl,
  .receive        = rzv2h_receive,
  .rxint          = rzv2h_rxint,
  .rxavailable    = rzv2h_rxavailable,
  .send           = rzv2h_send,
  .txint          = rzv2h_txint,
  .txready        = rzv2h_txready,
  .txempty        = rzv2h_txempty,
};

static char g_rxbuffer[RZV2H_RXBUFSIZE];
static char g_txbuffer[RZV2H_TXBUFSIZE];

static struct rzv2h_dev_s g_uart0priv =
{
  .scibase = RZV2H_SCI0_BASE,
  .rxi_irq = RZV2H_SCI0_RXI_IRQ,
  .txi_irq = RZV2H_SCI0_TXI_IRQ,
  .baud    = CONFIG_RZV2H_SCI_BAUDRATE,
};

static struct uart_dev_s g_uart0port =
{
  .recv     =
  {
    .size   = RZV2H_RXBUFSIZE,
    .buffer = g_rxbuffer,
  },
  .xmit     =
  {
    .size   = RZV2H_TXBUFSIZE,
    .buffer = g_txbuffer,
  },
  .ops      = &g_uart_ops,
  .priv     = &g_uart0priv,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int rzv2h_setup(struct uart_dev_s *dev)
{
  /* Initialize the SCI interface using early setup settings
   * Ref: RA4 ra_serial.c Setup callback
   */
  rzv2h_lowsetup();
  return OK;
}

static void rzv2h_shutdown(struct uart_dev_s *dev)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;

  /* Disable interrupts on SCI peripheral
   * Ref: Renesas SCI-B User's Manual - clear RIE and TIE in SCR
   */
  putreg8(0, priv->scibase + R_SCI_SCR_OFFSET);
}

static int rzv2h_rx_interrupt(int irq, void *context, void *arg)
{
  struct uart_dev_s *dev = (struct uart_dev_s *)arg;

  /* Notify NuttX serial layer of new received character */
  uart_recvchars(dev);
  return OK;
}

static int rzv2h_tx_interrupt(int irq, void *context, void *arg)
{
  struct uart_dev_s *dev = (struct uart_dev_s *)arg;

  /* Feed more characters to transmission channel */
  uart_xmitchars(dev);
  return OK;
}

static int rzv2h_attach(struct uart_dev_s *dev)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;
  int ret;

  /* Attach the RX and TX GIC interrupts to driver handlers
   * Ref: NuttX serial design for multi-interrupt UART peripherals
   */
  ret = irq_attach(priv->rxi_irq, rzv2h_rx_interrupt, dev);
  if (ret == OK)
    {
      ret = irq_attach(priv->txi_irq, rzv2h_tx_interrupt, dev);
      if (ret != OK)
        {
          irq_detach(priv->rxi_irq);
        }
      else
        {
          /* Enable interrupts at GIC controller level */
          up_enable_irq(priv->rxi_irq);
          up_enable_irq(priv->txi_irq);
        }
    }

  return ret;
}

static void rzv2h_detach(struct uart_dev_s *dev)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;

  /* Disable interrupts at GIC controller and detach ISRs */
  up_disable_irq(priv->rxi_irq);
  up_disable_irq(priv->txi_irq);
  irq_detach(priv->rxi_irq);
  irq_detach(priv->txi_irq);
}

static int rzv2h_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  return -ENOTTY;
}

static int rzv2h_receive(struct uart_dev_s *dev, unsigned int *status)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;

  /* Read character from Receive Data Register (RDR)
   * Ref: Renesas SCI-B User's Manual - RDR register
   */
  *status = 0;
  return getreg8(priv->scibase + R_SCI_RDR_OFFSET);
}

static void rzv2h_rxint(struct uart_dev_s *dev, bool enable)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;
  uint8_t scr;

  /* Enable/disable RX interrupt at SCI peripheral level (RIE bit in SCR) */
  scr = getreg8(priv->scibase + R_SCI_SCR_OFFSET);
  if (enable)
    {
      scr |= SCI_SCR_RIE;
    }
  else
    {
      scr &= ~SCI_SCR_RIE;
    }
  putreg8(scr, priv->scibase + R_SCI_SCR_OFFSET);
}

static bool rzv2h_rxavailable(struct uart_dev_s *dev)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;

  /* Check if Receive Data Full flag (RDRF) is set in SSR
   * Ref: Renesas SCI-B User's Manual - SSR RDRF bit
   */
  return (getreg8(priv->scibase + R_SCI_SSR_OFFSET) & SCI_SSR_RDRF) != 0;
}

static void rzv2h_send(struct uart_dev_s *dev, int ch)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;

  /* Write character to Transmit Data Register (TDR) */
  putreg8((uint8_t)ch, priv->scibase + R_SCI_TDR_OFFSET);
}

static void rzv2h_txint(struct uart_dev_s *dev, bool enable)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;
  uint8_t scr;

  /* Enable/disable TX interrupt at SCI peripheral level (TIE bit in SCR) */
  scr = getreg8(priv->scibase + R_SCI_SCR_OFFSET);
  if (enable)
    {
      scr |= SCI_SCR_TIE;
    }
  else
    {
      scr &= ~SCI_SCR_TIE;
    }
  putreg8(scr, priv->scibase + R_SCI_SCR_OFFSET);
}

static bool rzv2h_txready(struct uart_dev_s *dev)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;

  /* Check if Transmit Data Empty flag (TDRE) is set in SSR
   * Ref: Renesas SCI-B User's Manual - SSR TDRE bit
   */
  return (getreg8(priv->scibase + R_SCI_SSR_OFFSET) & SCI_SSR_TDRE) != 0;
}

static bool rzv2h_txempty(struct uart_dev_s *dev)
{
  struct rzv2h_dev_s *priv = (struct rzv2h_dev_s *)dev->priv;

  /* Check if Transmit End flag (TEND) is set in SSR */
  return (getreg8(priv->scibase + R_SCI_SSR_OFFSET) & SCI_SSR_TEND) != 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arm_serialinit
 *
 * Description:
 *   Initialize the serial character driver.
 *   Ref: NuttX architecture serial character driver registration interface
 *
 ****************************************************************************/

void arm_serialinit(void)
{
  uart_register("/dev/console", &g_uart0port);
  uart_register("/dev/ttyS0", &g_uart0port);
}
