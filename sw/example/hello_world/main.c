// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file hello_world/main.c
 * @author Stephan Nolting
 * @brief Classic 'hello world' demo program.
 **************************************************************************/

#include <astrarv32.h>


/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200
/**@}*/



/**********************************************************************//**
 * Main function; prints some fancy stuff via UART.
 *
 * @note This program requires the UART interface to be synthesized.
 *
 * @return 0 if execution was successful
 **************************************************************************/
int main() {

  // capture all exceptions and give debug info via UART
  // this is not required, but keeps us safe
  astrarv32_rte_setup();

  // setup UART at default baud rate, no interrupts
  astrarv32_uart0_setup(BAUD_RATE, 0);

  // print project logo via UART
  astrarv32_aux_print_logo();

  // say hello
  astrarv32_uart0_puts("Hello world! :)\n");


  return 0;
}
