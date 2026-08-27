// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_sdi.c
 * @brief Serial data interface controller (SDI) HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if SDI unit was synthesized.
 *
 * @return 0 if SDI was not synthesized, non-zero if SPI is available.
 **************************************************************************/
int astrarv32_sdi_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_SDI));
}


/**********************************************************************//**
 * Reset, enable and configure SDI controller.
 * The SDI control register bits are listed in #ASTRARV32_SDI_CTRL_enum.
 *
 * @param[in] irq_mask Interrupt configuration bit mask (CTRL's irq_* bits).
 **************************************************************************/
void astrarv32_sdi_setup(uint32_t irq_mask) {

  ASTRARV32_SDI->CTRL = 0; // reset

  const uint32_t mask = (1 << SDI_CTRL_IRQ_RX_NEMPTY) |
                        (1 << SDI_CTRL_IRQ_RX_FULL)   |
                        (1 << SDI_CTRL_IRQ_TX_EMPTY);

  uint32_t tmp = (uint32_t)(1 << SDI_CTRL_EN);
  ASTRARV32_SDI->CTRL = tmp | (irq_mask & mask);
}


/**********************************************************************//**
 * Disable SDI controller.
 **************************************************************************/
void astrarv32_sdi_disable(void) {

  __MMREG32_BCLR(ASTRARV32_SDI->CTRL, 1 << SDI_CTRL_EN);
}


/**********************************************************************//**
 * Enable SDI controller.
 **************************************************************************/
void astrarv32_sdi_enable(void) {

  __MMREG32_BSET(ASTRARV32_SDI->CTRL, 1 << SDI_CTRL_EN);
}


/**********************************************************************//**
 * Get SDI FIFO depth.
 *
 * @return FIFO depth (number of entries).
 **************************************************************************/
int astrarv32_sdi_get_fifo_depth(void) {

  uint32_t tmp = (ASTRARV32_SDI->CTRL >> SDI_CTRL_FIFO_LSB) & 0x0f;
  return (int)(1 << tmp);
}


/**********************************************************************//**
 * Push data to SDI output FIFO (blocking).
 *
 * @param[in] data Byte to push into TX FIFO.
 **************************************************************************/
void astrarv32_sdi_put(uint8_t data) {

  while (ASTRARV32_SDI->CTRL & (1 << SDI_CTRL_TX_FULL));
  ASTRARV32_SDI->DATA = (uint32_t)data;
}


/**********************************************************************//**
 * Get data from SDI input FIFO (blocking).
 *
 * @return Data byte read from RX FIFO.
 **************************************************************************/
uint8_t astrarv32_sdi_get(void) {

  while (ASTRARV32_SDI->CTRL & (1 << SDI_CTRL_RX_EMPTY));
  return (uint8_t)ASTRARV32_SDI->DATA;
}


/**********************************************************************//**
 * Push data to SDI output FIFO (non-blocking).
 *
 * @param[in] data Byte to push into TX FIFO.
 **************************************************************************/
void astrarv32_sdi_put_nonblocking(uint8_t data) {

  ASTRARV32_SDI->DATA = (uint32_t)data;
}


/**********************************************************************//**
 * Get data from SDI input FIFO (non-blocking).
 *
 * @return Data byte read from RX FIFO.
 **************************************************************************/
uint8_t astrarv32_sdi_get_nonblocking(void) {

  return (uint8_t)ASTRARV32_SDI->DATA;
}


/**********************************************************************//**
 * Check if RX FIFO is empty.
 *
 * @return Zero if RX FIFO is not empty, non-zero if RX FIFO is empty.
 **************************************************************************/
int astrarv32_sdi_rx_empty(void) {

  return (int)(ASTRARV32_SDI->CTRL & (1 << SDI_CTRL_RX_EMPTY));
}


/**********************************************************************//**
 * Check if RX FIFO is full.
 *
 * @return Zero if RX FIFO is not full, non-zero if RX FIFO is full.
 **************************************************************************/
int astrarv32_sdi_rx_full(void) {

  return (int)(ASTRARV32_SDI->CTRL & (1 << SDI_CTRL_RX_FULL));
}


/**********************************************************************//**
 * Check if TX FIFO is empty.
 *
 * @return Zero if RX FIFO is not empty, non-zero if RX FIFO is empty.
 **************************************************************************/
int astrarv32_sdi_tx_empty(void) {

  return (int)(ASTRARV32_SDI->CTRL & (1 << SDI_CTRL_TX_EMPTY));
}


/**********************************************************************//**
 * Check if TX FIFO is full.
 *
 * @return Zero if TX FIFO is not full, non-zero if TX FIFO is full.
 **************************************************************************/
int astrarv32_sdi_tx_full(void) {

  return (int)(ASTRARV32_SDI->CTRL & (1 << SDI_CTRL_TX_FULL));
}


/**********************************************************************//**
 * Clear RX FIFO.
 **************************************************************************/
void astrarv32_sdi_rx_clear(void) {

  __MMREG32_BSET(ASTRARV32_SDI->CTRL, 1 << SDI_CTRL_CLR_RX);
}


/**********************************************************************//**
 * Clear TX FIFO.
 **************************************************************************/
void astrarv32_sdi_tx_clear(void) {

  __MMREG32_BSET(ASTRARV32_SDI->CTRL, 1 << SDI_CTRL_CLR_TX);
}


/**********************************************************************//**
 * Get status of chip-select line.
 *
 * @return Non-zero if chip-select line is enabled/active (driven low), zero otherwise.
 **************************************************************************/
int astrarv32_sdi_check_cs(void) {

  return (int)(ASTRARV32_SDI->CTRL & (1 << SDI_CTRL_CS_ACTIVE));
}
