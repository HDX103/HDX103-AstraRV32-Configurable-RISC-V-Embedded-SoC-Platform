// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_trng.c
 * @brief True Random Number Generator (TRNG) HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if TRNG unit was synthesized.
 *
 * @return 0 if TRNG was not synthesized, non-zero if TRNG is available.
 **************************************************************************/
int astrarv32_trng_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_TRNG));
}


/**********************************************************************//**
 * Reset and enable TRNG.
 **************************************************************************/
void astrarv32_trng_enable(void) {

  ASTRARV32_TRNG->CTRL = 0; // disable and reset

  // wait for all internal components to reset
  int i = 0;
  for (i=0; i<64; i++) {
    asm volatile ("nop");
  }

  ASTRARV32_TRNG->CTRL = 1 << TRNG_CTRL_EN; // enable
}


/**********************************************************************//**
 * Reset and disable TRNG.
 **************************************************************************/
void astrarv32_trng_disable(void) {

  ASTRARV32_TRNG->CTRL = 0;
}


/**********************************************************************//**
 * Flush TRNG random data FIFO.
 **************************************************************************/
void astrarv32_trng_fifo_clear(void) {

  __MMREG32_BSET(ASTRARV32_TRNG->CTRL, 1 << TRNG_CTRL_FIFO_CLR);
}


/**********************************************************************//**
 * Get TRNG configuration: FIFO depth.
 *
 * @return TRNG FIFO size (number of entries).
 **************************************************************************/
int astrarv32_trng_get_fifo_depth(void) {

  uint32_t tmp = (ASTRARV32_TRNG->CTRL >> TRNG_CTRL_FIFO_LSB) & 0x0f;
  return (int)(1 << tmp);
}


/**********************************************************************//**
 * Get TRNG configuration: Number of raw bits processed for one output byte.
 *
 * @return Number of raw bits per output sample.
 **************************************************************************/
int astrarv32_trng_get_num_raw_bits(void) {

  uint32_t tmp = (ASTRARV32_TRNG->CTRL >> TRNG_CTRL_NBIT_LSB) & 0x0f;
  return (int)(1 << tmp);
}


/**********************************************************************//**
 * Get TRNG configuration: Total number of ring-oscillators.
 *
 * @return Number of ring-oscillators.
 **************************************************************************/
int astrarv32_trng_get_num_ros(void) {

  return (int)((ASTRARV32_TRNG->CTRL >> TRNG_CTRL_NRO_LSB) & 0xff);
}


/**********************************************************************//**
 * Get TRNG configuration: Number if inverters in first ring-oscillator.
 *
 * @return Number if inverters in first ring-oscillator.
 **************************************************************************/
int astrarv32_trng_get_num_inv(void) {

  return (int)((ASTRARV32_TRNG->CTRL >> TRNG_CTRL_NINV_LSB) & 0xfff);
}


/**********************************************************************//**
 * Check if at least one byte of random is available.
 *
 * @return 0 if no data available, non-zero if at least one byte is available.
 **************************************************************************/
int astrarv32_trng_data_avail(void) {

  return (int)(ASTRARV32_TRNG->CTRL & (1<<TRNG_CTRL_AVAIL));
}


/**********************************************************************//**
 * Get random data byte from TRNG (non-blocking).
 * Check before if data is available using astrarv32_trng_data_avail().
 *
 * @return Random data byte.
 **************************************************************************/
uint8_t astrarv32_trng_data_get(void) {

  return (uint8_t)ASTRARV32_TRNG->DATA;
}


/**********************************************************************//**
 * Check if TRNG is implemented using SIMULATION mode.
 *
 * @warning In simulation mode the physical entropy source is replaced by a PRNG (LFSR) with very bad random quality.
 *
 * @return Simulation mode active when not zero.
 **************************************************************************/
int astrarv32_trng_check_sim_mode(void) {

  return (int)(ASTRARV32_TRNG->CTRL & (1<<TRNG_CTRL_SIM_MODE));
}
