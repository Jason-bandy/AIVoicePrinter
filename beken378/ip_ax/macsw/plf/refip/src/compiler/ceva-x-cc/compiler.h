/**
 ****************************************************************************************
 *
 * @file compiler.h
 *
 * @brief Definitions of CEVA TL4 compiler specific directives.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_


/**
 ****************************************************************************************
 * @defgroup COMPILER COMPILER
 * @ingroup ARCH
 * @{
 * ****************************************************************************************
 */
/// define the force inlining attribute for this compiler - doesn't have it as attribute
#define __INLINE  static __inline__

/// define the IRQ handler attribute for this compiler
#define __IRQ __attribute__ ((interrupt))

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__ ((aligned(4)))
/// Align instantiated lvalue or struct member on 2 bytes
#define __ALIGN2 __attribute__ ((aligned(2)))

/// __MODULE__ comes from the RVDS compiler that supports it
#define __MODULE__ __FILE__

/// Pack a structure field on 16-bit boundary
#define __PACKED16
/// Pack a structure field
#define __PACKED __attribute__ ((__packed__))

/// define a variable as maybe unused, to avoid compiler warnings on it
#define __MAYBE_UNUSED __attribute__((unused))

/// Memory section definition for the IPC shared structure
#define __SHAREDRAMIPC __attribute__ ((section(".DSECT SHRAMIPC")))
/// Memory section definition for the shared RAM
#define __SHAREDRAM __attribute__ ((section(".DSECT SHRAM")))
/// Memory section definition for the MAC HW embedded logic analyzer
#define __LARAMMAC __attribute__ ((section(".DSECT LARAMMAC")))
/// Memory section definition for the MAC HW MIB
#define __MIB __attribute__ ((section(".DSECT MIB")))

/// @}


#endif // _COMPILER_H_
