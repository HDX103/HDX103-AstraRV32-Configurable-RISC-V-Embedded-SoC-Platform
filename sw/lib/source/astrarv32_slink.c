// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_slink.c
 * @brief Stream Link Interface HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if stream link interface was synthesized.
 *
 * @return Zero if SLINK was not synthesized, non-zero if SLINK is available.
 **************************************************************************/
int astrarv32_slink_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_SLINK));
}


/**********************************************************************//**
 * Reset, enable and configure SLINK.
 *
 * @param[in] irq_mask Interrupt conditions (#ASTRARV32_SLINK_CTRL_enum).
 **************************************************************************/
void astrarv32_slink_setup(uint32_t irq_mask) {

  ASTRARV32_SLINK->CTRL = 0; // reset and disable

  const uint32_t mask = (1 << SLINK_CTRL_IRQ_RX_NEMPTY) |
                        (1 << SLINK_CTRL_IRQ_RX_FULL)   |
                        (1 << SLINK_CTRL_IRQ_TX_EMPTY)  |
                        (1 << SLINK_CTRL_IRQ_TX_NFULL);

  uint32_t tmp = (uint32_t)(1 << SLINK_CTRL_EN);
  ASTRARV32_SLINK->CTRL = tmp | (irq_mask & mask);
}


/**********************************************************************//**
 * Get FIFO depth of RX link.
 *
 * @return FIFO depth of RX link.
 **************************************************************************/
int astrarv32_slink_get_rx_fifo_depth(void) {

  uint32_t tmp = (ASTRARV32_SLINK->CTRL >> SLINK_CTRL_RX_FIFO_LSB) & 0x0f;
  return (int)(1 << tmp);
}


/**********************************************************************//**
 * Get FIFO depth of TX link.
 *
 * @return FIFO depth of TX link.
 **************************************************************************/
int astrarv32_slink_get_tx_fifo_depth(void) {

  uint32_t tmp = (ASTRARV32_SLINK->CTRL >> SLINK_CTRL_TX_FIFO_LSB) & 0x0f;
  return (int)(1 << tmp);
}


/**********************************************************************//**
 * Check if RX FIFO is empty.
 *
 * @return Zero if RX FIFO is not empty, non-zero if RX FIFO is empty.
 **************************************************************************/
int astrarv32_slink_rx_empty(void) {

  return (int)(ASTRARV32_SLINK->CTRL & (1 << SLINK_CTRL_RX_EMPTY));
}


/**********************************************************************//**
 * Check if RX FIFO is full.
 *
 * @return Zero if RX FIFO is not full, non-zero if RX FIFO is full.
 **************************************************************************/
int astrarv32_slink_rx_full(void) {

  return (int)(ASTRARV32_SLINK->CTRL & (1 << SLINK_CTRL_RX_FULL));
}


/**********************************************************************//**
 * Check if TX FIFO is empty.
 *
 * @return Zero if RX FIFO is not empty, non-zero if RX FIFO is empty.
 **************************************************************************/
int astrarv32_slink_tx_empty(void) {

  return (int)(ASTRARV32_SLINK->CTRL & (1 << SLINK_CTRL_TX_EMPTY));
}


/**********************************************************************//**
 * Check if TX FIFO is full.
 *
 * @return Zero if TX FIFO is not full, non-zero if TX FIFO is full.
 **************************************************************************/
int astrarv32_slink_tx_full(void) {

  return (int)(ASTRARV32_SLINK->CTRL & (1 << SLINK_CTRL_TX_FULL));
}
