// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file demo_blink_led/main.c
 * @brief Minimal blinking LED demo program using the lowest 8 bits of the GPIO.output port.
 **************************************************************************/
#include <astrarv32.h>


/**********************************************************************//**
 * Simple bus-wait helper.
 *
 * @param[in] time_ms Time in ms to wait (unsigned 32-bit).
 **************************************************************************/
void delay_ms(uint32_t time_ms) {
  astrarv32_aux_delay_ms(astrarv32_sysinfo_get_clk(), time_ms);
}


/**********************************************************************//**
 * Main function; shows an incrementing 8-bit counter on GPIO.output(7:0).
 *
 * @note This program requires the GPIO controller to be synthesized.
 *
 * @return Will never return.
 **************************************************************************/
int main() {

  // clear GPIO output (set all bits to 0)
  astrarv32_gpio_port_set(0);

  // configure lowest 8 GPIO pins as outputs
  // this is only relevant if the GPIO direction control feature is enabled
  astrarv32_gpio_dir_set(0x000000FF);

  int cnt = 0;

  while (1) {
    astrarv32_gpio_port_set(cnt++ & 0xFF); // increment counter and mask for lowest 8 bit
    delay_ms(250); // wait 250ms using busy wait
  }

  // this should never be reached
  return 0;
}
