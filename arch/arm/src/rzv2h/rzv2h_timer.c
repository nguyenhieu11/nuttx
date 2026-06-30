/****************************************************************************
 * arch/arm/src/rzv2h/rzv2h_timer.c
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
#include <nuttx/timers/arch_alarm.h>

#include "arm_timer.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_timer_initialize
 *
 * Description:
 *   Initialize the system timer.
 *   Ref: Zephyr PR #91394 dts/arm/renesas/rz/rzv/r9a09g057_cr8.dtsi
 *   The Cortex-R8 core includes standard ARMv8 Architecture Timer ("arm,armv8-timer").
 *   We initialize it and register the alarm lowerhalf interface.
 *
 ****************************************************************************/

void up_timer_initialize(void)
{
  /* Initialize standard ARM architecture timer.
   * Ref: arch/arm/src/armv7-r/arm_timer.c generic driver
   */
  up_alarm_set_lowerhalf(arm_timer_initialize(0));
}
