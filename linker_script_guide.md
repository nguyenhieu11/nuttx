# A Practical Guide to Linker Scripts in Embedded Systems

A linker script is a text file that instructs the linker (`ld`) on how to map compile-time symbols and object code sections into the physical memory regions of a target micro-controller (MCU) or processor.

This guide takes you from simple to complex configurations, referencing actual files in your NuttX workspace as evidence of these patterns.

---

## Part 1: Simple Linker Scripts (The Basics)

A simple linker script has two primary jobs:
1.  **Define the memory layout** of the chip (What is the size and start address of FLASH and RAM?).
2.  **Assign input sections** (like executable code, read-only data, variables) into those memory regions.

### 1. The `MEMORY` command
The `MEMORY` command describes the layout of memory blocks.
*   **Format:** `name (attr) : ORIGIN = start, LENGTH = len`
*   Attributes: `r` (read-only), `w` (read/write), `x` (executable).

### 2. The `SECTIONS` command
The `SECTIONS` command tells the linker how to group input files' sections into output sections and where to place them in memory.
*   `*(.text)`: Match the `.text` section (executable code) of **all** (`*`) input compiled object files.
*   `> flash`: Output this section into the `flash` memory defined in the `MEMORY` block.

---

### 📂 Evidence & Reference: The Simple Model
Look at the NuttX linker script for the LM3S6965 board:
🔗 [ld.script](nuttx/boards/arm/tiva/lm3s6965-ek/scripts/ld.script#L27-L52)

Here is a simplified view of this simple configuration:

```ld
/* 1. Define Memory Layout */
MEMORY
{
  flash (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  sram (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

ENTRY(_stext) /* Specify the entry point (the first instruction) */

/* 2. Define Section Placements */
SECTIONS
{
    .text : {
        _stext = ABSOLUTE(.); /* Define a helper symbol at the start address */
        *(.vectors)           /* Place vector table at the very beginning of Flash */
        *(.text .text.*)       /* Place all code */
        *(.rodata .rodata.*)   /* Place read-only data (constants) */
        _etext = ABSOLUTE(.); /* Define a helper symbol at the end of code */
    } > flash                 /* Route this whole block to FLASH */
}
```

### 📊 Visualizing Simple Placement

In this basic model, everything defined inside the `.text` section is packed sequentially and written directly to the target memory region (`flash`). No runtime copy operations or initialization are handled yet.

```text
PHYSICAL FLASH MEMORY (ORIGIN: 0x00000000, LENGTH: 256K)
┌────────────────────────────────────────────────────────┐ <─── Location of '_stext' (Start of code)
│ Interrupt Vectors (*(.vectors))                        │
├────────────────────────────────────────────────────────┤
│ Executable Instructions (*(.text .text.*))             │
├────────────────────────────────────────────────────────┤
│ Constant Data / Strings (*(.rodata .rodata.*))         │
└────────────────────────────────────────────────────────┘ <─── Location of '_etext' (End of code)
```

---

## Part 2: Intermediate Linker Scripts (VMA vs. LMA & Symbols)

For real-world embedded software, initialized variables (`.data` section, like `int count = 10;`) present a challenge:
*   They must be **stored** in non-volatile flash memory (so they don't disappear when powered off).
*   They must **run** from RAM (so they can be updated during runtime).

To solve this, we use two different addresses:
*   **LMA (Load Memory Address):** Where the section is physically loaded in FLASH.
*   **VMA (Virtual Memory Address):** Where the code expects the section to exist at runtime (RAM).

### 1. The `AT` Keyword
The `AT` keyword specifies the LMA.
*   `> sram AT > flash`: Set the VMA to `sram` and the LMA to `flash`.

### 2. Exporting Symbols for Startup Code
The linker script defines pointers (symbols) like `_sdata` (start of data) and `_edata` (end of data). The startup assembly code of the CPU reads these symbols and copies the initialized values from FLASH to RAM at boot.

---

### 📂 Evidence & Reference: The Data/BSS Model
Look again at the LM3S6965 script:
🔗 [ld.script](nuttx/boards/arm/tiva/lm3s6965-ek/scripts/ld.script#L71-L91)

```ld
    _eronly = ABSOLUTE(.); /* Pointer to the source of initialized data in FLASH */

    /* Copy .data from flash (LMA) to sram (VMA) */
    .data : ALIGN(4) {
        _sdata = ABSOLUTE(.);  /* Start of RAM copy destination */
        *(.data .data.*)       /* Compile-time initialized variables */
        _edata = ABSOLUTE(.);  /* End of RAM copy destination */
    } > sram AT > flash        /* VMA is sram, LMA is flash */

    /* Initialize uninitialized variables (.bss) to 0 in RAM */
    .bss : ALIGN(4) {
        _sbss = ABSOLUTE(.);   /* Start of BSS */
        *(.bss .bss.*)         /* Global uninitialized variables */
        *(COMMON)
        _ebss = ABSOLUTE(.);   /* End of BSS */
    } > sram                   /* VMA is sram (no LMA copy needed) */
```

At boot, NuttX's early architecture setup reads `_eronly`, `_sdata`, and `_edata` and executes a loop equivalent to:
`memcpy(_sdata, _eronly, _edata - _sdata);`

### 📊 Visualizing Memory Placement (Load-time vs. Run-time)

Here is a visual map showing how the linker maps these sections, and how they physically transition from programming (flash) to execution (RAM) at boot:

#### 1. At Programming / Load Time (Power-off state)
At load time, the flashing utility writes the entire binary payload into the FLASH memory. RAM remains uninitialized.

```text
PHYSICAL FLASH MEMORY (e.g. 0x00000000)
┌────────────────────────────────────────────────────────┐
│ Vector Table (.vectors)                                │
├────────────────────────────────────────────────────────┤
│ Executable Code (.text)                                │
├────────────────────────────────────────────────────────┤
│ Read-only constants (.rodata)                          │
├────────────────────────────────────────────────────────┤
│ Initializer payload for .data (LMA image)              │ <─── Positioned at Symbol '_eronly'
└────────────────────────────────────────────────────────┘

PHYSICAL SRAM MEMORY (e.g. 0x20000000)
┌────────────────────────────────────────────────────────┐
│ [ Uninitialized / Garbage values ]                     │
└────────────────────────────────────────────────────────┘
```

#### 2. At Boot / Run-Time (Execution state)
Immediately after reset, before entering the main application (`nuttx_main`), the CPU executes startup code. It reads the exported symbols from the linker script (`_eronly`, `_sdata`, `_edata`, `_sbss`, `_ebss`) to map data into SRAM.

```text
PHYSICAL FLASH MEMORY (e.g. 0x00000000)
┌────────────────────────────────────────────────────────┐
│ Vector Table (.vectors)                                │
├────────────────────────────────────────────────────────┤
│ Executable Code (.text)                                │
├────────────────────────────────────────────────────────┤
│ Read-only constants (.rodata)                          │
├────────────────────────────────────────────────────────┤
│ Initializer payload for .data (LMA image)              │
└──────────────────────────┬─────────────────────────────┘
                           │
                           │ CPU copies initialized bytes
                           │ (memcpy using _eronly, _sdata, _edata)
                           ▼
PHYSICAL SRAM MEMORY (e.g. 0x20000000)
┌────────────────────────────────────────────────────────┐
│ Active Variables (.data VMA image)                     │ <─── Address: [_sdata to _edata]
├────────────────────────────────────────────────────────┤
│ Zeroed variables (.bss)                                │ <─── Address: [_sbss to _ebss] (Cleared to 0)
├────────────────────────────────────────────────────────┤
│ Active Application Heap & Call Stacks                  │ <─── Rest of SRAM
└────────────────────────────────────────────────────────┘
```

---

## Part 3: Complex Linker Scripts (Multi-Memory & Partitioning)

As SoCs grow more complex, they present multiple memory banks with different speeds and capabilities:
*   **DTCM (Data Tightly Coupled Memory):** Ultra-fast RAM for core registers, interrupt stacks, or critical variables.
*   **ITCM (Instruction Tightly Coupled Memory):** Ultra-fast RAM for executing critical code loops.
*   **External SDRAM:** Large, slower RAM.
*   **Isolated Kernel/User Regions:** For secure MPU (Memory Protection Unit) configurations.

### 1. Partitioning the `MEMORY` block
Instead of defining one large block, we carve the memory space into specialized partitions.

---

### 📂 Evidence & Reference: Complex Multi-Memory layout
Take a look at the memory configuration of the Nucleo-F746ZG board in NuttX:
🔗 [memory.ld](nuttx/boards/arm/stm32f7/nucleo-f746zg/scripts/memory.ld#L94-L116)

```ld
MEMORY
{
  itcm  (rwx) : ORIGIN = 0x00200000, LENGTH = 1024K  /* Instruction TCM */
  dtcm  (rwx) : ORIGIN = 0x20000000, LENGTH = 64K    /* Data TCM */

  /* 1024KB FLASH split for Secure MPU: Kernel-space vs. User-space */
  kflash (rx) : ORIGIN = 0x08000000, LENGTH = 128K   /* Protected Kernel Code */
  uflash (rx) : ORIGIN = 0x08020000, LENGTH = 128K   /* User Applications Code */
  xflash (rx) : ORIGIN = 0x08040000, LENGTH = 768K   /* Main storage */

  /* 240KB SRAM split */
  ksram (rwx) : ORIGIN = 0x20010000, LENGTH = 4K     /* Kernel RAM variables */
  usram (rwx) : ORIGIN = 0x20011000, LENGTH = 4K     /* User RAM variables */
  xsram (rwx) : ORIGIN = 0x20012000, LENGTH = 232K   /* Application Heap */
}
```

### 2. Targeting Specific Modules
In the `SECTIONS` configuration of a complex script, we route specific modules or objects into these partitions.
For example, to execute interrupt handlers or math routines from ITCM, or store state in DTCM:

```ld
SECTIONS
{
    /* Code to run from ITCM */
    .itcm_code : {
        _sitcm = .;
        *(.itcm.text)
        *libfastmath.a:*(.text .text.*) /* Map entire math library code into ITCM */
        _eitcm = .;
    } > itcm AT > kflash

    /* Data stored in DTCM */
    .dtcm_data : {
        _sdtcm = .;
        *(.dtcm.data)
        _edtcm = .;
    } > dtcm AT > kflash
}
```

### 📊 Visualizing Complex Layout (Multi-Memory & MPU Partitioning)

In a complex system, the linker works with multiple disconnected memory domains. The diagram below illustrates how:
1. Physical FLASH is divided into Kernel (`kflash`) and User (`uflash`) domains for MPU protection.
2. Fast Tightly-Coupled Memories (`itcm` and `dtcm`) are loaded from `kflash` (LMA) at boot and mapped to their respective TCM addresses (VMA) for zero-latency execution.
3. System RAM is partitioned to isolate Kernel-level state (`ksram`) from general Application Heap space (`xsram`).

```text
 PHYSICAL FLASH MEMORY (LMA)               PHYSICAL RAM & TCM TARGETS (VMA)
 ┌───────────────────────────┐             ┌───────────────────────────┐
 │ kflash (0x08000000)       │             │ ITCM (0x00200000)         │
 │ ┌───────────────────────┐ │             │ ┌───────────────────────┐ │
 │ │ ITCM Code Load Image  ├─┼────────────>│ │ Executable ITCM Code  │ │ (Fast loop execution)
 │ ├───────────────────────┤ │             │ └───────────────────────┘ │
 │ │ DTCM Data Load Image  ├─┼────────┐    └───────────────────────────┘
 │ ├───────────────────────┤ │        │    ┌───────────────────────────┐
 │ │ Kernel Code           │ │        │    │ DTCM (0x20000000)         │
 │ └───────────────────────┘ │        │    │ ┌───────────────────────┐ │
 └───────────────────────────┘        └───>│ │ DTCM Data/Variables   │ │ (Zero-wait data)
 ┌───────────────────────────┐             │ └───────────────────────┘ │
 │ uflash (0x08020000)       │             └───────────────────────────┘
 │ ┌───────────────────────┐ │             ┌───────────────────────────┐
 │ │ User Applications     │ │             │ KSRAM (0x20010000)        │
 │ └───────────────────────┘ │             │ ┌───────────────────────┐ │
 └───────────────────────────┘             │ │ Kernel .data & .bss   │ │ (Isolated Kernel RAM)
                                           │ └───────────────────────┘ │
                                           └───────────────────────────┘
                                           ┌───────────────────────────┐
                                           │ XSRAM (0x20012000)        │
                                           │ ┌───────────────────────┐ │
                                           │ │ App Data, Stack, Heap │ │ (Application Area)
                                           │ └───────────────────────┘ │
                                           └───────────────────────────┘
```

---

## 4. Checklist: How to Create a Linker Script for a New Board

If you are writing a linker script for a new board from scratch in NuttX:

1.  **Read the MCU Datasheet:** Find the exact Memory Map (Flash origin and length, SRAM origin and length).
2.  **Determine your build mode:** 
    *   *Flat Build (Standard):* Create a simple layout using one Flash and one RAM region.
    *   *Kernel/Protected Build:* Subdivide Flash and SRAM into distinct Kernel and User regions.
3.  **Identify Boot Vector placement:** Find out where the CPU reads its reset vector (usually start of Flash) and place `*(.vectors)` or `*(.exvectors)` there.
4.  **Copy and adapt a template:** Avoid writing all standard system alignment configurations (`.init_array`, `.ctors`, debugging stabs) from scratch. Copy a working configuration from a similar architecture in `boards/` (e.g., STM32, Tiva) and adjust the `MEMORY` origin/length definitions in the script.
