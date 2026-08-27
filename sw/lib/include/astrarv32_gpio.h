// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_gpio.h
 * @brief General purpose input/output port unit (GPIO) HW driver header file.
 */

#ifndef ASTRARV32_GPIO_H
#define ASTRARV32_GPIO_H

#include <astrarv32.h>
#include <stdint.h>

/**********************************************************************//**
 * @name IO Device: General Purpose Input/Output Port Unit (GPIO)
 **************************************************************************/
/**@{*/
/** GPIO module prototype */
typedef volatile struct __attribute__((packed,aligned(4))) {
  const uint32_t PORT_IN;      /**< parallel input port, read-only */
  uint32_t       PORT_OUT;     /**< parallel output port */
  uint32_t       PORT_DIR;     /**< optional direction configuration: 0 = in, 1 = out */
  const uint32_t reserved;     /**< reserved */
  uint32_t       IRQ_TYPE;     /**< trigger type (#GPIO_TRIGGER_enum MSB) */
  uint32_t       IRQ_POLARITY; /**< trigger polarity (#GPIO_TRIGGER_enum LSB) */
  uint32_t       IRQ_ENABLE;   /**< interrupt enable */
  uint32_t       IRQ_PENDING;  /**< interrupt pending */
} astrarv32_gpio_t;

/** GPIO module hardware handle (#astrarv32_gpio_t) */
#define ASTRARV32_GPIO ((astrarv32_gpio_t*) (ASTRARV32_GPIO_BASE))
/**@}*/


/**********************************************************************//**
 * @name Trigger types
 **************************************************************************/
enum GPIO_TRIGGER_enum {
  GPIO_TRIG_LEVEL_LOW    = 0b00, // low-level
  GPIO_TRIG_LEVEL_HIGH   = 0b01, // high-level
  GPIO_TRIG_EDGE_FALLING = 0b10, // falling-edge
  GPIO_TRIG_EDGE_RISING  = 0b11  // rising-edge
};


/**********************************************************************//**
 * @name Prototypes
 **************************************************************************/
/**@{*/
int      astrarv32_gpio_available(void);
void     astrarv32_gpio_pin_set(int pin, int value);
void     astrarv32_gpio_pin_toggle(int pin);
uint32_t astrarv32_gpio_pin_get(int pin);
void     astrarv32_gpio_port_set(uint32_t pin_mask);
void     astrarv32_gpio_port_toggle(uint32_t pin_mask);
uint32_t astrarv32_gpio_port_get(void);
void     astrarv32_gpio_dir_set(uint32_t pin_mask);
uint32_t astrarv32_gpio_dir_get(void);
void     astrarv32_gpio_irq_setup(int pin, int trigger);
void     astrarv32_gpio_irq_enable(uint32_t pin_mask);
void     astrarv32_gpio_irq_disable(uint32_t pin_mask);
uint32_t astrarv32_gpio_irq_get(void);
void     astrarv32_gpio_irq_clr(uint32_t pin_mask);
/**@}*/


#endif // ASTRARV32_GPIO_H
