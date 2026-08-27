// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_smp.h
 * @brief Symmetric multiprocessing (SMP) library header file.
 */

#ifndef ASTRARV32_SMP_H
#define ASTRARV32_SMP_H

#include <astrarv32.h>

/**********************************************************************//**
 * @name Prototypes
 **************************************************************************/
/**@{*/
int astrarv32_smp_launch(int (*entry_point)(void), uint8_t* stack_memory, size_t stack_size_bytes);
/**@}*/

/**********************************************************************//**
 * Get core/hart ID of the CPU that is executing this function.
 * @return Core ID from mhartid CSR.
 **************************************************************************/
static inline uint32_t __attribute__ ((always_inline)) astrarv32_smp_whoami(void) {
  return astrarv32_cpu_csr_read(CSR_MHARTID);
}

#endif // ASTRARV32_SMP_H
