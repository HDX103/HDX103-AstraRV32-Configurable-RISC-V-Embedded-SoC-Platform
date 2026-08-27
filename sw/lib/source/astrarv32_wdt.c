// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_wdt.c
 * @brief Watchdog Timer (WDT) HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if WDT unit was synthesized.
 *
 * @return 0 if WDT was not synthesized, non-zero if WDT is available.
 **************************************************************************/
int astrarv32_wdt_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_WDT));
}


/**********************************************************************//**
 * Configure and enable watchdog timer.
 *
 * @warning Once the lock bit is set it can only be removed by a hardware reset.
 *
 * @param[in] timeout LSB-aligned 24-bit timeout value (number of clock cycles).
 * @param[in] lock Control register will be locked when 1 (until next HW reset).
 **************************************************************************/
void astrarv32_wdt_setup(uint32_t timeout, int lock) {

  ASTRARV32_WDT->CTRL = 0; // reset and disable

  // set configuration
  uint32_t ctrl = 0;
  ctrl |= ((uint32_t)(1))                    << WDT_CTRL_EN;
  ctrl |= ((uint32_t)(timeout & 0x00ffffff)) << WDT_CTRL_TIMEOUT_LSB;
  ctrl |= ((uint32_t)(lock & 1))             << WDT_CTRL_LOCK;
  ASTRARV32_WDT->CTRL = ctrl;
}


/**********************************************************************//**
 * Disable watchdog timer.
 *
 * @return Returns 0 if WDT is deactivated, non-zero otherwise.
 **************************************************************************/
int astrarv32_wdt_disable(void) {

  __MMREG32_BCLR(ASTRARV32_WDT->CTRL, 1 << WDT_CTRL_EN); // try to disable
  return (int)(ASTRARV32_WDT->CTRL & (1 << WDT_CTRL_EN));
}


/**********************************************************************//**
 * Feed watchdog (reset timeout counter).
 *
 * @param[in] password Password for WDT reset.
 **************************************************************************/
void astrarv32_wdt_feed(uint32_t password) {

  ASTRARV32_WDT->RESET = password;
}


/**********************************************************************//**
 * Force a hardware reset triggered by the watchdog.
 **************************************************************************/
void astrarv32_wdt_force_hwreset(void) {

  // make sure the WDT is enabled
  // if it is locked this will trigger a hardware reset
  ASTRARV32_WDT->CTRL = (uint32_t)(1 << WDT_CTRL_EN);

  // reset the WDT using an incorrect password;
  // this will trigger a hardware reset
  ASTRARV32_WDT->RESET = 0;
}


/**********************************************************************//**
 * Get cause of last system reset.
 *
 * @return Cause of last reset (#ASTRARV32_WDT_RCAUSE_enum).
 **************************************************************************/
int astrarv32_wdt_get_cause(void) {

  return (ASTRARV32_WDT->CTRL >> WDT_CTRL_RCAUSE_LO) & 0x3;
}
