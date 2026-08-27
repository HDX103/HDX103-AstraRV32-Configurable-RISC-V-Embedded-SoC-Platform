<p align="center"> <img src="docs/figures/astrarv32_logo_riscv.png" alt="AstraRV32 logo" width="380"> </p> <h1 align="center">AstraRV32</h1> <p align="center"><b>A Configurable, Extensible 32-bit RISC-V Embedded SoC Platform</b></p> <p align="center"> <img alt="License" src="https://img.shields.io/badge/License-BSD--3--Clause-blue.svg"> <img alt="ISA" src="https://img.shields.io/badge/RISC--V-RV32I%2FE-informational"> <img alt="HDL" src="https://img.shields.io/badge/HDL-VHDL--2008-orange"> <img alt="Toolchain" src="https://img.shields.io/badge/Toolchain-GCC%20%2F%20GHDL-lightgrey"> </p> <p align="center"><i>Built on the <a href="https://github.com/stnolting/neorv32">NEORV32</a> RISC-V processor platform by Stephan Nolting, released under the BSD 3-Clause License.</i></p>
Overview

AstraRV32 is a configurable 32-bit RISC-V CPU and microcontroller-class SoC, built to drop into a larger chip design as an auxiliary core or to stand alone as a complete embedded MCU. Every non-essential block — caches, internal memories, timers, serial interfaces, the on-chip debugger, even a second CPU core — is an opt-in generic, so the same RTL scales from a minimal RV32E footprint up to a dual-core system with bit-manipulation and cryptography extensions enabled.

The platform ships as three integrated pieces: portable VHDL for the CPU/SoC (no vendor-specific primitives), a C software framework (bootloader, HAL, linker scripts, example applications), and a JTAG on-chip debugger compatible with OpenOCD/GDB.

Table of Contents
Key Features
Architecture
Repository Structure
Getting Started
Configuration
Documentation
About This Build
License & Acknowledgments
Key Features

CPU / ISA

32-bit RISC-V core inheriting full compliance with the official RISC-V architecture test suite
Base ISA + privileged ISA, plus a wide range of optional standard extensions — M, C, E, U, Zba/Zbb/Zbc/Zbs (bit manipulation), Zkn*/Zks* (cryptography), Zfinx (floating point without a dedicated register file), Zaamo/Zalrsc (atomics), and more
Optional dual-core homogeneous SMP configuration
Optional custom functions unit (CFU) for user-defined RISC-V instructions, and a custom functions subsystem (CFS) for tightly-coupled co-processors
Full execution tracing (RVFI-compatible), physical memory protection (PMP), hardware performance monitors (HPM)

SoC / Peripherals

UART ×2, SPI (host + device), TWI (host + device), 1-Wire, GPIO, PWM, watchdog timer, TRNG, and a native NeoPixel-compatible smart-LED interface
Configurable I-cache/D-cache, internal IMEM/DMEM, a DMA controller, and a serial memory controller with XIP support
XBUS external bus interface for attaching custom, processor-external memories and peripherals

Debug & Bring-up

JTAG on-chip debugger, OpenOCD/GDB-compatible, with optional authentication and hardware breakpoints
Internal bootloader with a UART upload interface
FPGA-friendly by design — all internal memories, including the register file, map cleanly to block RAM

Software & Ecosystem

C software framework on RISC-V GCC: HAL, linker scripts, application Makefiles, Doxygen-generated docs
30+ example programs covering every peripheral, plus a CoreMark port and the official RISC-V architecture test suite
Integration wrappers for LiteX and the Vivado IP Integrator (AXI4 / AXI4-Stream)
Community ports: FreeRTOS, upstream Zephyr, MicroPython
Architecture
<p align="center"> <img src="docs/figures/astrarv32_processor.png" alt="AstraRV32 processor block diagram" width="640"> </p>

The SoC wraps the CPU with an internal bus fabric and a set of memory-mapped peripherals. Every block is generate-conditioned on its enable generic, so a disabled peripheral costs zero logic rather than being gated at runtime. External connectivity runs through XBUS, the platform's simple, technology-agnostic external bus interface. See docs/datasheet for the full memory map and a signal-level description of every module.

Repository Structure
text
.
├── rtl/                      # Hardware sources (portable VHDL — no vendor primitives)
│   ├── core/                 # CPU + SoC modules (single VHDL library: astrarv32)
│   ├── system_integration/   # LiteX core complex, Vivado IP-integrator wrapper
│   ├── test_setups/          # Minimal, board-independent bring-up designs
│   └── verilog/              # Auto-generated all-Verilog export (via GHDL)
├── sw/                       # Software framework
│   ├── bootloader/           # Default UART bootloader
│   ├── common/                # App Makefile, linker script, startup code
│   ├── lib/                    # HAL / driver sources for every peripheral
│   ├── example/                 # 30+ demo programs, one per peripheral/feature
│   ├── ocd-firmware/          # On-chip debugger "park loop" firmware
│   ├── openocd/                # OpenOCD config for JTAG access
│   └── svd/                     # CMSIS-SVD system-view description
├── sim/                      # GHDL testbench + simulation scripts
└── docs/                     # AsciiDoc data sheet, user guide, Doxygen config
Getting Started

1. Prerequisites

A RISC-V GCC toolchain targeting rv32, prefix riscv-none-elf- by default (the xPack toolchain is a convenient prebuilt option)
GHDL (VHDL-2008 support) for simulation
Optional: OpenOCD + a JTAG adapter for on-chip debug; Vivado / Quartus / GateMate / etc. for FPGA synthesis

2. Check the toolchain

bash
export PATH=$PATH:/opt/riscv/bin      # point this at your toolchain's bin/
cd sw/example/demo_blink_led
make check

Toolchain check OK confirms the compiler, image generator, and Makefile flow all work end to end.

3. Simulate

bash
cd sim
sh ghdl.sh          # elaborates rtl/core + sim, runs the astrarv32_tb testbench

4. Bring up hardware rtl/test_setups contains minimal, board-independent reference designs (bootloader, on-chip debugger, AP-ROM variants) that the user guide's tutorials build on — a good starting point before wiring the SoC into a board-specific top level.

Configuration

Every optional block is a VHDL generic on the top-level entity, astrarv32_top. ISA extensions, memory sizes and base addresses, cache geometry, peripheral instance counts and FIFO depths, and debugger/PMP/HPM options are all set at elaboration time, so unused features are optimized away rather than gated at runtime.

Category	Example generics
ISA extensions	RISCV_ISA_{C, E, M, U, Zba, Zbb, Zbc, Zbs, Zkn*, Zks*, Zfinx, Zaamo, Zalrsc, Xcfu, ...}
Memory	IMEM_SIZE / IMEM_BASE, DMEM_SIZE / DMEM_BASE, ICACHE_*, DCACHE_*
Peripherals	IO_GPIO_NUM, IO_PWM_NUM, IO_UART{0,1}_*_FIFO, IO_TRNG_*
Debug / Safety	OCD_EN, OCD_AUTHENTICATION, PMP_NUM_REGIONS, HPM_NUM_CNTS
Documentation

The full data sheet and user guide live in docs/ as AsciiDoc sources, built locally:

bash
cd docs
make html      # data sheet  -> public/index.html
make ug-html   # user guide  -> public/ug/index.html

Module-level READMEs are scattered through the tree too — start with rtl/README.md, sw/README.md, and sim/README.md.

Note: a few cross-references inside the AsciiDoc sources (GitHub Pages links, the FreeRTOS/Zephyr/MicroPython port links) still point at the upstream NEORV32 project's hosted docs and repos. Repoint them at your own GitHub Pages/fork once you publish, or leave them as pointers to the upstream resources.

About This Build

(A place to note your specific target for this platform — e.g. the FPGA/board you're deploying to, the course or project it supports, which ISA extensions and peripherals you've enabled, and anything you've built on top of the base platform.)

License & Acknowledgments

AstraRV32 is a configured deployment of NEORV32, the RISC-V processor designed and maintained by Stephan Nolting. Credit for the CPU/SoC architecture, software framework, and documentation belongs to the upstream project and its contributors.

Released under the BSD 3-Clause License — see docs/legal.adoc for the full text and copyright notice, which must be retained in redistributions per the license terms.

Content
1787820405724_HDX103-AstraRV32-Configurable-RISC-V-Embedded-SoC-Platform-main.zip

ZIP
