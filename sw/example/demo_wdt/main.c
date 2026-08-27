// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file demo_wdt/main.c
 * @brief Watchdog demo program.
 **************************************************************************/
#include <astrarv32.h>


/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200
/** WDT timeout (until system reset) in seconds */
#define WDT_TIMEOUT_SEC 8
/**@}*/


/**********************************************************************//**
 * Main function
 *
 * @note This program requires the WDT and UART0 to be synthesized.
 *
 * @return Should never return.
 **************************************************************************/
int main() {

  // setup ASTRARV32 runtime environment for capturing all traps
  astrarv32_rte_setup();

  // setup UART at default baud rate, no interrupts
  astrarv32_uart0_setup(BAUD_RATE, 0);

  // check if UART0 is implemented at all
  if (astrarv32_uart0_available() == 0) {
    return -1; // UART0 not synthesized
  }

  // check if WDT is implemented at all
  if (astrarv32_wdt_available() == 0) {
    astrarv32_uart0_puts("\nWDT not synthesized!\n");
    return -1;
  }


  // intro
  astrarv32_uart0_puts("\n<< Watchdog Demo Program >>\n\n");


  // show the cause of the last processor reset
  astrarv32_uart0_puts("Cause of last processor reset: ");
  if (astrarv32_wdt_get_cause() == WDT_RCAUSE_EXT) {
    astrarv32_uart0_puts("External reset\n\n");
  }
  else if (astrarv32_wdt_get_cause() == WDT_RCAUSE_OCD) {
    astrarv32_uart0_puts("On-chip debugger reset\n\n");
  }
  else if (astrarv32_wdt_get_cause() == WDT_RCAUSE_TMO) {
    astrarv32_uart0_puts("Watchdog timeout\n\n");
  }
  else if (astrarv32_wdt_get_cause() == WDT_RCAUSE_ACC) {
    astrarv32_uart0_puts("Watchdog illegal access\n\n");
  }
  else {
    astrarv32_uart0_puts("Unknown\n\n");
  }


  // compute WDT timeout value; the WDT counter increments at f_wdt = f_main / 4096
  uint32_t timeout = WDT_TIMEOUT_SEC * (astrarv32_sysinfo_get_clk() / 4096);
  if (timeout & 0xFF000000U) { // check if timeout value fits into 24-bit
    astrarv32_uart0_puts("Timeout value does not fit into 24-bit!\n");
    return -1;
  }

  // setup watchdog: no lock
  astrarv32_uart0_puts("Starting WDT...\n");
  astrarv32_wdt_setup(timeout, 0);


  // feed the watchdog
  astrarv32_uart0_puts("Resetting WDT 5 times...\n");
  int i;
  for (i=0; i<5; i++) {
    astrarv32_aux_delay_ms(astrarv32_sysinfo_get_clk(), 750);
    astrarv32_wdt_feed(WDT_PASSWORD); // reset internal counter using the access password
    astrarv32_uart0_puts("WDT reset.\n");
  }


  // go to sleep mode and wait for watchdog to time-out
  astrarv32_uart0_puts("Entering sleep mode and waiting for WDT timeout...\n");
  while(1) {
    astrarv32_cpu_sleep();
  }

  return 0; // will never be reached
}
