// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_spi.c
 * @brief Serial peripheral interface controller (SPI) HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if SPI unit was synthesized.
 *
 * @return 0 if SPI was not synthesized, non-zero if SPI is available.
 **************************************************************************/
int astrarv32_spi_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_SPI));
}


/**********************************************************************//**
 * Enable and configure SPI controller. The SPI control register bits are listed in #ASTRARV32_SPI_CTRL_enum.
 *
 * @param[in] prsc Clock prescaler select (0..7).  See #ASTRARV32_CLOCK_PRSC_enum.
 * @param[in] cdiv Clock divider (0..15).
 * @param[in] clk_phase Clock phase (0=sample on rising edge, 1=sample on falling edge).
 * @param[in] clk_polarity Clock polarity (when idle).
 **************************************************************************/
void astrarv32_spi_setup(int prsc, int cdiv, int clk_phase, int clk_polarity) {

  ASTRARV32_SPI->CTRL = 0; // reset

  uint32_t tmp = 0;
  tmp |= (uint32_t)(1            & 0x01) << SPI_CTRL_EN;
  tmp |= (uint32_t)(clk_phase    & 0x01) << SPI_CTRL_CPHA;
  tmp |= (uint32_t)(clk_polarity & 0x01) << SPI_CTRL_CPOL;
  tmp |= (uint32_t)(prsc         & 0x07) << SPI_CTRL_PRSC0;
  tmp |= (uint32_t)(cdiv         & 0x0f) << SPI_CTRL_CDIV0;

  ASTRARV32_SPI->CTRL = tmp;
}


/**********************************************************************//**
 * Get configured clock speed in Hz.
 *
 * @return Actual configured SPI clock speed in Hz.
 **************************************************************************/
uint32_t astrarv32_spi_get_clock_speed(void) {

  const uint16_t PRSC_LUT[8] = {2, 4, 8, 64, 128, 1024, 2048, 4096};

  uint32_t ctrl = ASTRARV32_SPI->CTRL;
  uint32_t prsc_sel  = (ctrl >> SPI_CTRL_PRSC0) & 0x7;
  uint32_t clock_div = (ctrl >> SPI_CTRL_CDIV0) & 0xf;

  uint32_t tmp = 2 * PRSC_LUT[prsc_sel] * (1 + clock_div);
  return astrarv32_sysinfo_get_clk() / tmp;
}


/**********************************************************************//**
 * Disable SPI controller.
 **************************************************************************/
void astrarv32_spi_disable(void) {

  __MMREG32_BCLR(ASTRARV32_SPI->CTRL, 1 << SPI_CTRL_EN);
}


/**********************************************************************//**
 * Enable SPI controller.
 **************************************************************************/
void astrarv32_spi_enable(void) {

  __MMREG32_BSET(ASTRARV32_SPI->CTRL, 1 << SPI_CTRL_EN);
}


/**********************************************************************//**
 * Get SPI FIFO depth.
 *
 * @return FIFO depth (number of entries), zero if no FIFO implemented
 **************************************************************************/
int astrarv32_spi_get_fifo_depth(void) {

  uint32_t tmp = (ASTRARV32_SPI->CTRL >> SPI_CTRL_FIFO_LSB) & 0x0f;
  return (int)(1 << tmp);
}


/**********************************************************************//**
 * Activate single SPI chip select signal.
 *
 * @note The SPI chip select output lines are LOW when activated.
 * @note This function is blocking.
 *
 * @param cs Chip select line to activate (0..7).
 **************************************************************************/
void astrarv32_spi_cs_en(int cs) {

  while (ASTRARV32_SPI->CTRL & (1<<SPI_CTRL_TX_FULL)); // wait for free space in TX FIFO
  astrarv32_spi_cs_en_nonblocking(cs);
}


/**********************************************************************//**
 * Deactivate currently active SPI chip select signal.
 *
 * @note The SPI chip select output lines are HIGH when deactivated.
 * @note This function is blocking.
 **************************************************************************/
void astrarv32_spi_cs_dis(void) {

  while (ASTRARV32_SPI->CTRL & (1<<SPI_CTRL_TX_FULL)); // wait for free space in TX FIFO
  astrarv32_spi_cs_dis_nonblocking();
}


/**********************************************************************//**
 * Perform a single SPI data transfer.
 *
 * @note This function is blocking.
 *
 * @param tx_data Transmit data (8-bit, LSB-aligned).
 * @return Receive data (8-bit, LSB-aligned).
 **************************************************************************/
uint8_t astrarv32_spi_transfer(uint8_t tx_data) {

  astrarv32_spi_put_nonblocking(tx_data);
  while (astrarv32_spi_busy()); // wait for current transfer to finish
  return astrarv32_spi_get_nonblocking();
}


/**********************************************************************//**
 * Put SPI TX data (non-blocking).
 *
 * @param tx_data Transmit data (8-bit, LSB-aligned).
 **************************************************************************/
void astrarv32_spi_put_nonblocking(uint8_t tx_data) {

  ASTRARV32_SPI->DATA = (0 << SPI_DATA_CMD) | ((uint32_t)tx_data); // put data into TX FIFO
}


/**********************************************************************//**
 * Get SPI RX data (non-blocking).
 *
 * @return Receive data (8-bit, LSB-aligned).
 **************************************************************************/
uint8_t astrarv32_spi_get_nonblocking(void) {

  return (uint8_t)ASTRARV32_SPI->DATA;
}


/**********************************************************************//**
 * Activate single SPI chip select signal (non-blocking).
 *
 * @note The SPI chip select output lines are LOW when activated.
 *
 * @param cs Chip select line to activate (0..7).
 **************************************************************************/
void astrarv32_spi_cs_en_nonblocking(int cs) {

  ASTRARV32_SPI->DATA = (1 << SPI_DATA_CMD) | ((1 << SPI_DATA_CSEN) + (cs & 7)); // put CS command into TX FIFO
}


/**********************************************************************//**
 * Deactivate currently active SPI chip select signal (non-blocking).
 *
 * @note The SPI chip select output lines are HIGH when deactivated.
 **************************************************************************/
void astrarv32_spi_cs_dis_nonblocking(void) {

  ASTRARV32_SPI->DATA = (1 << SPI_DATA_CMD) | 0; // put CS command into TX FIFO
}


/**********************************************************************//**
 * Check if any chip-select line is active.
 *
 * @return Zero if no CS lines are active, non-zero if a CS line is active.
 **************************************************************************/
int astrarv32_spi_check_cs(void) {

  return (int)(ASTRARV32_SPI->CTRL & (1 << SPI_CS_ACTIVE));
}


/**********************************************************************//**
 * Check if RX FIFO data is available.
 *
 * @return Zero if no data available, non-zero if RX FIFO data is available.
 **************************************************************************/
int astrarv32_spi_rx_avail(void) {

  return (int)(ASTRARV32_SPI->CTRL & (1 << SPI_CTRL_RX_AVAIL));
}


/**********************************************************************//**
 * Check if TX FIFO is empty.
 *
 * @return Zero if RX FIFO is not empty, non-zero if RX FIFO is empty.
 **************************************************************************/
int astrarv32_spi_tx_empty(void) {

  return (int)(ASTRARV32_SPI->CTRL & (1 << SPI_CTRL_TX_EMPTY));
}


/**********************************************************************//**
 * Check if TX FIFO is full.
 *
 * @return Zero if TX FIFO is not full, non-zero if TX FIFO is full.
 **************************************************************************/
int astrarv32_spi_tx_full(void) {

  return (int)(ASTRARV32_SPI->CTRL & (1 << SPI_CTRL_TX_FULL));
}


/**********************************************************************//**
 * Check if SPI transceiver is busy or TX FIFO not empty.
 *
 * @return 0 if idle, non-zero if busy
 **************************************************************************/
int astrarv32_spi_busy(void) {

  return (int)(ASTRARV32_SPI->CTRL & (1 << SPI_CTRL_BUSY));
}
