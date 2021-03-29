/**
 ****************************************************************************************
 *
 * @file arch.h
 *
 * @brief This file contains the definitions of the macros and functions that are
 * architecture dependent.  The implementation of those is implemented in the
 * appropriate architecture directory.
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */


#ifndef _ARCH_H_
#define _ARCH_H_

/**
 ****************************************************************************************
 * @defgroup PLATFORM_DRIVERS PLATFORM
 * @ingroup MACSW
 * @brief Description of the RW reference FPGA platform.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup ARCH ARCH
 * @ingroup PLATFORM_DRIVERS
 * @brief Specific Ceva TL4 CPU architecture.
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// required to define GLOBAL_INT_** macros as inline assembly
#include "compiler.h"
#include "rwnx_config.h"
#include "boot.h"

/*
 * CPU WORD SIZE
 ****************************************************************************************
 */
// 32bit word size
#define CPU_WORD_SIZE   4

/*
 * CPU Endianness
 ****************************************************************************************
 */
/// risc-v is little endian
#define CPU_LE          1

/*
 * Shared RAM CHECK
 ****************************************************************************************
 */
/// Macro checking if a pointer is part of the shared RAM
#define TST_SHRAM_PTR(ptr) ((((uint32_t)(ptr)) < (uint32_t)_sshram) ||                   \
                            (((uint32_t)(ptr)) >= (uint32_t)_eshram))

/// Macro checking if a pointer is part of the shared RAM
#define CHK_SHRAM_PTR(ptr) { if (TST_SHRAM_PTR(ptr)) return;}

#include "ll.h"


/// @}
#endif // _ARCH_H_
