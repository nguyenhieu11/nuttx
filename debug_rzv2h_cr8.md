# Debugging Renesas RZ/V2H Cortex-R8 with GDB and SEGGER Ozone

This guide outlines the procedures for loading, running, and debugging NuttX firmware on the **Cortex-R8 (CR8)** cores of the **Renesas RZ/V2H EVK** using both CLI GDB and SEGGER Ozone.

---

## Method A: Debugging with SEGGER Ozone (GUI)

Ozone is a standalone graphical debugger. It loads symbols directly from your ELF file and downloads code using J-Link.

### 1. Project Setup
1. Launch **Ozone**.
2. Start the **New Project Wizard**:
   * **Device:** Search for and select `R9A09G057_CR8`. *(If your J-Link software version doesn't list it yet, choose `Cortex-R8` as a generic device).*
   * **Register Set:** Select `Cortex-R8`.
   * **Target Interface:** Select `JTAG`.
   * **Speed:** Select `4000 kHz` (4 MHz).
   * **Host Interface:** Select `USB`.
3. In the next step:
   * **Program File:** Browse to the root of your `nuttx_ws/nuttx` directory and select the compiled binary file **`nuttx`** (the ELF binary).

### 2. Download and Run
* Press **`F5`** (or click the green **Download & Reset** button).
* Ozone will connect, clear the Cortex-R8 SRAM, write your NuttX binary starting at address `0x00000000`, and halt execution at `arm_boot()`.
* You can now step through code, watch local variables, examine registers, and view standard disassembly.

---

## Method B: Debugging with GDB (CLI)

This is the standard command-line workflow utilizing J-Link GDB Server.

### 1. Launch J-Link GDB Server
Open a terminal window and launch the server:
```bash
JLinkGDBServer -device R9A09G057_CR8 -if JTAG -speed 4000 -port 2331
```
Keep this window open. It acts as the bridge between your PC and the board.

### 2. Launch GDB
Open a second terminal window, navigate to the `nuttx` root directory, and start GDB pointing to the binary:
```bash
arm-none-eabi-gdb nuttx
```
*(Verify you see `Reading symbols from nuttx...done.` to ensure GDB successfully loaded the local symbols).*

### 3. Connect, Load, and Run
Enter the following command sequence into the GDB prompt `(gdb)`:
```gdb
# 1. Connect to the local J-Link GDB Server
target remote localhost:2331

# 2. Reset the Cortex-R8 CPU core
monitor reset

# 3. Load the binary segments physically into target SRAM (0x00000000)
load

# 4. Set a breakpoint at early boot initialization
break arm_boot

# 5. Start the program
continue
```
* **Verify Load Success:** GDB should print the size and load memory address (LMA) of each segment loaded. If you see transfer speeds and a `Start address` confirmation, the load was successful.
