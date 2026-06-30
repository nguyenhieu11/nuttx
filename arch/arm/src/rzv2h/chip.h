/****************************************************************************
 * arch/arm/src/rzv2h/chip.h
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

#ifndef __ARCH_ARM_SRC_RZV2H_CHIP_H
#define __ARCH_ARM_SRC_RZV2H_CHIP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "hardware/rzv2h_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Ref: Zephyr PR #91394 dts/arm/renesas/rz/rzv/r9a09g057_cr8.dtsi
 * The GIC Distributor is at 0x12c11000 and the CPU Interface is at 0x12c10100.
 * Thus the private peripheral base address (PERIPHBASE) is 0x12c10000.
 */
#define CHIP_MPCORE_VBASE     0x12c10000

/* Ref: Cortex-R8 MPCore TRM - SCU at 0x0, CPU Interface at 0x100, ICD at 0x1000 */
#define MPCORE_ICC_OFFSET     0x0100
#define MPCORE_ICD_OFFSET     0x1000

#endif /* __ARCH_ARM_SRC_RZV2H_CHIP_H */
