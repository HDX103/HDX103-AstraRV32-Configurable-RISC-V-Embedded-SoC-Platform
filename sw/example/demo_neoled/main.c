// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file demo_neopixel/main.c
 * @author Stephan Nolting
 * @brief NeoPixel (WS2812) interface demo using the processor's smart LED interface (NEOLED).
 **************************************************************************/

#include <astrarv32.h>


/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200
/** Number of RGB LEDs in stripe (24-bit data) */
#define NUM_LEDS_24BIT (12)
/** Max intensity (0..255) */
#define MAX_INTENSITY (16)
/**@}*/


// prototypes
uint32_t hsv2rgb(int h, int v);


/**********************************************************************//**
 * Simple bus-wait helper.
 *
 * @param[in] time_ms Time in ms to wait (unsigned 32-bit).
 **************************************************************************/
void delay_ms(uint32_t time_ms) {
  astrarv32_aux_delay_ms(astrarv32_sysinfo_get_clk(), time_ms);
}


/**********************************************************************//**
 * Main function
 * This demo uses a 12-LED RGB ring
 *
 * @note This program requires the NEOLED controller to be synthesized (UART0 is optional).
 *
 * @return 0 if execution was successful
 **************************************************************************/
int main() {

  // capture all exceptions and give debug info via UART0
  // this is not required, but keeps us safe
  astrarv32_rte_setup();

  // setup UART at default baud rate, no interrupts
  astrarv32_uart0_setup(BAUD_RATE, 0);


  // check if NEOLED unit is implemented at all, abort if not
  if (astrarv32_neoled_available() == 0) {
    astrarv32_uart0_printf("Error! No NEOLED unit synthesized!\n");
    return 1;
  }


  // illustrate setup
  astrarv32_uart0_printf("\n\n<<< ASTRARV32 NeoPixel Controller (NEOLED) Demo Program >>>\n"
                       "(TM) 'NeoPixel' is a trademark of Adafruit Industries.\n\n"
                       "This demo uses the following LED setup:\n"
                       "ASTRARV32.neoled_o -> %u RGB-LEDs (WS2812, 24-bit)\n\n", (uint32_t)NUM_LEDS_24BIT);


  // use the "astrarv32_neoled_setup_ws2812()" setup function here instead the raw "astrarv32_neoled_setup()"
  // astrarv32_neoled_setup_ws2812() will configure all timing parameters according to the WS2812 specs.
  // for the current processor clock speed
  astrarv32_neoled_setup_ws2812();


  // check NEOLED configuration
  astrarv32_uart0_printf("Hardware FIFO size: %u entries\n\n", astrarv32_neoled_get_fifo_depth());


  // clear all LEDs
  astrarv32_uart0_printf("Clearing all LEDs...\n");
  int i;
  for (i=0; i<NUM_LEDS_24BIT; i++) {
    astrarv32_neoled_write24_blocking(0);
  }
  delay_ms(500);


  // a simple animation example: rotating rainbow
  // this example uses BLOCKING NEOLED functions that check the FIFO flags before writing new data
  // non-blocking functions should only be used when checking the FIFO flags (half-full) in advance
  // (for example using the NEOLED interrupt)
  astrarv32_uart0_printf("Starting animation...\n");

  int angle = 0, led_id = 0;
  while (1) {
    for (led_id=0; led_id<NUM_LEDS_24BIT; led_id++) {
      // give every LED a different color
      astrarv32_neoled_write24_blocking(hsv2rgb(angle + (360/NUM_LEDS_24BIT) * led_id, MAX_INTENSITY));
    }
    angle++; // rotation increment per frame

    astrarv32_neoled_strobe_blocking(); // send strobe ("RESET") command
    delay_ms(10); // delay between frames
  }

  return 0;
}


/**********************************************************************//**
 * Convert HSV color to RGB.
 *
 * @note Very simple version: using integer arithmetic and ignoring saturation (saturation is always MAX).
 *
 * @param[in] h Hue (color angle), 0..359
 * @param[in] v Value (intensity), 0..255
 * @return LSB-aligned 24-bit RGB data [G,R,B]
 **************************************************************************/
uint32_t hsv2rgb(int h, int v) {

  h = h % 360;
  int r, g, b;
  int i = h / 60;
  int difs = h % 60;
  int rgb_adj = (v * difs) / 60;

  switch (i) {
    case 0:
      r = v;
      g = 0 + rgb_adj;
      b = 0;
      break;
    case 1:
      r = v - rgb_adj;
      g = v;
      b = 0;
      break;
    case 2:
      r = 0;
      g = v;
      b = 0 + rgb_adj;
      break;
    case 3:
      r = 0;
      g = v - rgb_adj;
      b = v;
      break;
    case 4:
      r = 0 + rgb_adj;
      g = 0;
      b = v;
      break;
    default:
      r = v;
      g = 0;
      b = v - rgb_adj;
      break;
  }

  uint32_t res = 0;
  res |= (((uint32_t)g) & 0xff) << 16;
  res |= (((uint32_t)r) & 0xff) << 8;
  res |= (((uint32_t)b) & 0xff) << 0;

  return res;
}
