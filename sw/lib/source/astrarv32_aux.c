// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_aux.c
 * @brief General auxiliary functions source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Simple delay function using busy-wait.
 *
 * @warning Timing is imprecise! Use TIME or CYCLE CSRs for precise timing.
 *
 * @param[in] clock_hz CPU clock speed in Hz.
 * @param[in] time_ms Time in ms to wait (unsigned 32-bit).
 **************************************************************************/
void astrarv32_aux_delay_ms(uint32_t clock_hz, uint32_t time_ms) {

  // clock ticks per ms (avoid division, therefore shift by 10 instead dividing by 1000)
  uint32_t ms_ticks = clock_hz >> 10;
  uint64_t wait_cycles = ((uint64_t)ms_ticks) * ((uint64_t)time_ms);
  // divide by clock cycles per iteration of the ASM loop (16 = shift by 4)
  uint32_t iterations = (uint32_t)(wait_cycles >> 4);

  asm volatile (
    " 1:                      \n"
    " beq  %[cnt], zero,   2f \n" // 3 cycles (if not taken)
    " bne  zero,   zero,   2f \n" // 3 cycles (never taken)
    " addi %[cnt], %[cnt], -1 \n" // 2 cycles
    " nop                     \n" // 2 cycles
    " j    1b                 \n" // 6 cycles
    " 2:                      \n"
    : [cnt] "+r" (iterations) : : "memory"
  );
}


/**********************************************************************//**
 * Convert date to Unix time stamp.
 *
 * @copyright Copyright (C) 2010-2024 Oryx Embedded SARL. All rights reserved.
 * SPDX-License-Identifier: GPL-2.0-or-later
 * https://github.com/Oryx-Embedded/Common/blob/master/date_time.c
 *
 * @param[in] date Pointer to date and time struct (#date_t).
 * @return Unix time since 00:00:00 UTC, January 1, 1970 in seconds.
 **************************************************************************/
uint64_t astrarv32_aux_date2unixtime(date_t* date) {

  uint32_t y = 0, m = 0, d = 0, t = 0;

  // range checks
  if (date->year < 1970) {
    y = 1970;
  }
  else {
    y = date->year;
  }

  if ((date->month < 1) || (date->month > 12)) {
    m = 1;
  }
  else {
    m = date->month;
  }

  if ((date->day < 1) || (date->day > 31)) {
    d = 1;
  }
  else {
    d = date->day;
  }

  // Jan and Feb are counted as months 13 and 14 of the previous year
  if (m <= 2) {
    m += 12;
    y -= 1;
  }

  // years to days
  t = (365 * y) + (y / 4) - (y / 100) + (y / 400);
  // month to days
  t += (30 * m) + (3 * (m + 1) / 5) + d;
  // Unix time base: January 1st, 1970
  t -= 719561;
  // days to seconds
  t *= 86400;
  // sum up
  t += (3600 * (date->hours % 24)) + (60 * (date->minutes % 60)) + (date->seconds % 60);

  return t;
}


/**********************************************************************//**
 * Convert Unix time stamp to date.
 *
 * @copyright Copyright (C) 2010-2024 Oryx Embedded SARL. All rights reserved.
 * SPDX-License-Identifier: GPL-2.0-or-later
 * https://github.com/Oryx-Embedded/Common/blob/master/date_time.c
 *
 * @param[in] unixtime Unix time since 00:00:00 UTC, January 1, 1970 in seconds.
 * @param[in,out] date Pointer to date and time struct (#date_t).
 **************************************************************************/
void astrarv32_aux_unixtime2date(uint64_t unixtime, date_t* date) {

  uint32_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;

  // invalid
  if (unixtime < 1) {
    unixtime = 0;
  }

  date->seconds = unixtime % 60;
  unixtime /= 60;
  date->minutes = unixtime % 60;
  unixtime /= 60;
  date->hours = unixtime % 24;
  unixtime /= 24;

  // convert
  a = (uint32_t) ((4 * unixtime + 102032) / 146097 + 15);
  b = (uint32_t) (unixtime + 2442113 + a - (a / 4));
  c = (20 * b - 2442) / 7305;
  d = b - 365 * c - (c / 4);
  e = d * 1000 / 30601;
  f = d - e * 30 - e * 601 / 1000;

  // Jan and Feb are counted as months 13 and 14 of the previous year
  if (e <= 13) {
    c -= 4716;
    e -= 1;
  }
  else {
    c -= 4715;
    e -= 13;
  }

  date->year = c;
  date->month = e;
  date->day = f;

  // day of the week
  uint32_t h, j, k;

  // Jan and Feb are counted as months 13 and 14 of the previous year
  if (e <= 2) {
    e += 12;
    c -= 1;
  }

  // century
  j = c / 100;
  // year of the century
  k = c % 100;

  // Zeller's congruence
  h = f + (26 * (e + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);

  date->weekday = ((h + 5) % 7) + 1;
}


/**********************************************************************//**
 * Helper function to convert a string of up to 16 hex chars into uint64_t.
 *
 * @param[in,out] buffer Pointer to array of chars to convert into number.
 * @param[in] length Length of the conversion string.
 * @return Converted number (uint64_t).
 **************************************************************************/
uint64_t astrarv32_aux_hexstr2uint64(char *buffer, unsigned int length) {

  uint64_t res = 0;
  uint32_t d = 0;
  char c = 0;

  // constrain max range
  if (length > 16) {
    length = 16;
  }

  while (length--) {
    c = *buffer++;

    if (c == '\0') { // end of input string
      break;
    }

    if ((c >= '0') && (c <= '9')) {
      d = (uint32_t)(c - '0');
    }
    else if ((c >= 'a') && (c <= 'f')) {
      d = (uint32_t)((c - 'a') + 10);
    }
    else if ((c >= 'A') && (c <= 'F')) {
      d = (uint32_t)((c - 'A') + 10);
    }
    else {
      d = 0;
    }

    res <<= 4;
    res += (uint64_t)d;
  }

  return res;
}


/**********************************************************************//**
 * XORSHIFT pseudo random number generator.
 *
 * @return Random number (uint32_t).
 **************************************************************************/
uint32_t astrarv32_aux_xorshift32(void) {

  static uint32_t x32 = 314159265;

  x32 ^= x32 << 13;
  x32 ^= x32 >> 17;
  x32 ^= x32 << 5;

  return x32;
}


/**********************************************************************//**
 * Simplified version of "itoa": convert number to string.
 *
 * @param[in,out] buffer Pointer to array for the result string [33 chars].
 * @param[in] num Number to convert.
 * @param[in] base Base of number representation (2..16).
 **************************************************************************/
void astrarv32_aux_itoa(char *buffer, uint32_t num, uint32_t base) {

  const char digits[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  char tmp[36];
  char *tmp_ptr = 0;
  unsigned int i = 0;

  // prevent uninitialized stack bytes
  for (i=0; i<sizeof(tmp); i++) {
    tmp[i] = 0;
  }

  if ((base < 2) || (base > 16)) { // invalid base?
    *buffer = '\0';
    return;
  }

  // initialize with all-zero
  for (i=0; i<sizeof(tmp); i++) {
    tmp[i] = '\0';
  }

  tmp_ptr = &tmp[sizeof(tmp)-1]; // go to end of array
  do { // generate digit by digit
    tmp_ptr--;
    *tmp_ptr = digits[num%base];
    num /= base;
  } while (num != 0);

  // delete leading zeros
  for (i=0; i<sizeof(tmp); i++) {
    if (tmp[i] != '\0') {
      *buffer = tmp[i];
      buffer++;
    }
  }

  // terminate result string
  *buffer = '\0';
}


/**********************************************************************//**
 * Print hardware configuration information via UART0.
 *
 * @warning This function overrides several CSRs!
 **************************************************************************/
void astrarv32_aux_print_hw_config(void) {

  if (astrarv32_uart0_available() == 0) {
    return; // cannot output anything if UART0 is not implemented
  }

  uint32_t tmp = 0;
  int i = 0;

  astrarv32_uart0_printf("\n\n<< ASTRARV32 Processor Configuration >>\n\n");

  // general
  astrarv32_uart0_printf("Is simulation:       ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_SIM)) {
    astrarv32_uart0_printf("yes\n");
  }
  else {
    astrarv32_uart0_printf("no\n");
  }

  astrarv32_uart0_printf("CPU cores (harts):   %u\n", astrarv32_sysinfo_get_numcores());

  astrarv32_uart0_printf("Clock speed:         %u Hz\n", astrarv32_sysinfo_get_clk());

  astrarv32_uart0_printf("On-chip debugger:    ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_OCD)) {
    astrarv32_uart0_printf("enabled");
    if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_OCD_AUTH)) {
      astrarv32_uart0_printf(" + authentication");
    }
    astrarv32_uart0_printf(", %u HW trigger(s)\n", astrarv32_cpu_hwtrig_get_number());
  }
  else {
    astrarv32_uart0_printf("disabled\n");
  }

  // IDs
  astrarv32_uart0_printf(
    "Hart ID:             0x%x\n"
    "Vendor ID:           0x%x\n"
    "Architecture ID:     0x%x\n"
    "Implementation ID:   0x%x (v",
    astrarv32_cpu_csr_read(CSR_MHARTID),
    astrarv32_cpu_csr_read(CSR_MVENDORID),
    astrarv32_cpu_csr_read(CSR_MARCHID),
    astrarv32_cpu_csr_read(CSR_MIMPID)
  );
  astrarv32_aux_print_hw_version(astrarv32_cpu_csr_read(CSR_MIMPID));
  astrarv32_uart0_printf(")\n");

  // CPU architecture and Endianness
  astrarv32_uart0_printf("Architecture:        ");
  tmp = astrarv32_cpu_csr_read(CSR_MISA);
  tmp = (tmp >> 30) & 0x03;
  if (tmp == 1) {
    astrarv32_uart0_printf("rv32-little");
  }
  else {
    astrarv32_uart0_printf("unknown");
  }

  // CPU extensions
  astrarv32_uart0_printf("\nISA extensions:      ");
  tmp = astrarv32_cpu_csr_read(CSR_MISA);
  for (i=0; i<26; i++) {
    if (tmp & (1 << i)) {
      astrarv32_uart0_putc((char)('A' + i));
      astrarv32_uart0_putc(' ');
    }
  }

  // CPU sub-extensions
  uint32_t mxisa  = astrarv32_cpu_csr_read(CSR_MXISA);
  uint32_t mxisah = astrarv32_cpu_csr_read(CSR_MXISAH);
  if (mxisa  & (1<<CSR_MXISA_ZAAMO))     { astrarv32_uart0_printf("Zaamo ");     }
  if (mxisa  & (1<<CSR_MXISA_ZALRSC))    { astrarv32_uart0_printf("Zalrsc ");    }
  if (mxisa  & (1<<CSR_MXISA_ZBA))       { astrarv32_uart0_printf("Zba ");       }
  if (mxisa  & (1<<CSR_MXISA_ZBB))       { astrarv32_uart0_printf("Zbb ");       }
  if (mxisah & (1<<CSR_MXISAH_ZBC))      { astrarv32_uart0_printf("Zbc ");       }
  if (mxisa  & (1<<CSR_MXISA_ZBKB))      { astrarv32_uart0_printf("Zbkb ");      }
  if (mxisa  & (1<<CSR_MXISA_ZBKC))      { astrarv32_uart0_printf("Zbkc ");      }
  if (mxisa  & (1<<CSR_MXISA_ZBKX))      { astrarv32_uart0_printf("Zbkx ");      }
  if (mxisa  & (1<<CSR_MXISA_ZBS))       { astrarv32_uart0_printf("Zbs ");       }
  if (mxisa  & (1<<CSR_MXISA_ZCA))       { astrarv32_uart0_printf("Zca ");       }
  if (mxisa  & (1<<CSR_MXISA_ZCB))       { astrarv32_uart0_printf("Zcb ");       }
  if (mxisah & (1<<CSR_MXISAH_ZCMOP))    { astrarv32_uart0_printf("Zcmop ");     }
  if (mxisa  & (1<<CSR_MXISA_ZFINX))     { astrarv32_uart0_printf("Zfinx ");     }
  if (mxisa  & (1<<CSR_MXISA_ZIBI))      { astrarv32_uart0_printf("Zibi ");      }
  if (mxisa  & (1<<CSR_MXISA_ZICNTR))    { astrarv32_uart0_printf("Zicntr ");    }
  if (mxisa  & (1<<CSR_MXISA_ZICOND))    { astrarv32_uart0_printf("Zicond ");    }
  if (mxisa  & (1<<CSR_MXISA_ZICSR))     { astrarv32_uart0_printf("Zicsr ");     }
  if (mxisa  & (1<<CSR_MXISA_ZIFENCEI))  { astrarv32_uart0_printf("Zifencei ");  }
  if (mxisa  & (1<<CSR_MXISA_ZIHPM))     { astrarv32_uart0_printf("Zihpm ");     }
  if (mxisa  & (1<<CSR_MXISA_ZIMOP))     { astrarv32_uart0_printf("Zimop ");     }
  if (mxisa  & (1<<CSR_MXISA_ZKN))       { astrarv32_uart0_printf("Zkn ");       }
  if (mxisa  & (1<<CSR_MXISA_ZKND))      { astrarv32_uart0_printf("Zknd ");      }
  if (mxisa  & (1<<CSR_MXISA_ZKNE))      { astrarv32_uart0_printf("Zkne ");      }
  if (mxisa  & (1<<CSR_MXISA_ZKNH))      { astrarv32_uart0_printf("Zknh ");      }
  if (mxisa  & (1<<CSR_MXISA_ZKS))       { astrarv32_uart0_printf("Zks ");       }
  if (mxisa  & (1<<CSR_MXISA_ZKSED))     { astrarv32_uart0_printf("Zksed ");     }
  if (mxisa  & (1<<CSR_MXISA_ZKSH))      { astrarv32_uart0_printf("Zksh ");      }
  if (mxisa  & (1<<CSR_MXISA_ZKT))       { astrarv32_uart0_printf("Zkt ");       }
  if (mxisa  & (1<<CSR_MXISA_ZMMUL))     { astrarv32_uart0_printf("Zmmul ");     }
  if (mxisa  & (1<<CSR_MXISA_SDEXT))     { astrarv32_uart0_printf("Sdext ");     }
  if (mxisa  & (1<<CSR_MXISA_SDTRIG))    { astrarv32_uart0_printf("Sdtrig ");    }
  if (mxisa  & (1<<CSR_MXISA_SMCNTRPMF)) { astrarv32_uart0_printf("Smcntrpmf "); }
  if (mxisa  & (1<<CSR_MXISA_SMPMP))     { astrarv32_uart0_printf("Smpmp ");     }
  if (mxisa  & (1<<CSR_MXISA_XCFU))      { astrarv32_uart0_printf("Xcfu ");      }

  // check physical memory protection
  astrarv32_uart0_printf("\nPhys. Memory Prot.:  ");
  uint32_t pmp_num_regions = astrarv32_cpu_pmp_get_num_regions();
  if (pmp_num_regions != 0)  {
    astrarv32_uart0_printf("%u region(s), %u bytes granularity, modes =", pmp_num_regions, astrarv32_cpu_pmp_get_granularity());
    // check implemented modes
    astrarv32_cpu_csr_write(CSR_PMPCFG0, (PMP_OFF << PMPCFG_A_LSB)); // try to set mode "OFF"
    if ((astrarv32_cpu_csr_read(CSR_PMPCFG0) & 0xff) == (PMP_OFF << PMPCFG_A_LSB)) {
      astrarv32_uart0_printf(" OFF");
    }
    astrarv32_cpu_csr_write(CSR_PMPCFG0, (PMP_TOR << PMPCFG_A_LSB)); // try to set mode "TOR"
    if ((astrarv32_cpu_csr_read(CSR_PMPCFG0) & 0xff) == (PMP_TOR << PMPCFG_A_LSB)) {
      astrarv32_uart0_printf(" TOR");
    }
    astrarv32_cpu_csr_write(CSR_PMPCFG0, (PMP_NA4 << PMPCFG_A_LSB)); // try to set mode "NA4"
    if ((astrarv32_cpu_csr_read(CSR_PMPCFG0) & 0xff) == (PMP_NA4 << PMPCFG_A_LSB)) {
      astrarv32_uart0_printf(" NA4");
    }
    astrarv32_cpu_csr_write(CSR_PMPCFG0, (PMP_NAPOT << PMPCFG_A_LSB)); // try to set mode "NAPOT"
    if ((astrarv32_cpu_csr_read(CSR_PMPCFG0) & 0xff) == (PMP_NAPOT << PMPCFG_A_LSB)) {
      astrarv32_uart0_printf(" NAPOT");
    }
    astrarv32_cpu_csr_write(CSR_PMPCFG0, 0); // disable PMP entry again
  }
  else {
    astrarv32_uart0_printf("none");
  }

  // check hardware performance monitors
  astrarv32_uart0_printf("\nHPM counters:        ");
  uint32_t hpm_num = astrarv32_cpu_hpm_get_num_counters();
  if (hpm_num != 0) {
    astrarv32_uart0_printf("%u counter(s), %u bit(s) wide", hpm_num, astrarv32_cpu_hpm_get_size());
  }
  else {
    astrarv32_uart0_printf("none");
  }

  astrarv32_uart0_printf("\nBoot configuration:  ");
  int boot_config = astrarv32_sysinfo_get_bootmode();
  switch (boot_config) {
    case 0:  astrarv32_uart0_printf("boot via bootloader (0)\n"); break;
    case 1:  astrarv32_uart0_printf("boot from custom address (1)\n"); break;
    case 2:  astrarv32_uart0_printf("boot from pre-initialized IMEM (2)\n"); break;
    default: astrarv32_uart0_printf("unknown (%u)\n", boot_config); break;
  }

  // internal IMEM
  astrarv32_uart0_printf("Internal IMEM:       ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IMEM)) {
    astrarv32_uart0_printf("%u bytes\n", astrarv32_sysinfo_get_imemsize());
  }
  else {
    astrarv32_uart0_printf("none\n");
  }

  // internal DMEM
  astrarv32_uart0_printf("Internal DMEM:       ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_DMEM)) {
    astrarv32_uart0_printf("%u bytes\n", astrarv32_sysinfo_get_dmemsize());
  }
  else {
    astrarv32_uart0_printf("none\n");
  }


  // CPU caches
  uint32_t c_block_size = (ASTRARV32_SYSINFO->CACHE >> SYSINFO_CACHE_BLOCK_SIZE_0) & 0x0F;
  c_block_size = 1 << c_block_size;

  uint32_t ic_num_blocks = (ASTRARV32_SYSINFO->CACHE >> SYSINFO_CACHE_I_NUM_BLOCKS_0) & 0x0F;
  ic_num_blocks = 1 << ic_num_blocks;

  uint32_t dc_num_blocks = (ASTRARV32_SYSINFO->CACHE >> SYSINFO_CACHE_D_NUM_BLOCKS_0) & 0x0F;
  dc_num_blocks = 1 << dc_num_blocks;

  uint32_t uncached_beg = ((ASTRARV32_SYSINFO->CACHE >> SYSINFO_CACHE_UC_BEGIN_0) & 0x0F) << 28;
  uint32_t uncached_end = 0xFFFFFFFF;

  astrarv32_uart0_printf("CPU I-cache:         ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_ICACHE)) {
    astrarv32_uart0_printf("%u bytes (%ux%u)", ic_num_blocks*c_block_size, ic_num_blocks, c_block_size);
    if (ASTRARV32_SYSINFO->CACHE & (1 << SYSINFO_CACHE_BURSTS_EN)) {
      astrarv32_uart0_printf(", bursts enabled");
    }
    else {
      astrarv32_uart0_printf(", no bursts");
    }
    astrarv32_uart0_printf(", uncached: 0x%x..0x%x\n", uncached_beg, uncached_end);
  }
  else {
    astrarv32_uart0_printf("none\n");
  }

  astrarv32_uart0_printf("CPU D-cache:         ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_DCACHE)) {
    astrarv32_uart0_printf("%u bytes (%ux%u)", dc_num_blocks*c_block_size, dc_num_blocks, c_block_size);
    if (ASTRARV32_SYSINFO->CACHE & (1 << SYSINFO_CACHE_BURSTS_EN)) {
      astrarv32_uart0_printf(", bursts enabled");
    }
    else {
      astrarv32_uart0_printf(", no bursts");
    }
    astrarv32_uart0_printf(", uncached: 0x%x..0x%x\n", uncached_beg, uncached_end);
  }
  else {
    astrarv32_uart0_printf("none\n");
  }

  // serial memory controller
  astrarv32_uart0_printf("Serial MEM ctrl.:    ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_SMC)) {
    astrarv32_uart0_printf("enabled, memory base: 0x%x\n", astrarv32_smc_get_baseaddr());
  }
  else {
    astrarv32_uart0_printf("none\n");
  }

  // external bus interface
  astrarv32_uart0_printf("Ext. bus interface:  ");
  if (ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_XBUS)) {
    astrarv32_uart0_printf("enabled");
  }
  else {
    astrarv32_uart0_printf("none");
  }
  if (ASTRARV32_SYSINFO->CACHE & (1 << SYSINFO_CACHE_BURSTS_EN)) {
    astrarv32_uart0_printf(", bursts enabled\n");
  }
  else {
    astrarv32_uart0_printf("\n");
  }

  // bus timeouts
  astrarv32_uart0_printf("Bus timeout (int):   %u cycles\n", astrarv32_sysinfo_get_intbustimeout());
  astrarv32_uart0_printf("Bus timeout (ext):   %u cycles\n", astrarv32_sysinfo_get_extbustimeout());

  // peripherals
  astrarv32_uart0_printf("Peripherals:         ");
  tmp = ASTRARV32_SYSINFO->SOC;
  if (tmp & (1 << SYSINFO_SOC_IO_CFS))     { astrarv32_uart0_printf("CFS ");        }
  if (tmp & (1 << SYSINFO_SOC_IO_CLINT))   { astrarv32_uart0_printf("CLINT ");      }
  if (tmp & (1 << SYSINFO_SOC_IO_DMA))     { astrarv32_uart0_printf("DMA ");        }
  if (tmp & (1 << SYSINFO_SOC_IO_GPIO))    { astrarv32_uart0_printf("GPIO ");       }
  if (tmp & (1 << SYSINFO_SOC_IO_GPTMR))   { astrarv32_uart0_printf("GPTMR ");      }
  if (tmp & (1 << SYSINFO_SOC_IO_NEOLED))  { astrarv32_uart0_printf("NEOLED ");     }
  if (tmp & (1 << SYSINFO_SOC_IO_ONEWIRE)) { astrarv32_uart0_printf("ONEWIRE ");    }
  if (tmp & (1 << SYSINFO_SOC_IO_PWM))     { astrarv32_uart0_printf("PWM ");        }
  if (tmp & (1 << SYSINFO_SOC_IO_SDI))     { astrarv32_uart0_printf("SDI ");        }
  if (tmp & (1 << SYSINFO_SOC_IO_SLINK))   { astrarv32_uart0_printf("SLINK ");      }
  if (tmp & (1 << SYSINFO_SOC_IO_SPI))     { astrarv32_uart0_printf("SPI ");        }
                                             astrarv32_uart0_printf("SYSINFO "); // always enabled
  if (tmp & (1 << SYSINFO_SOC_IO_TRACER))  { astrarv32_uart0_printf("TRACER ");     }
  if (tmp & (1 << SYSINFO_SOC_IO_TRNG))    { astrarv32_uart0_printf("TRNG ");       }
  if (tmp & (1 << SYSINFO_SOC_IO_TWD))     { astrarv32_uart0_printf("TWD ");        }
  if (tmp & (1 << SYSINFO_SOC_IO_TWI))     { astrarv32_uart0_printf("TWI ");        }
  if (tmp & (1 << SYSINFO_SOC_IO_UART0))   { astrarv32_uart0_printf("UART0 ");      }
  if (tmp & (1 << SYSINFO_SOC_IO_UART1))   { astrarv32_uart0_printf("UART1 ");      }
  if (tmp & (1 << SYSINFO_SOC_IO_WDT))     { astrarv32_uart0_printf("WDT ");        }

  astrarv32_uart0_printf("\n\n");
}


/**********************************************************************//**
 * Print processor version in human-readable format via UART0.
 *
 * @param[in] impid BCD-coded implementation ID (aka the version),
 * typically from the mimpid CSR.
 **************************************************************************/
void astrarv32_aux_print_hw_version(uint32_t impid) {

  uint32_t i = 0;
  char tmp = 0, cnt = 0;

  if (astrarv32_uart0_available() != 0) { // cannot output anything if UART0 is not implemented
    for (i=0; i<4; i++) {

      tmp = (char)(impid >> (24 - 8*i));

      // serial division
      cnt = 0;
      while (tmp >= 16) {
        tmp = tmp - 16;
        cnt++;
      }

      if (cnt) {
        astrarv32_uart0_putc('0' + cnt);
      }
      astrarv32_uart0_putc('0' + tmp);
      if (i < 3) {
        astrarv32_uart0_putc('.');
      }
    }
  }
}


/**********************************************************************//**
 * Print project info via UART0.
 **************************************************************************/
void astrarv32_aux_print_about(void) {

  if (astrarv32_uart0_available() != 0) { // cannot output anything if UART0 is not implemented
    astrarv32_uart0_puts("The ASTRARV32 RISC-V Processor, github.com/stnolting/astrarv32\n"
                       "Copyright (c) ASTRARV32 contributors.\n"
                       "Copyright (c) 2020 - 2026, Stephan Nolting. All rights reserved.\n"
                       "SPDX-License-Identifier: BSD-3-Clause\n");
  }
}


/**********************************************************************//**
 * Print project logo via UART0.
 **************************************************************************/
void astrarv32_aux_print_logo(void) {

  const uint16_t logo_c[9][7] = {
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0300, 0xc630},
    {0x60c7, 0xfc7f, 0x87f8, 0xc0c7, 0xf87f, 0x8303, 0xfffc},
    {0xf0cc, 0x00c0, 0xcc0c, 0xc0cc, 0x0cc0, 0xc30f, 0x000f},
    {0xd8cc, 0x00c0, 0xcc0c, 0xc0c0, 0x0c01, 0x8303, 0x1f8c},
    {0xcccf, 0xf8c0, 0xcff8, 0xc0c0, 0xf806, 0x030f, 0x1f8f},
    {0xc6cc, 0x00c0, 0xcc18, 0x6180, 0x0c18, 0x0303, 0x1f8c},
    {0xc3cc, 0x00c0, 0xcc0c, 0x330c, 0x0c60, 0x030f, 0x000f},
    {0xc187, 0xfc7f, 0x8c06, 0x0c07, 0xf8ff, 0xc303, 0xfffc},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0300, 0xc630}
  };

  unsigned int x = 0, y = 0, z = 0;
  uint16_t tmp = 0;
  char c = 0;

  if (astrarv32_uart0_available() != 0) { // cannot output anything if UART0 is not implemented
    for (y=0; y<(sizeof(logo_c) / sizeof(logo_c[0])); y++) {
      astrarv32_uart0_puts("\n");
      for (x=0; x<(sizeof(logo_c[0]) / sizeof(logo_c[0][0])); x++) {
        tmp = logo_c[y][x];
        for (z=0; z<(sizeof(logo_c[0][0])*8); z++){
          c = ' ';
          if (((int16_t)tmp) < 0) { // check MSB
            c = '#';
          }
          astrarv32_uart0_putc(c);
          tmp <<= 1;
        }
      }
    }
    astrarv32_uart0_puts("\n");
  }
}


/**********************************************************************//**
 * Print project license via UART0.
 **************************************************************************/
void astrarv32_aux_print_license(void) {

  if (astrarv32_uart0_available() != 0) { // cannot output anything if UART0 is not implemented
    astrarv32_uart0_puts(
      "\n"
      "BSD 3-Clause License\n"
      "\n"
      "Copyright (c) ASTRARV32 contributors.\n"
      "Copyright (c) 2020 - 2026, Stephan Nolting. All rights reserved.\n"
      "\n"
      "Redistribution and use in source and binary forms, with or without modification, are\n"
      "permitted provided that the following conditions are met:\n"
      "\n"
      "1. Redistributions of source code must retain the above copyright notice, this list of\n"
      "   conditions and the following disclaimer.\n"
      "\n"
      "2. Redistributions in binary form must reproduce the above copyright notice, this list of\n"
      "   conditions and the following disclaimer in the documentation and/or other materials\n"
      "   provided with the distribution.\n"
      "\n"
      "3. Neither the name of the copyright holder nor the names of its contributors may be used to\n"
      "   endorse or promote products derived from this software without specific prior written\n"
      "   permission.\n"
      "\n"
      "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS\n"
      "OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
      "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE\n"
      "COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n"
      "EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE\n"
      "GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED\n"
      "AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n"
      "NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED\n"
      "OF THE POSSIBILITY OF SUCH DAMAGE.\n"
      "\n"
      "SPDX-License-Identifier: BSD-3-Clause\n"
      "\n"
    );
  }
}
