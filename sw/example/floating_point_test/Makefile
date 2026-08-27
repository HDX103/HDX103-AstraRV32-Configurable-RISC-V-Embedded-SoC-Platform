# Application makefile.
# Use this makefile to configure all relevant CPU / compiler options.

# Override the default CPU ISA
MARCH = rv32i_zicsr_zifencei

# Override default optimization goal
EFFORT = -Os

# Add extended debug symbols
USER_FLAGS += -ggdb -gdwarf-3

# Adjust processor IMEM size
USER_FLAGS += -Wl,--defsym,__astrarv32_rom_size=32k

# Adjust processor DMEM size
USER_FLAGS += -Wl,--defsym,__astrarv32_ram_size=8k

# Set path to ASTRARV32 root directory
ASTRARV32_HOME ?= ../../..

# Include the main ASTRARV32 makefile
include $(ASTRARV32_HOME)/sw/common/common.mk
