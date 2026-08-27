// ================================================================================ //
// The ASTRARV32 RISC-V Processor - https://github.com/stnolting/astrarv32              //
// Copyright (c) ASTRARV32 contributors.                                              //
// Copyright (c) 2020 - 2026 Stephan Nolting. All rights reserved.                  //
// Licensed under the BSD-3-Clause license, see LICENSE for details.                //
// SPDX-License-Identifier: BSD-3-Clause                                            //
// ================================================================================ //

/**
 * @file astrarv32_cfs.c
 * @brief Custom Functions Subsystem (CFS) HW driver source file.
 *
 * @warning There are no "real" CFS driver functions available here, because these functions are defined by the actual hardware.
 * @warning Hence, the CFS designer has to provide the actual driver functions.
 *
 * @see https://stnolting.github.io/astrarv32/sw/files.html
 */

#include <astrarv32.h>


/**********************************************************************//**
 * Check if custom functions subsystem was synthesized.
 *
 * @return 0 if CFS was not synthesized, non-zero if CFS is available.
 **************************************************************************/
int astrarv32_cfs_available(void) {

  return (int)(ASTRARV32_SYSINFO->SOC & (1 << SYSINFO_SOC_IO_CFS));
}

