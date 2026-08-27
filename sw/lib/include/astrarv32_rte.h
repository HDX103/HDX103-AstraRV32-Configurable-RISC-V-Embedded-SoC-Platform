// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_rte.h
 * @brief ASTRARV32 Runtime Environment.
 */

#ifndef ASTRARV32_RTE_H
#define ASTRARV32_RTE_H

#include <astrarv32.h>
#include <stdint.h>

/**********************************************************************//**
 * @name Prototypes
 **************************************************************************/
/**@{*/
void     astrarv32_rte_setup(void);
int      astrarv32_rte_handler_install(uint32_t code, void (*handler)(void));
int      astrarv32_rte_handler_uninstall(uint32_t code);
uint32_t astrarv32_rte_context_get(int x);
void     astrarv32_rte_context_put(int x, uint32_t data);
/**@}*/

#endif // ASTRARV32_RTE_H
