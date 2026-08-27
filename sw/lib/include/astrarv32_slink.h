// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_slink.h
 * @brief Stream Link Interface HW driver header file.
 */

#ifndef ASTRARV32_SLINK_H
#define ASTRARV32_SLINK_H

#include <astrarv32.h>
#include <stdint.h>

/**********************************************************************//**
 * @name IO Device: Stream Link Interface (SLINK)
 **************************************************************************/
/**@{*/
/** SLINK module prototype */
typedef volatile struct __attribute__((packed,aligned(4))) {
  uint32_t CTRL;      /**< control register (#ASTRARV32_SLINK_CTRL_enum) */
  uint32_t ROUTE;     /**< routing information */
  uint32_t DATA;      /**< RX/TX data register */
  uint32_t DATA_LAST; /**< RX/TX data register (+ TX end-of-stream) */
} astrarv32_slink_t;

/** SLINK module hardware handle (#astrarv32_slink_t) */
#define ASTRARV32_SLINK ((astrarv32_slink_t*) (ASTRARV32_SLINK_BASE))

/** SLINK control register bits */
enum ASTRARV32_SLINK_CTRL_enum {
  SLINK_CTRL_EN            =  0, /**< SLINK control register(0)  (r/w): SLINK unit enable */

  SLINK_CTRL_RX_EMPTY      =  8, /**< SLINK control register(8)  (r/-): RX FIFO empty */
  SLINK_CTRL_RX_FULL       =  9, /**< SLINK control register(9)  (r/-): RX FIFO full */
  SLINK_CTRL_TX_EMPTY      = 10, /**< SLINK control register(10) (r/-): TX FIFO empty */
  SLINK_CTRL_TX_FULL       = 11, /**< SLINK control register(11) (r/-): TX FIFO full */
  SLINK_CTRL_RX_LAST       = 12, /**< SLINK control register(1)  (r/-): RX end-of-stream delimiter */

  SLINK_CTRL_IRQ_RX_NEMPTY = 16, /**< SLINK control register(16) (r/w): interrupt if RX FIFO not empty */
  SLINK_CTRL_IRQ_RX_FULL   = 17, /**< SLINK control register(17) (r/w): interrupt if RX FIFO full */
  SLINK_CTRL_IRQ_TX_EMPTY  = 18, /**< SLINK control register(18) (r/w): interrupt if TX FIFO empty */
  SLINK_CTRL_IRQ_TX_NFULL  = 19, /**< SLINK control register(19) (r/w): interrupt if TX FIFO not full */

  SLINK_CTRL_RX_FIFO_LSB   = 24, /**< SLINK control register(24) (r/-): log2(RX FIFO size) LSB */
  SLINK_CTRL_RX_FIFO_MSB   = 27, /**< SLINK control register(27) (r/-): log2(RX FIFO size) MSB */
  SLINK_CTRL_TX_FIFO_LSB   = 28, /**< SLINK control register(28) (r/-): log2(TX FIFO size) LSB */
  SLINK_CTRL_TX_FIFO_MSB   = 31  /**< SLINK control register(31) (r/-): log2(TX FIFO size) MSB */
};
/**@}*/


/**********************************************************************//**
 * @name Prototypes
 **************************************************************************/
/**@{*/
int  astrarv32_slink_available(void);
void astrarv32_slink_setup(uint32_t irq_mask);
int  astrarv32_slink_get_rx_fifo_depth(void);
int  astrarv32_slink_get_tx_fifo_depth(void);
int  astrarv32_slink_rx_empty(void);
int  astrarv32_slink_rx_full(void);
int  astrarv32_slink_tx_empty(void);
int  astrarv32_slink_tx_full(void);
/**@}*/


/**********************************************************************//**
 * Read data from RX link (non-blocking).
 * @return Data received from link.
 **************************************************************************/
static inline uint32_t __attribute__((always_inline)) astrarv32_slink_get(void) {
  return ASTRARV32_SLINK->DATA;
}

/**********************************************************************//**
 * Check if last RX word has "end-of-stream" delimiter.
 * @note This function must be called AFTER reading the actual data word using #astrarv32_slink_get(void).
 * @return Zero if not end of stream, non-zero if end of stream.
 **************************************************************************/
static inline int __attribute__((always_inline)) astrarv32_slink_check_last(void) {
  return (int)(ASTRARV32_SLINK->CTRL & (1 << SLINK_CTRL_RX_LAST));
}

/**********************************************************************//**
 * Set TX link routing destination.
 * @note This function must be called BEFORE sending the actual data word using #astrarv32_slink_put(void).
 * @param[in] dst Routing destination ID (4-bit, LSB-aligned).
 **************************************************************************/
static inline void __attribute__((always_inline)) astrarv32_slink_set_dst(uint32_t dst) {
  ASTRARV32_SLINK->ROUTE = dst;
}

/**********************************************************************//**
 * Get RX link routing source.
 * @note This function must be called AFTER reading the actual data word using #astrarv32_slink_get(void).
 * @return 4-bit source routing ID.
 **************************************************************************/
static inline uint32_t __attribute__((always_inline)) astrarv32_slink_get_src(void) {
  return ASTRARV32_SLINK->ROUTE;
}

/**********************************************************************//**
 * Write data to TX link (non-blocking).
 * @param[in] tx_data Data to send.
 **************************************************************************/
static inline void __attribute__((always_inline)) astrarv32_slink_put(uint32_t tx_data) {
  ASTRARV32_SLINK->DATA = tx_data;
}

/**********************************************************************//**
 * Write data to TX link (non-blocking) and set "last" (end-of-stream) delimiter.
 * @param[in] tx_data Data to send.
 **************************************************************************/
static inline void __attribute__((always_inline)) astrarv32_slink_put_last(uint32_t tx_data) {
  ASTRARV32_SLINK->DATA_LAST = tx_data;
}

#endif // ASTRARV32_SLINK_H
