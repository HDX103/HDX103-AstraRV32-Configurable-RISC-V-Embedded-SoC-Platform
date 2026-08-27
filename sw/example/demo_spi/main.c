// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2025 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //


/**********************************************************************//**
 * @file demo_spi/main.c
 * @brief SPI bus explorer (execute SPI transactions by hand).
 **************************************************************************/

#include <astrarv32.h>
#include <string.h>


/**********************************************************************//**
 * @name User configuration
 **************************************************************************/
/**@{*/
/** UART BAUD rate */
#define BAUD_RATE 19200
/**@}*/


// Global variables
uint32_t spi_configured;

// Prototypes
void spi_cs(uint32_t type);
void spi_transfer(void);
void spi_setup(void);
void aux_print_hex_byte(uint8_t byte);


/**********************************************************************//**
 * This program provides an interactive console to communicate with SPI devices.
 *
 * @note This program requires the UART and the SPI to be synthesized.
 *
 * @return Irrelevant.
 **************************************************************************/
int main() {

  char buffer[8];
  int length = 0;

  // capture all exceptions and give debug info via UART
  // this is not required, but keeps us safe
  astrarv32_rte_setup();

  // setup UART at default baud rate, no interrupts
  astrarv32_uart0_setup(BAUD_RATE, 0);


  // check if UART0 unit is implemented at all
  if (astrarv32_uart0_available() == 0) {
    return 1;
  }

  // intro
  astrarv32_uart0_printf("\n<<< SPI Bus Explorer >>>\n\n");

  // check if SPI unit is implemented at all
  if (astrarv32_spi_available() == 0) {
    astrarv32_uart0_printf("ERROR! No SPI unit implemented.");
    return 1;
  }

  // info
  astrarv32_uart0_printf("This program allows to create SPI transfers by hand.\n"
                       "Type 'help' to see the help menu.\n\n");

  // disable and reset SPI module
  astrarv32_spi_disable();
  spi_configured = 0; // SPI not configured yet


  // Main menu
  for (;;) {
    astrarv32_uart0_printf("SPI_EXPLORER:> ");
    length = astrarv32_uart0_scan(buffer, 15, 1);
    astrarv32_uart0_printf("\n");

    if (!length) // nothing to be done
     continue;

    // decode input and execute command
    if (!strcmp(buffer, "help")) {
      astrarv32_uart0_printf("Available commands:\n"
                          " help  - show this text\n"
                          " setup - configure SPI module\n"
                          " en    - enable single chip-select line (set low)\n"
                          " dis   - disable all chip-select lines (set high)\n"
                          " trans - SPI data transmission (write & read to/from SPI)\n"
                          "\n"
                          "Configure the SPI module using 'setup'. Enable a certain module using 'cs-en',\n"
                          "then transfer data using 'trans' and disable the module again using 'cs-dis'.\n"
                          "\n");
    }
    else if (!strcmp(buffer, "setup")) {
      spi_setup();
    }
    else if (!strcmp(buffer, "en")) {
      spi_cs(1);
    }
    else if (!strcmp(buffer, "dis")) {
      spi_cs(0);
    }
    else if (!strcmp(buffer, "trans")) {
      spi_transfer();
    }
    else {
      astrarv32_uart0_printf("Invalid command. Type 'help' to see all commands.\n");
    }
  }

  return 0;
}


/**********************************************************************//**
 * Enable or disable chip-select line
 *
 * @param[in] type 0=disable, 1=enable
 **************************************************************************/
void spi_cs(uint32_t type) {

  char terminal_buffer[2];
  uint8_t channel;

  if (spi_configured == 0) {
    astrarv32_uart0_printf("SPI module not configured yet! Use 'setup' to configure SPI module.\n");
    return;
  }

  if (type) {
    astrarv32_uart0_printf("Chip-select line to ENABLE (set low) [0..7]: ");
    while (1) {
      astrarv32_uart0_scan(terminal_buffer, 2, 1); // 1 hex char plus '\0'
      channel = (uint8_t)astrarv32_aux_hexstr2uint64(terminal_buffer, strlen(terminal_buffer));
      if (channel > 7) {
        astrarv32_uart0_printf("\nInvalid channel selection!\n");
        return;
      }
      else {
        astrarv32_uart0_printf("\n");
        break;
      }
    }
    astrarv32_spi_cs_en(channel);
  }
  else {
    astrarv32_uart0_printf("Disabling chip select lines.\n");
    astrarv32_spi_cs_dis();
  }
}


/**********************************************************************//**
 * SPI data transfer
 **************************************************************************/
void spi_transfer(void) {

  char terminal_buffer[4];

  if (spi_configured == 0) {
    astrarv32_uart0_printf("SPI module not configured yet! Use 'setup' to configure SPI module.\n");
    return;
  }

  astrarv32_uart0_printf("Enter TX data (2 hex chars): 0x");
  astrarv32_uart0_scan(terminal_buffer, 2+1, 1);
  uint32_t tx_data = (uint32_t)astrarv32_aux_hexstr2uint64(terminal_buffer, strlen(terminal_buffer));

  uint32_t rx_data = astrarv32_spi_transfer(tx_data);

  astrarv32_uart0_printf("\nTX data: 0x");
  aux_print_hex_byte(tx_data);
  astrarv32_uart0_printf("\nRX data: 0x");
  aux_print_hex_byte(rx_data);
  astrarv32_uart0_printf("\n");
}


/**********************************************************************//**
 * Configure SPI module
 **************************************************************************/
void spi_setup(void) {

  const uint32_t PRSC_LUT[8] = {2, 4, 8, 64, 128, 1024, 2048, 4096};

  char terminal_buffer[9];
  uint8_t spi_prsc, clk_div, clk_phase, clk_pol;
  uint32_t tmp;

  // ---- SPI clock ----

  while (1) {
    astrarv32_uart0_printf("Select SPI clock prescaler (0..7): ");
    astrarv32_uart0_scan(terminal_buffer, 2, 1);
    tmp = (uint32_t)astrarv32_aux_hexstr2uint64(terminal_buffer, strlen(terminal_buffer));
    if (tmp > 8) {
      astrarv32_uart0_printf("\nInvalid selection!\n");
    }
    else {
      spi_prsc = (uint8_t)tmp;
      break;
    }
  }

  astrarv32_uart0_printf("\nEnter clock divider (0..15, as one hex char): ");
  astrarv32_uart0_scan(terminal_buffer, 2, 1);
  clk_div = (uint8_t)astrarv32_aux_hexstr2uint64(terminal_buffer, strlen(terminal_buffer));

  uint32_t clock = astrarv32_sysinfo_get_clk() / (2 * PRSC_LUT[spi_prsc] * (1 + clk_div));
  astrarv32_uart0_printf("\n+ New SPI clock speed = %u Hz\n", clock);

  // ---- SPI clock mode ----

  while (1) {
    astrarv32_uart0_printf("Select SPI clock mode (0..3): ");
    astrarv32_uart0_scan(terminal_buffer, 2, 1);
    tmp = (uint32_t)astrarv32_aux_hexstr2uint64(terminal_buffer, strlen(terminal_buffer));
    if (tmp > 4) {
      astrarv32_uart0_printf("\nInvalid selection!\n");
    }
    else {
      clk_pol   = (uint8_t)((tmp >> 1) & 1);
      clk_phase = (uint8_t)(tmp & 1);
      break;
    }
  }
  astrarv32_uart0_printf("\n+ New SPI clock mode = %u\n\n", tmp);

  astrarv32_spi_setup(spi_prsc, clk_div, clk_phase, clk_pol);
  spi_configured = 1; // SPI is configured now
}


/**********************************************************************//**
 * Print HEX byte.
 *
 * @param[in] byte Byte to be printed as 2-cahr hex value.
 **************************************************************************/
void aux_print_hex_byte(uint8_t byte) {

  static const char symbols[] = "0123456789abcdef";

  astrarv32_uart0_putc(symbols[(byte >> 4) & 0x0f]);
  astrarv32_uart0_putc(symbols[(byte >> 0) & 0x0f]);
}
