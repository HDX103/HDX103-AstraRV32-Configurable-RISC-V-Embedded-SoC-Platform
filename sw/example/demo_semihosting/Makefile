# Application makefile.
# Use this makefile to configure all relevant CPU / compiler options.

# Override the default CPU ISA
MARCH = rv32i_zicsr_zifencei

# Override default optimization goal
EFFORT = -Os

# Add extended debug symbols
USER_FLAGS += -ggdb -gdwarf-3

# Forward stdio calls to the host computer
USER_FLAGS += -DSTDIO_SEMIHOSTING

# Adjust processor IMEM size and base address
USER_FLAGS += -Wl,--defsym,__astrarv32_rom_size=32k
USER_FLAGS += -Wl,--defsym,__astrarv32_rom_base=0x00000000

# Adjust processor DMEM size and base address
USER_FLAGS += -Wl,--defsym,__astrarv32_ram_size=8k
USER_FLAGS += -Wl,--defsym,__astrarv32_ram_base=0x80000000

# Set path to ASTRARV32 root directory
ASTRARV32_HOME ?= ../../..

# Include the main ASTRARV32 makefile
include $(ASTRARV32_HOME)/sw/common/common.mk
