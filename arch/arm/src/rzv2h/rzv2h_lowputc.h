/****************************************************************************
 * arch/arm/src/rzv2h/rzv2h_lowputc.h
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

#ifndef __ARCH_ARM_SRC_RZV2H_RZV2H_LOWPUTC_H
#define __ARCH_ARM_SRC_RZV2H_RZV2H_LOWPUTC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Ref: boards/arm/rzv2h/rzv2h-evk/configs/nsh/defconfig
 * We configure SCI0 as the default serial console.
 */
#if defined(CONFIG_RZV2H_SCI0)
#  define HAVE_CONSOLE 1
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__

void rzv2h_lowsetup(void);
void arm_lowputc(char ch);

#endif /* __ASSEMBLY__ */

#endif /* __ARCH_ARM_SRC_RZV2H_RZV2H_LOWPUTC_H */
