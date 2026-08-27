// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_pwm.h
 * @brief Pulse-Width Modulation Controller (PWM) HW driver header file.
 */

#ifndef ASTRARV32_PWM_H
#define ASTRARV32_PWM_H

#include <astrarv32.h>
#include <stdint.h>

/**********************************************************************//**
 * @name IO Device: Pulse Width Modulation Controller (PWM)
 **************************************************************************/
/**@{*/
/** PWM module prototype */
typedef volatile struct __attribute__((packed,aligned(4))) {
  uint32_t ENABLE;             /**< per-channel enable */
  uint32_t POLARITY;           /**< per-channel polarity */
  uint32_t CLKPRSC;            /**< global clock prescaler */
  uint32_t MODE;               /**< per-channel operation mode */
  const uint32_t reserved[28]; /**< reserved */
  union {
    uint32_t TOPCMP; /**< full 32-bit channel access */
    struct {
      uint16_t CMP;  /**< per-channel counter compare value */
      uint16_t TOP;  /**< per-channel counter wrap value */
    };
  } CHANNEL[32];
} astrarv32_pwm_t;

/** PWM module hardware handle (#astrarv32_pwm_t) */
#define ASTRARV32_PWM ((astrarv32_pwm_t*) (ASTRARV32_PWM_BASE))
/**@}*/


/**********************************************************************//**
 * @name Prototypes
 **************************************************************************/
/**@{*/
int  astrarv32_pwm_available(void);
int  astrarv32_pwm_get_num_channels(void);
void astrarv32_pwm_set_clock(int prsc);
void astrarv32_pwm_ch_enable_mask(uint32_t mask);
void astrarv32_pwm_ch_disable_mask(uint32_t mask);
void astrarv32_pwm_ch_enable_single(int ch);
void astrarv32_pwm_ch_disable_single(int ch);
void astrarv32_pwm_ch_setup(int ch, int top, int pol, int mode);
void astrarv32_pwm_ch_set_duty(int ch, int duty);
/**@}*/

#endif // ASTRARV32_PWM_H
