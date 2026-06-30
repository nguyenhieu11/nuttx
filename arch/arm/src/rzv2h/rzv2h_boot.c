/****************************************************************************
 * arch/arm/src/rzv2h/rzv2h_boot.c
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
#include <assert.h>
#include <nuttx/debug.h>

#include "arm_internal.h"
#include "sctlr.h"
#include "mpu.h"
#include "chip.h"
#include <nuttx/init.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rzv2h_mpu_initialize
 *
 * Description:
 *   Configure the MPU memory regions for RZ/V2H.
 *   Ref: Zephyr PR #91394 (soc/renesas/rz/rzv2h/cr8/arm_mpu_regions.c)
 *
 ****************************************************************************/

#ifdef CONFIG_ARM_MPU
void rzv2h_mpu_initialize(void)
{
  /* Region 0: SRAM/TCM (0x00000000 - 0x0FFFFFFF, 256MB)
   * Cacheable, Bufferable, Shareable, Read/Write access.
   */
  mpu_configure_region(0x00000000, 256 * 1024 * 1024,
                       MPU_RACR_C | MPU_RACR_B | MPU_RACR_S | MPU_RACR_AP_RWRW);

  /* Region 1: Peripheral Registers (0x10000000 - 0x1FFFFFFF, 256MB)
   * Strongly-ordered/Device memory, Non-executable, Read/Write access.
   */
  mpu_peripheral(0x10000000, 256 * 1024 * 1024);

  /* Region 2: Flash memory (0x20000000 - 0x2FFFFFFF, 256MB)
   * Cacheable, Shareable, Executable, Read-Only access.
   */
  mpu_configure_region(0x20000000, 256 * 1024 * 1024,
                       MPU_RACR_C | MPU_RACR_S | MPU_RACR_AP_RORO);

  /* Enable MPU */
  mpu_control(true);
}
#endif

/****************************************************************************
 * Name: arm_boot
 *
 * Description:
 *   Complete boot operations started in arm_head.S
 *
 ****************************************************************************/

void arm_boot(void)
{
  /* Configure the FPU
   * Ref: ARM Cortex-R8 MPCore TRM - VFPv3-D16 FPU is present
   */
  arm_fpuconfig();

#ifdef CONFIG_ARM_MPU
  /* Configure memory regions using MPU */
  rzv2h_mpu_initialize();
#endif

  /* Copy data segment and clear BSS */
  arm_data_initialize();

#ifdef USE_EARLYSERIALINIT
  /* Initialize console UART early for boot diagnostics */
  arm_earlyserialinit();
#endif

  /* Perform board-level boot initialization if defined */
#ifdef CONFIG_BOARD_INITIALIZE
  board_initialize();
#endif

  /* Jump to NuttX entry point (does not return)
   * Ref: arch/arm/src/armv7-r/arm_head.S architecture startup design
   */
  nx_start();
}
