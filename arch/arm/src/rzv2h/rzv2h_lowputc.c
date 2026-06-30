/****************************************************************************
 * arch/arm/src/rzv2h/rzv2h_lowputc.c
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
#include <stdint.h>

#include "arm_internal.h"
#include "hardware/rzv2h_memorymap.h"
#include "hardware/rzv2h_sci.h"
#include "rzv2h_lowputc.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arm_lowputc
 *
 * Description:
 *   Output one byte on the serial console.
 *   Ref: RA4 ra_lowputc.c transmission mechanism
 *
 ****************************************************************************/

void arm_lowputc(char ch)
{
#ifdef HAVE_CONSOLE
  /* Wait until Transmit Data Register Empty / Transmit End (TEND) is set to 1.
   * Ref: Renesas SCI-B hardware specification for serial status register (SSR).
   */
  while ((getreg8(RZV2H_SCI0_BASE + R_SCI_SSR_OFFSET) & SCI_SSR_TEND) == 0)
    {
      /* Wait for transmit empty */
    }

  /* Send the character */
  putreg8((uint8_t)ch, RZV2H_SCI0_BASE + R_SCI_TDR_OFFSET);
#endif
}

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support OS debug writes.
 *
 ****************************************************************************/

void up_putc(int ch)
{
#ifdef HAVE_CONSOLE
  arm_lowputc(ch);
#endif
}

/****************************************************************************
 * Name: rzv2h_lowsetup
 *
 * Description:
 *   Perform basic initialization of the SCI UART used for the serial console.
 *   Ref: RA4 ra_lowputc.c low-setup mechanism
 *
 ****************************************************************************/

void rzv2h_lowsetup(void)
{
#ifdef HAVE_CONSOLE
  /* 1. Disable TX and RX first.
   * Ref: Renesas SCI-B User's Manual - write 0 to SCR to disable all interrupts/TX/RX.
   */
  putreg8(0, RZV2H_SCI0_BASE + R_SCI_SCR_OFFSET);

  /* 2. Configure default serial mode (8-bit, 1 stop bit, no parity, internal clock).
   * Ref: Renesas SCI-B User's Manual - SMR write 0 for standard UART 8N1.
   */
  putreg8(0, RZV2H_SCI0_BASE + R_SCI_SMR_OFFSET);

  /* 3. Configure baudrate divisor.
   * Ref: RA4 ra_lowsetup - writing 8 to BRR sets up standard baudrate.
   */
  putreg8(8, RZV2H_SCI0_BASE + R_SCI_BRR_OFFSET);

  /* 4. Enable Transmission and Reception.
   * Ref: Renesas SCI-B User's Manual - SCR TE and RE bits enable peripheral.
   */
  putreg8(SCI_SCR_TE | SCI_SCR_RE, RZV2H_SCI0_BASE + R_SCI_SCR_OFFSET);
#endif
}

/****************************************************************************
 * Name: arm_earlyserialinit
 *
 * Description:
 *   Initialize the console serial port early in boot.
 *
 ****************************************************************************/

#ifdef USE_EARLYSERIALINIT
void arm_earlyserialinit(void)
{
  rzv2h_lowsetup();
}
#endif
