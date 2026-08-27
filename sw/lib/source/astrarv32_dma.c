// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_dma.c
 * @brief Direct Memory Access Controller (DMA) HW driver source file.
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if DMA controller was synthesized.
 *
 * @return 0 if DMA was not synthesized, non-zero if DMA is available.
 **************************************************************************/
int astrarv32_dma_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_DMA));
}


/**********************************************************************//**
 * Get DMA descriptor FIFO depth.
 *
 * @return FIFO depth (number of entries)
 **************************************************************************/
int astrarv32_dma_get_descriptor_fifo_depth(void) {

  uint32_t tmp = (ASTRARV32_DMA->CTRL >> DMA_CTRL_DFIFO_LSB) & 0xf;
  return (int)(1 << tmp);
}


/**********************************************************************//**
 * Check if descriptor FIFO is full.
 *
 * @return Non-zero if FIFO is full, zero otherwise.
 **************************************************************************/
int astrarv32_dma_descriptor_fifo_full(void) {

  return (int)(ASTRARV32_DMA->CTRL & (1 << DMA_CTRL_DFULL));
}


/**********************************************************************//**
 * Check if descriptor FIFO is empty.
 *
 * @return Non-zero if FIFO is empty, zero otherwise.
 **************************************************************************/
int astrarv32_dma_descriptor_fifo_empty(void) {

  return (int)(ASTRARV32_DMA->CTRL & (1 << DMA_CTRL_DEMPTY));
}


/**********************************************************************//**
 * Enable DMA.
 **************************************************************************/
void astrarv32_dma_enable(void) {

  __MMREG32_BSET(ASTRARV32_DMA->CTRL, 1 << DMA_CTRL_EN);
}


/**********************************************************************//**
 * Disable DMA. This will reset the DMA and will also terminate the current transfer.
 **************************************************************************/
void astrarv32_dma_disable(void) {

  __MMREG32_BCLR(ASTRARV32_DMA->CTRL, 1 << DMA_CTRL_EN);
}


/**********************************************************************//**
 * Manually clear pending DMA interrupt. This will also clear the
 * transfer-error and transfer-done status flags.
 **************************************************************************/
void astrarv32_dma_irq_ack(void) {

  __MMREG32_BSET(ASTRARV32_DMA->CTRL, 1 << DMA_CTRL_ACK);
}


/**********************************************************************//**
 * Program DMA descriptor.
 *
 * @param[in] base_src Source data base address.
 * @param[in] base_dst Destination data base address.
 * @param[in] config Transfer type configuration (#ASTRARV32_DMA_CONF_enum).
 *
 * @return 0 if programming was successful; if the descriptor FIFO does not
 * provide enough space for the entire descriptor, a negative value is returned
 * that represents the number of missing FIFO entries.
 **************************************************************************/
int astrarv32_dma_program(uint32_t src_addr, uint32_t dst_addr, uint32_t config) {

  if (ASTRARV32_DMA->CTRL & (1 << DMA_CTRL_DFULL)) { return -3; } // three free entries too few
  ASTRARV32_DMA->DESC = src_addr;
  if (ASTRARV32_DMA->CTRL & (1 << DMA_CTRL_DFULL)) { return -2; } // two free entries too few
  ASTRARV32_DMA->DESC = dst_addr;
  if (ASTRARV32_DMA->CTRL & (1 << DMA_CTRL_DFULL)) { return -1; } // one free entry too few
  ASTRARV32_DMA->DESC = config;
  return 0;
}


/**********************************************************************//**
 * Program DMA descriptor (without checking FIFO level).
 *
 * @warning Descriptor FIFO might overflow. Use with care.
 *
 * @param[in] base_src Source data base address.
 * @param[in] base_dst Destination data base address.
 * @param[in] config Transfer type configuration (#ASTRARV32_DMA_CONF_enum).
 **************************************************************************/
void astrarv32_dma_program_nocheck(uint32_t src_addr, uint32_t dst_addr, uint32_t config) {

  ASTRARV32_DMA->DESC = src_addr;
  ASTRARV32_DMA->DESC = dst_addr;
  ASTRARV32_DMA->DESC = config;
}


/**********************************************************************//**
 * Trigger pre-programmed DMA transfer(s)
 **************************************************************************/
void astrarv32_dma_start(void) {

  __MMREG32_BSET(ASTRARV32_DMA->CTRL, 1 << DMA_CTRL_START);
}


/**********************************************************************//**
 * Get DMA status.
 *
 * @return Current DMA status (#ASTRARV32_DMA_STATUS_enum)
 **************************************************************************/
int astrarv32_dma_status(void) {

  uint32_t tmp = ASTRARV32_DMA->CTRL;

  if (tmp & (1 << DMA_CTRL_ERROR)) {
    return DMA_STATUS_ERROR; // error during transfer
  }
  else if (tmp & (1 << DMA_CTRL_BUSY)) {
    return DMA_STATUS_BUSY; // transfer in progress
  }
  else if (tmp & (1 << DMA_CTRL_DONE)) {
    return DMA_STATUS_DONE; // transfer done
  }
  else {
    return DMA_STATUS_IDLE; // idle
  }
}
