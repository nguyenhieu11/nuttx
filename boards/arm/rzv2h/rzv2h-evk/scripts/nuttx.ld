/****************************************************************************
 * boards/arm/rzv2h/rzv2h-evk/scripts/nuttx.ld
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

/* Ref: Zephyr PR #91394 rzv2h_evk_r9a09g057h44gbg_cr8_0.dts
 * The memory node defines mmio-sram at address 0x0 with 256M size.
 */
MEMORY
{
  ram (rwx) : ORIGIN = 0x00000000, LENGTH = 256M
}

OUTPUT_ARCH(arm)
ENTRY(__start)

SECTIONS
{
  .text : {
      _stext = .;            /* Text section start */
      __text_start = .;
      *(.vectors)            /* Interrupt vector table */
      *(.text*)
      *(.fixup)
      *(.gnu.warning)
      *(.rodata*)            /* Read-only data */
      *(.data.rel.ro*)
      . = ALIGN(4096);
      _etext = .;            /* Text section end */
  } > ram

  .init_section : {
      _sinit = ABSOLUTE(.);
      KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
      KEEP(*(.init_array EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o) .ctors))
      _einit = ABSOLUTE(.);
  } > ram

  .data : {                  /* Data section */
      _sdata = .;
      *(.data*)
      . = ALIGN(4);
      _edata = .;
  } > ram

  _eronly = LOADADDR(.data);

  .bss : {                   /* BSS section */
      _sbss = .;
      *(.bss*)
      *(COMMON)
      . = ALIGN(4096);
      _ebss = .;
  } > ram

  /* Sections to be discarded */
  /DISCARD/ : {
      *(.exit.text)
      *(.exit.data)
      *(.exitcall.exit)
  }
}
