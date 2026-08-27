<p align="center">
  <img src="docs/figures/astrarv32_logo_riscv.png" alt="AstraRV32 logo" width="380">
</p>

<h1 align="center">AstraRV32</h1>

<p align="center">
  <b>A Configurable, Extensible 32-bit RISC-V Embedded SoC Platform</b>
</p>

<p align="center">
  <img alt="License" src="https://img.shields.io/badge/License-BSD--3--Clause-blue.svg">
  <img alt="ISA" src="https://img.shields.io/badge/RISC--V-RV32I%2FE-informational">
  <img alt="HDL" src="https://img.shields.io/badge/HDL-VHDL--2008-orange">
  <img alt="Toolchain" src="https://img.shields.io/badge/Toolchain-GCC%20%2F%20GHDL-lightgrey">
</p>

<p align="center">
  <i>
    Built on the RISC-V processor platform by Revanth Kumar kalla,
    released under the BSD 3-Clause License.
  </i>
</p>

---

## Overview

AstraRV32 is a configurable 32-bit RISC-V CPU and microcontroller-class SoC platform designed for integration into larger digital systems or standalone embedded applications.

The platform is organized around a portable VHDL implementation of the CPU and SoC, a C-based software framework, simulation infrastructure, and JTAG-based on-chip debugging support.

The architecture is highly configurable. Optional memories, caches, timers, serial interfaces, debugging infrastructure, and other peripherals can be enabled or disabled through VHDL generics, allowing the same platform to scale across different embedded-system requirements.

## Table of Contents

- [Key Features](#key-features)
- [Architecture](#architecture)
- [Repository Structure](#repository-structure)
- [Getting Started](#getting-started)
- [Configuration](#configuration)
- [Documentation](#documentation)
- [About This Build](#about-this-build)
- [License & Acknowledgments](#license--acknowledgments)

---

## Key Features

### CPU / ISA

- 32-bit RISC-V processor architecture
- Support for the RV32I and RV32E base ISA configurations
- Optional standard RISC-V extensions including:
  - `M` — Integer multiplication and division
  - `C` — Compressed instructions
  - `E` — Embedded register configuration
  - `U` — User-mode support
  - `Zba`, `Zbb`, `Zbc`, `Zbs` — Bit-manipulation extensions
  - `Zkn*`, `Zks*` — Cryptographic extensions
  - `Zfinx` — Floating-point instructions using the integer register file
  - `Zaamo`, `Zalrsc` — Atomic memory operations
- Optional dual-core configuration
- Custom Functions Unit (CFU) support
- Custom Functions Subsystem (CFS) support
- Physical Memory Protection (PMP)
- Hardware Performance Monitors (HPM)
- Execution tracing / RVFI-compatible interfaces

### SoC / Peripherals

- UART interfaces
- SPI host/device interfaces
- TWI host/device interfaces
- GPIO
- PWM
- Watchdog timer
- True Random Number Generator (TRNG)
- 1-Wire interface
- NeoPixel-compatible smart LED interface
- Configurable instruction and data memories
- Instruction and data caches
- DMA controller
- Serial memory controller
- Execute-in-place (XIP) support
- Technology-independent XBUS external bus interface

### Debug & Bring-up

- JTAG-based on-chip debugger
- OpenOCD/GDB integration
- Optional debugger authentication
- Hardware breakpoints
- UART-based bootloader
- FPGA-oriented implementation
- Portable VHDL-2008 RTL without vendor-specific primitives

### Software & Ecosystem

- RISC-V GCC-based C software framework
- Hardware abstraction layer (HAL)
- Linker scripts and startup code
- Peripheral driver libraries
- Example applications
- Bootloader
- On-chip debugger firmware
- OpenOCD configuration
- CMSIS-SVD system description
- GHDL-based simulation environment
- Integration support for external FPGA/SoC development environments

---

## Architecture

<p align="center">
  <img
    src="docs/figures/astrarv32_processor.png"
    alt="AstraRV32 processor block diagram"
    width="640"
  >
</p>

.
├── docs/
│   ├── datasheet/            # SoC datasheet and architecture documentation
│   ├── figures/              # Architecture and project figures
│   ├── userguide/            # User guide and tutorials
│   ├── astrarv32-theme.yml   # Documentation theme configuration
│   ├── attrs.adoc            # Documentation attributes
│   ├── Doxyfile              # Doxygen configuration
│   ├── doxygen_main.md       # Doxygen entry point
│   ├── legal.adoc            # License and legal information
│   └── Makefile              # Documentation build system
│
├── rtl/
│   ├── core/                 # CPU and SoC RTL
│   ├── system_integration/   # System-level integration wrappers
│   ├── test_setups/          # Board-independent test configurations
│   ├── verilog/              # Verilog export
│   ├── file_list_core.f      # RTL file list
│   ├── file_list_regenerate.sh
│   └── README.md
│
├── sim/
│   ├── astrarv32_tb.vhd      # Main simulation testbench
│   ├── ghdl.sh               # GHDL simulation script
│   ├── jtag_dmi_pkg.vhd      # JTAG/DMI simulation support
│   ├── psram_model.vhd       # PSRAM simulation model
│   ├── sim_uart_rx.vhd       # UART simulation model
│   ├── xbus_fmem.vhd         # XBUS memory model
│   ├── xbus_gateway.vhd      # XBUS gateway
│   ├── xbus_memory.vhd       # XBUS memory model
│   └── README.md
│
└── sw/
    ├── bootloader/            # UART bootloader
    ├── common/                # Startup code, linker scripts, Makefiles
    ├── example/               # Example applications
    ├── image_gen/             # Software image generation utilities
    ├── lib/                   # HAL and peripheral drivers
    ├── ocd-firmware/          # On-chip debugger firmware
    ├── openocd/               # OpenOCD configuration
    ├── svd/                   # CMSIS-SVD description
    └── README.md

AstraRV32 combines the RISC-V CPU with an internal bus fabric, configurable memories, and memory-mapped peripherals.

Optional components are controlled through VHDL generics. This allows unused functionality to be eliminated during synthesis rather than remaining as inactive runtime logic.

External memories and peripherals can be connected through the platform's **XBUS** interface.

For detailed architectural information, memory maps, interfaces, and module descriptions, see the documentation under:

```text
docs/datasheet/
docs/userguide/


