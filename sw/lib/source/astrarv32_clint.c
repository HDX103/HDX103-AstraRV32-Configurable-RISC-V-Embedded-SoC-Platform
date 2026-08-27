// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_clint.c
 * @brief Core-Local Interruptor (CLINT) HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if CLINT module was synthesized.
 *
 * @return 0 if CLINT was not synthesized, non-zero if CLINT is available.
 **************************************************************************/
int astrarv32_clint_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_CLINT));
}


/**********************************************************************//**
 * Trigger machine software interrupt.
 *
 * @param[in] hart Hart index (0..4095).
 **************************************************************************/
void astrarv32_clint_msi_set(int hart) {

  ASTRARV32_CLINT->MSWI[hart & 0xfff] = 1;
}


/**********************************************************************//**
 * Clear machine software interrupt.
 *
 * @param[in] hart Hart index (0..4095).
 **************************************************************************/
void astrarv32_clint_msi_clr(int hart) {

  ASTRARV32_CLINT->MSWI[hart & 0xfff] = 0;
}


/**********************************************************************//**
 * Get machine software interrupt register.
 *
 * @param[in] hart Hart index (0..4095).
 **************************************************************************/
uint32_t astrarv32_clint_msi_get(int hart) {

  return ASTRARV32_CLINT->MSWI[hart & 0xfff];
}


/**********************************************************************//**
 * Set current CLINT system time.
 *
 * @note The CLINT MTIMER increments with the primary processor clock.
 *
 * @param[in] time New system time (uint64_t).
 **************************************************************************/
void astrarv32_clint_time_set(uint64_t time) {

  subwords64_t cycles;
  cycles.uint64 = time;

  // prevent low-to-high carry while writing
  ASTRARV32_CLINT->MTIME.uint32[0] = 0;
  ASTRARV32_CLINT->MTIME.uint32[1] = cycles.uint32[1];
  ASTRARV32_CLINT->MTIME.uint32[0] = cycles.uint32[0];
}


/**********************************************************************//**
 * Get current system time.
 *
 * @note The CLINT MTIMER increments with the primary processor clock.
 *
 * @return Current system time (uint64_t).
 **************************************************************************/
uint64_t astrarv32_clint_time_get(void) {

  uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0;
  while(1) {
    tmp1 = ASTRARV32_CLINT->MTIME.uint32[1];
    tmp2 = ASTRARV32_CLINT->MTIME.uint32[0];
    tmp3 = ASTRARV32_CLINT->MTIME.uint32[1];
    if (tmp1 == tmp3) {
      break;
    }
  }

  subwords64_t cycles;
  cycles.uint32[0] = tmp2;
  cycles.uint32[1] = tmp3;

  return cycles.uint64;
}


/**********************************************************************//**
 * Set MTIMER compare time register (MTIMECMP) for generating interrupts.
 *
 * @note This functions uses MHARTID to automatically select the according hart's MTIMECMP register.
 *
 * @param[in] timecmp System time for interrupt (uint64_t).
 **************************************************************************/
void astrarv32_clint_mtimecmp_set(uint64_t timecmp) {

  subwords64_t cycles;
  cycles.uint64 = timecmp;

  uint32_t hart_id = astrarv32_cpu_csr_read(CSR_MHARTID);

  // prevent MTIMECMP from temporarily becoming smaller than the lesser of the old and new values
  ASTRARV32_CLINT->MTIMECMP[hart_id].uint32[0] = -1;
  ASTRARV32_CLINT->MTIMECMP[hart_id].uint32[1] = cycles.uint32[1];
  ASTRARV32_CLINT->MTIMECMP[hart_id].uint32[0] = cycles.uint32[0];
}


/**********************************************************************//**
 * Get MTIMER compare time register (MTIMECMP).
 *
 * @note This functions uses MHARTID to automatically select the according hart's MTIMECMP register.
 *
 * @return Current MTIMECMP value.
 **************************************************************************/
uint64_t astrarv32_clint_mtimecmp_get(void) {

  uint32_t hart_id = astrarv32_cpu_csr_read(CSR_MHARTID);
  return ASTRARV32_CLINT->MTIMECMP[hart_id].uint64;
}


/**********************************************************************//**
 * Set MTIMER to Unix time.
 *
 * @param[in] unixtime Unix time since 00:00:00 UTC, January 1st, 1970 in seconds.
 **************************************************************************/
void astrarv32_clint_unixtime_set(uint64_t unixtime) {

  astrarv32_clint_time_set(((uint64_t)astrarv32_sysinfo_get_clk()) * unixtime);
}


/**********************************************************************//**
 * Get Unix time from MTIMER.
 *
 * @return Unix time since 00:00:00 UTC, January 1st, 1970 in seconds.
 **************************************************************************/
uint64_t astrarv32_clint_unixtime_get(void) {

  return astrarv32_clint_time_get() / ((uint64_t)astrarv32_sysinfo_get_clk());
}
