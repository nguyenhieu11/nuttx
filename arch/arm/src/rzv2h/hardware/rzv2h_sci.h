/****************************************************************************
 * arch/arm/src/rzv2h/hardware/rzv2h_sci.h
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
 *   http://www.bg.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_RZV2H_HARDWARE_RZV2H_SCI_H
#define __ARCH_ARM_SRC_RZV2H_HARDWARE_RZV2H_SCI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "hardware/rzv2h_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Register Offsets *********************************************************/

/* Ref: ra4/hardware/ra_sci.h - standard Renesas SCI offsets */
#define R_SCI_SMR_OFFSET          0x0000  /* Serial Mode Register (8-bits) */
#define R_SCI_BRR_OFFSET          0x0001  /* Bit Rate Register (8-bits) */
#define R_SCI_SCR_OFFSET          0x0002  /* Serial Control Register (8-bits) */
#define R_SCI_TDR_OFFSET          0x0003  /* Transmit Data Register (8-bits) */
#define R_SCI_SSR_OFFSET          0x0004  /* Serial Status Register (8-bits) */
#define R_SCI_RDR_OFFSET          0x0005  /* Receive Data Register (8-bits) */
#define R_SCI_SCMR_OFFSET         0x0006  /* Smart Card Mode Register (8-bits) */
#define R_SCI_SEMR_OFFSET         0x0007  /* Serial Extended Mode Register (8-bits) */
#define R_SCI_FCR_OFFSET          0x0014  /* FIFO Control Register (16-bits) */
#define R_SCI_FDR_OFFSET          0x0016  /* FIFO Data Count Register (16-bits) */
#define R_SCI_LSR_OFFSET          0x0018  /* Line Status Register (16-bits) */
#define R_SCI_SPTR_OFFSET         0x001c  /* Serial Port Register (8-bits) */

/* Register Addresses *******************************************************/

#define RZV2H_SCI0_SMR            (RZV2H_SCI0_BASE + R_SCI_SMR_OFFSET)
#define RZV2H_SCI0_BRR            (RZV2H_SCI0_BASE + R_SCI_BRR_OFFSET)
#define RZV2H_SCI0_SCR            (RZV2H_SCI0_BASE + R_SCI_SCR_OFFSET)
#define RZV2H_SCI0_TDR            (RZV2H_SCI0_BASE + R_SCI_TDR_OFFSET)
#define RZV2H_SCI0_SSR            (RZV2H_SCI0_BASE + R_SCI_SSR_OFFSET)
#define RZV2H_SCI0_RDR            (RZV2H_SCI0_BASE + R_SCI_RDR_OFFSET)
#define RZV2H_SCI0_SCMR           (RZV2H_SCI0_BASE + R_SCI_SCMR_OFFSET)
#define RZV2H_SCI0_SEMR           (RZV2H_SCI0_BASE + R_SCI_SEMR_OFFSET)
#define RZV2H_SCI0_FCR            (RZV2H_SCI0_BASE + R_SCI_FCR_OFFSET)
#define RZV2H_SCI0_FDR            (RZV2H_SCI0_BASE + R_SCI_FDR_OFFSET)
#define RZV2H_SCI0_LSR            (RZV2H_SCI0_BASE + R_SCI_LSR_OFFSET)
#define RZV2H_SCI0_SPTR           (RZV2H_SCI0_BASE + R_SCI_SPTR_OFFSET)

/* Register Bit Definitions *************************************************/

/* Serial Mode Register (SMR) */
#define SCI_SMR_CKS_SHIFT         (0)       /* Bits 0-1: Clock Select */
#define SCI_SMR_CKS_MASK          (3 << SCI_SMR_CKS_SHIFT)
#  define SCI_SMR_CKS_DIV1        (0 << SCI_SMR_CKS_SHIFT)
#  define SCI_SMR_CKS_DIV4        (1 << SCI_SMR_CKS_SHIFT)
#  define SCI_SMR_CKS_DIV16       (2 << SCI_SMR_CKS_SHIFT)
#  define SCI_SMR_CKS_DIV64       (3 << SCI_SMR_CKS_SHIFT)
#define SCI_SMR_MP                (1 << 2)  /* Bit 2: Multi-Processor Mode */
#define SCI_SMR_STOP              (1 << 3)  /* Bit 3: Stop Bit Length */
#define SCI_SMR_PM                (1 << 4)  /* Bit 4: Parity Mode */
#define SCI_SMR_PE                (1 << 5)  /* Bit 5: Parity Enable */
#define SCI_SMR_CHR               (1 << 6)  /* Bit 6: Character Length */
#define SCI_SMR_CM                (1 << 7)  /* Bit 7: Communication Mode */

/* Serial Control Register (SCR) */
#define SCI_SCR_CKE_SHIFT         (0)       /* Bits 0-1: Clock Enable */
#define SCI_SCR_CKE_MASK          (3 << SCI_SCR_CKE_SHIFT)
#define SCI_SCR_TEIE              (1 << 2)  /* Bit 2: Transmit End Interrupt Enable */
#define SCI_SCR_MPIE              (1 << 3)  /* Bit 3: Multi-Processor Interrupt Enable */
#define SCI_SCR_RE                (1 << 4)  /* Bit 4: Receive Enable */
#define SCI_SCR_TE                (1 << 5)  /* Bit 5: Transmit Enable */
#define SCI_SCR_RIE               (1 << 6)  /* Bit 6: Receive Interrupt Enable */
#define SCI_SCR_TIE               (1 << 7)  /* Bit 7: Transmit Interrupt Enable */

/* Serial Status Register (SSR) */
#define SCI_SSR_MPBT              (1 << 0)  /* Bit 0: Multi-Processor Bit Transfer */
#define SCI_SSR_MPB               (1 << 1)  /* Bit 1: Multi-Processor */
#define SCI_SSR_TEND              (1 << 2)  /* Bit 2: Transmit End */
#define SCI_SSR_PER               (1 << 3)  /* Bit 3: Parity Error */
#define SCI_SSR_FER               (1 << 4)  /* Bit 4: Framing Error */
#define SCI_SSR_ORER              (1 << 5)  /* Bit 5: Overrun Error */
#define SCI_SSR_RDRF              (1 << 6)  /* Bit 6: Receive Data Full */
#define SCI_SSR_TDRE              (1 << 7)  /* Bit 7: Transmit Data Empty */

/* FIFO Control Register (FCR) */
#define SCI_FCR_FM                (1 << 0)  /* Bit 0: FIFO Mode Select */
#define SCI_FCR_TFRST             (1 << 1)  /* Bit 1: Transmit FIFO Data Register Reset */
#define SCI_FCR_RFRST             (1 << 2)  /* Bit 2: Receive FIFO Data Register Reset */

/* Line Status Register (LSR) */
#define SCI_LSR_ORER              (1 << 0)  /* Bit 0: Overrun Error */

#endif /* __ARCH_ARM_SRC_RZV2H_HARDWARE_RZV2H_SCI_H */
