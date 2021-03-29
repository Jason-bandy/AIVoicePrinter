/**
 ****************************************************************************************
 *
 * @file arch.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */


#ifndef _ARCH_H_
#define _ARCH_H_

/**
 ****************************************************************************************
 * @defgroup PLATFORM_DRIVERS PLATFORM
 * @ingroup MACSW
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup ARCH ARCH
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the ATMEL AT91SAM261 architecture API.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// required to define GLOBAL_INT_** macros as inline assembly
#include "boot.h"
#include "ll.h"
#include "compiler.h"
#include "rwnx_config.h"

/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
/// ARM is a 32-bit CPU
#define CPU_WORD_SIZE   4

/*
 * CPU Endianness
 ****************************************************************************************
 */
/// ARM is little endian
#define CPU_LE          1

/// Macro checking if a pointer is part of the shared RAM
#define TST_SHRAM_PTR(ptr) ((((uint32_t)(ptr)) < (uint32_t)_sshram) ||                   \
                            (((uint32_t)(ptr)) >= (uint32_t)_eshram))

/// Macro checking if a pointer is part of the shared RAM
#define CHK_SHRAM_PTR(ptr) { if (TST_SHRAM_PTR(ptr)) return;}

/// @}
/// @}
#endif // _ARCH_H_
