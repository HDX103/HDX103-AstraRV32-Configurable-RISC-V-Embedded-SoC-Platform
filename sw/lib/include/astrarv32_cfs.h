// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_cfs.h
 * @brief Custom Functions Subsystem (CFS) HW driver header file.
 *
 * @warning There are no "real" CFS driver functions available here, because these functions are defined by the actual hardware.
 * @warning The CFS designer has to provide the actual driver functions.
 */

#ifndef ASTRARV32_CFS_H
#define ASTRARV32_CFS_H

#include <astrarv32.h>
#include <stdint.h>

/**********************************************************************//**
 * @name IO Device: Custom Functions Subsystem (CFS)
 **************************************************************************/
/**@{*/
/** CFS module prototype */
typedef volatile struct __attribute__((packed,aligned(4))) {
  uint32_t REG[(64*1024)/4]; /**< CFS registers, user-defined */
} astrarv32_cfs_t;

/** CFS module hardware handle (#astrarv32_cfs_t) */
#define ASTRARV32_CFS ((astrarv32_cfs_t*) (ASTRARV32_CFS_BASE))
/**@}*/


/**********************************************************************//**
 * @name Prototypes
 **************************************************************************/
/**@{*/
int astrarv32_cfs_available(void);
/**@}*/


#endif // ASTRARV32_CFS_H
