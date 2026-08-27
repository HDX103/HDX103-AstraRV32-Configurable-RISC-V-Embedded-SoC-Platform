# Processor-check program makefile

# Maximum ISA configuration
MARCH = rv32imac_zba_zbb_zbc_zbs_zbkb_zbkc_zbkx_zcb_zknd_zkne_zknh_zksh_zksed_zicsr_zfinx_zifencei_zicond_zimop

# Optimization goal
EFFORT = -O2

# Add extended debug symbols
override USER_FLAGS += -ggdb -gdwarf-3

# Adjust processor IMEM size
override USER_FLAGS += -Wl,--defsym,__astrarv32_rom_size=32k -Wl,--defsym,__astrarv32_rom_base=0x00000000

# Adjust processor DMEM size
override USER_FLAGS += -Wl,--defsym,__astrarv32_ram_size=8k -Wl,--defsym,__astrarv32_ram_base=0x80000000

# Adjust maximum heap size
override USER_FLAGS += -Wl,--defsym,__astrarv32_heap_size=2k

# Set path to ASTRARV32 root directory
ASTRARV32_HOME ?= ../../..

# Include the main ASTRARV32 makefile
include $(ASTRARV32_HOME)/sw/common/common.mk

# Add test-specific makefile target
sim-check: sim
	@cat $(ASTRARV32_HOME)/sim/ghdl.log | grep -q "PROCESSOR CHECK COMPLETED SUCCESSFULLY!"
