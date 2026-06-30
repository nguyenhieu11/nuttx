/****************************************************************************
 * arch/arm/src/rzv2h/rzv2h_irq.c
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
#include <nuttx/arch.h>

#include "arm_internal.h"
#include "sctlr.h"
#include "gic.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Size of the interrupt stack allocation */
#define INTSTACK_ALLOC (CONFIG_SMP_NCPUS * INTSTACK_SIZE)

/****************************************************************************
 * Public Data
 ****************************************************************************/

#if defined(CONFIG_SMP) && CONFIG_ARCH_INTERRUPTSTACK > 7
/* SMP configurations need custom aligned interrupt stacks for each CPU core */
static uint64_t g_irqstack_alloc[INTSTACK_ALLOC >> 3];
static uint64_t g_fiqstack_alloc[INTSTACK_ALLOC >> 3];

uintptr_t g_irqstack_top[CONFIG_SMP_NCPUS] =
{
  (uintptr_t)g_irqstack_alloc + INTSTACK_SIZE,
#if CONFIG_SMP_NCPUS > 1
  (uintptr_t)g_irqstack_alloc + (2 * INTSTACK_SIZE),
#endif
#if CONFIG_SMP_NCPUS > 2
  (uintptr_t)g_irqstack_alloc + (3 * INTSTACK_SIZE),
#endif
#if CONFIG_SMP_NCPUS > 3
  (uintptr_t)g_irqstack_alloc + (4 * INTSTACK_SIZE)
#endif
};

uintptr_t g_fiqstack_top[CONFIG_SMP_NCPUS] =
{
  (uintptr_t)g_fiqstack_alloc + INTSTACK_SIZE,
#if CONFIG_SMP_NCPUS > 1
  (uintptr_t)g_fiqstack_alloc + 2 * INTSTACK_SIZE,
#endif
#if CONFIG_SMP_NCPUS > 2
  (uintptr_t)g_fiqstack_alloc + 3 * INTSTACK_SIZE,
#endif
#if CONFIG_SMP_NCPUS > 3
  (uintptr_t)g_fiqstack_alloc + 4 * INTSTACK_SIZE
#endif
};
#endif

/* Symbols defined in the linker script to find the vector table */
extern uint8_t _vector_start[];

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_irqinitialize
 *
 * Description:
 *   Initialize the interrupt subsystem and ready it for action.
 *
 ****************************************************************************/

void up_irqinitialize(void)
{
#ifdef CONFIG_ARCH_LOWVECTORS
  /* Set the VBAR (Vector Base Address Register) to the address of our vector table.
   * Ref: ARM Cortex-R8 MPCore TRM - CP15 c12 VBAR register configures vectors.
   */
  DEBUGASSERT((((uintptr_t)_vector_start) & ~VBAR_MASK) == 0);
  cp15_wrvbar((uint32_t)_vector_start);
#endif

  /* Ref: ARM Generic Interrupt Controller v1.0 architecture (Cortex-R8 standard)
   * We reuse NuttX's built-in GICv2/v1 distributor and CPU interface logic.
   */
  if (sched_getcpu() == 0)
    {
      arm_gic0_initialize();  /* Initialize distributor (only on CPU0) */
    }

  arm_gic_initialize();       /* Initialize GIC CPU interface on this core */

#ifndef CONFIG_SUPPRESS_INTERRUPTS
  /* Set stack coloration for stack overflow detection and enable core interrupts */
  arm_color_intstack();
  up_irq_enable();
#endif
}

/****************************************************************************
 * Name: up_get_intstackbase
 *
 * Description:
 *   Return the start address of the interrupt stack for the specified CPU.
 *
 ****************************************************************************/

#if defined(CONFIG_SMP) && CONFIG_ARCH_INTERRUPTSTACK > 7
uintptr_t up_get_intstackbase(int cpu)
{
  return g_irqstack_top[cpu] - INTSTACK_SIZE;
}
#endif
