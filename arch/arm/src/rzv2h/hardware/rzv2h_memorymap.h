/****************************************************************************
 * arch/arm/src/rzv2h/hardware/rzv2h_memorymap.h
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

#ifndef __ARCH_ARM_SRC_RZV2H_HARDWARE_RZV2H_MEMORYMAP_H
#define __ARCH_ARM_SRC_RZV2H_HARDWARE_RZV2H_MEMORYMAP_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Ref: Zephyr PR #91394 dts/arm/renesas/rz/rzv/r9a09g057_cr8.dtsi
 * sci0 (SCI-B UART) base address is 0x12800c00
 */
#define RZV2H_SCI0_BASE       0x12800c00

/* Ref: Linux Device Tree - r9a09g057.dtsi
 * ostm0 (OS Timer 0) base address is 0x11800000
 */
#define RZV2H_OSTM0_BASE      0x11800000

#endif /* __ARCH_ARM_SRC_RZV2H_HARDWARE_RZV2H_MEMORYMAP_H */
