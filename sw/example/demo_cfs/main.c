// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**********************************************************************//**
 * @file demo_cfs/main.c
 * @brief Simple demo program for the default custom functions subsystem (CFS).
 **************************************************************************/

#include <astrarv32.h>

/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200
/** Number of test cases per CFS function */
#define TESTCASES 4
/**@}*/

/**********************************************************************//**
 * Main function
 * @note This program requires the CFS and UART0.
 * @return 0 if execution was successful
 **************************************************************************/
int main() {

  uint32_t i, tmp;

  // setup ASTRARV32 runtime environment
  astrarv32_rte_setup();

  // setup UART at default baud rate, no interrupts
  astrarv32_uart0_setup(BAUD_RATE, 0);

  // check if CFS is implemented at all
  if (astrarv32_cfs_available() == 0) {
    astrarv32_uart0_printf("Error! CFS not available!\n");
    return 1;
  }

  // intro
  astrarv32_uart0_printf("<<< Custom Functions Subsystem (CFS) Demo Program >>>\n\n");

  astrarv32_uart0_printf("NOTE: This program assumes the _default_ CFS hardware module, which implements\n"
                       "      simple data conversion functions using four memory-mapped registers.\n\n");

  astrarv32_uart0_printf("Default CFS memory-mapped registers:\n"
                       " * ASTRARV32_CFS->REG[0] (r/w): convert binary to gray code\n"
                       " * ASTRARV32_CFS->REG[1] (r/w): convert gray to binary code\n"
                       " * ASTRARV32_CFS->REG[2] (r/w): bit reversal\n"
                       "The remaining 60 CFS registers are unused and will return 0 when read.\n");

  // function examples
  astrarv32_uart0_printf("\n--- CFS 'OR-all-bits' function ---\n");
  for (i=0; i<TESTCASES; i++) {
    tmp = astrarv32_aux_xorshift32(); // get random test data
    ASTRARV32_CFS->REG[0] = tmp; // write to CFS memory-mapped register 0
    astrarv32_uart0_printf("%u: IN = 0x%x, OUT = 0x%x\n", i, tmp, ASTRARV32_CFS->REG[0]); // read from CFS memory-mapped register 0
  }

  astrarv32_uart0_printf("\n--- CFS 'XOR-all-bits' function ---\n");
  for (i=0; i<TESTCASES; i++) {
    tmp = astrarv32_aux_xorshift32(); // get random test data
    ASTRARV32_CFS->REG[1] = tmp; // write to CFS memory-mapped register 1
    astrarv32_uart0_printf("%u: IN = 0x%x, OUT = 0x%x\n", i, tmp, ASTRARV32_CFS->REG[1]); // read from CFS memory-mapped register 1
  }

  astrarv32_uart0_printf("\n--- CFS 'bit reversal' function ---\n");
  for (i=0; i<TESTCASES; i++) {
    tmp = astrarv32_aux_xorshift32(); // get random test data
    ASTRARV32_CFS->REG[2] = tmp; // write to CFS memory-mapped register 2
    astrarv32_uart0_printf("%u: IN = 0x%x, OUT = 0x%x\n", i, tmp, ASTRARV32_CFS->REG[2]); // read from CFS memory-mapped register 2
  }

  astrarv32_uart0_printf("\nCFS demo program completed.\n");
  return 0;
}
