// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_gptmr.c
 * @brief General purpose timer (GPTMR) HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if general purpose timer unit was synthesized.
 *
 * @return 0 if GPTMR was not synthesized, non-zero if GPTMR is available.
 **************************************************************************/
int astrarv32_gptmr_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_GPTMR));
}


/**********************************************************************//**
 * Get number of implemented GPTMR timer slices.
 *
 * @return Number of implemented GPTMR slices (0..16).
 **************************************************************************/
int astrarv32_gptmr_get_num_slices(void) {

  uint16_t backup = ASTRARV32_GPTMR->CSR0.MODE;

  ASTRARV32_GPTMR->CSR0.MODE = -1;
  uint16_t tmp = ASTRARV32_GPTMR->CSR0.MODE;
  int i = 0, cnt = 0;
  for (i=0; i<16; i++) {
    cnt += tmp & 1;
    tmp >>= 1;
  }

  ASTRARV32_GPTMR->CSR0.MODE = backup;

  return cnt;
}


/**********************************************************************//**
 * Reset module and configure GPTMR global clock prescaler.
 *
 * @param[in] prsc Clock prescaler select (0..7). See #ASTRARV32_CLOCK_PRSC_enum.
 **************************************************************************/
void astrarv32_gptmr_setup(int prsc) {

  // reset control registers
  ASTRARV32_GPTMR->CSR0.WORD = 0;
  ASTRARV32_GPTMR->CSR1.WORD = 0;

  // set prescaler
  ASTRARV32_GPTMR->CSR1.PRSC = (uint16_t)prsc;

  // reset all slices
  int i;
  for (i=0; i<16; i++) {
    ASTRARV32_GPTMR->SLICE[i].CNT = 0;
    ASTRARV32_GPTMR->SLICE[i].THR = 0;
  }
}


/**********************************************************************//**
 * Enable single GPTMR timer slice.
 *
 * @param[in] sel Timer slice to enable (0..15).
 **************************************************************************/
void astrarv32_gptmr_enable_single(int sel) {

  __MMREG16_BSET(ASTRARV32_GPTMR->CSR0.ENABLE, 1 << (sel & 15));
}


/**********************************************************************//**
 * Disable single GPTMR timer slice.
 *
 * @param[in] sel Timer slice to disable (0..15).
 **************************************************************************/
void astrarv32_gptmr_disable_single(int sel) {

  __MMREG16_BCLR(ASTRARV32_GPTMR->CSR0.ENABLE, 1 << (sel & 15));
}


/**********************************************************************//**
 * Enable multiple GPTMR timer slices.
 *
 * @param[in] mask Bit mask, one bit for each slice; bit set = enable slice.
 **************************************************************************/
void astrarv32_gptmr_enable_mask(uint16_t mask) {

  __MMREG16_BSET(ASTRARV32_GPTMR->CSR0.ENABLE, mask);
}


/**********************************************************************//**
 * Disable multiple GPTMR timer slices.
 *
 * @param[in] mask Bit mask, one bit for each slice; bit set = disable slice.
 **************************************************************************/
void astrarv32_gptmr_disable_mask(uint16_t mask) {

  __MMREG16_BCLR(ASTRARV32_GPTMR->CSR0.ENABLE, mask);
}


/**********************************************************************//**
 * Configure timer slice.
 *
 * @param[in] sel Timer slice to enable (0..15).
 * @param[in] cnt Initial counter value (32-bit).
 * @param[in] thr Counter threshold value (32-bit).
 * @param[in] Mode Operation mode: 0 = single-shot mode, 1 = continuous mode.
 **************************************************************************/
void astrarv32_gptmr_configure(int sel, uint32_t cnt, uint32_t thr, int mode) {

  int i = sel & 15;
  ASTRARV32_GPTMR->SLICE[i].CNT = cnt;
  ASTRARV32_GPTMR->SLICE[i].THR = thr;

  uint16_t tmp = ASTRARV32_GPTMR->CSR0.MODE;
  tmp &= ~(uint16_t)(1 << i);
  tmp |= (uint16_t)((mode & 1) << i);
  ASTRARV32_GPTMR->CSR0.MODE = tmp;
}


/**********************************************************************//**
 * Get highest-priority pending interrupt.
 *
 * @return Id of highest-priority pending slice interrupt (0..15). -1 if
 * no interrupt is pending.
 **************************************************************************/
int astrarv32_gptmr_irq_get(void) {

  uint16_t pnd = ASTRARV32_GPTMR->CSR1.IRQ;

  if (pnd == 0) {
    return -1; // no interrupt pending
  }

  // find pending interrupt with highest priority
  int i;
  for (i=0; i<16; i++) {
    if (pnd & 1) {
      break;
    }
    pnd >>= 1;
  }
  return i;
}


/**********************************************************************//**
 * Clear pending timer interrupt.
 *
 * @param[in] sel Timer slice interrupt to acknowledge/clear (0..15); no
 * pending interrupt is cleared if an other value is provided).
 **************************************************************************/
void astrarv32_gptmr_irq_ack(int sel) {

  __MMREG16_BCLR(ASTRARV32_GPTMR->CSR1.IRQ, 1 << (sel & 0xf));
}
