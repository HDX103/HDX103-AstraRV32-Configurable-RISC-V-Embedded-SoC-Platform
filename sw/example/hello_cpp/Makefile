# Application makefile.
# Use this makefile to configure all relevant CPU / compiler options.

# Override the default CPU ISA
MARCH = rv32i_zicsr_zifencei

# Override the default RISC-V GCC prefix
#RISCV_PREFIX ?= riscv-none-elf-

# Override default optimization goal
EFFORT = -Os

# Add extended debug symbols
USER_FLAGS += -ggdb -gdwarf-3

# Adjust processor IMEM size
USER_FLAGS += -Wl,--defsym,__astrarv32_rom_size=16k

# Adjust processor DMEM size
USER_FLAGS += -Wl,--defsym,__astrarv32_ram_size=8k

# Adjust maximum heap size
#USER_FLAGS += -Wl,--defsym,__astrarv32_heap_size=1k

# Additional sources
#APP_SRC += $(wildcard ./*.c)
#APP_INC += -I .

# Set path to ASTRARV32 root directory
ASTRARV32_HOME ?= ../../..

# Include the main ASTRARV32 makefile
include $(ASTRARV32_HOME)/sw/common/common.mk

# Use c++ compiler and define c++ standard
override CC = $(RISCV_PREFIX)g++
USER_FLAGS += -std=c++11
