## Software Framework

This folder provides the hardware abstraction layer, drivers, scripts
and helpers of the ASTRARV32 software framework.

### > [`bootloader`](bootloader)

Source(s) of the default ASTRARV32 bootloader. A pre-built image is already installed
by the default `rtl/core/astrarv32_bootloader_image.vhd` file.

### > [`common`](common)

ASTRARV32-specific common files: central application Makefile,
linker script and processor start-up code.

### > [`example`](example)

Several example/demo programs for testing and for getting started.

### > [`image_gen`](image_gen)

Helper program to generate ASTRARV32 executables (e.g. for upload via bootloader) and
memory initialization files. This program is automatically compiled when
executing one of the application compilation makefile targets.

### > [`lib`](lib)

Core libraries (sources and header files) and helper functions for using
the processor peripherals and the CPU itself.

### > [`ocd-firmware`](ocd-firmware)

Firmware (debugger "park loop") for the on-chip debugger.

### > [`openocd`](openocd)

Configuration files for openOCD to connect to the ASTRARV32 on-chip debugger via JTAG.

### > [`svd`](svd)

Contains a CMSIS-SVD compatible system view description file for all
default processor peripherals.
