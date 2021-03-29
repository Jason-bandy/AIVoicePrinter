/**
 ****************************************************************************************
 *
 * @file gnuarm/compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifndef __GNUC__
#error "File only included with ARM GCC"
#endif

/// define the force inlining attribute for this compiler
#define __INLINE static __attribute__((__always_inline__)) inline

/// define the IRQ handler attribute for this compiler
#define __IRQ __attribute__((__interrupt__("IRQ")))

/// define the FIQ handler attribute for this compiler
#define __FIQ __attribute__((__interrupt__("FIQ")))

/// Function returns struct in registers (4 in rvds, var with gnuarm).
/// With Gnuarm, feature depends on command line options and
/// impacts ALL functions returning 2-words max structs
/// (check -freg-struct-return and -mabi=xxx)
#define __VIR

/// function has no side effect and return depends only on arguments
#define __PURE __attribute__((const))

/// symbol is defined weakly (silently replaced by NULL at link if not defined)
/// GCC 3.4 for ARM may not accept it, must be tested. Error meanwhile.
//#if __GNUC__ >= 4
//#define __WEAK __attribute__((weak))
//#else
//#error "check weak with gnuarm 3.4"
//#endif

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__((aligned(4)))

/// Pack a structure field
#define __PACKED16 __attribute__ ((__packed__))
#define __PACKED __attribute__ ((__packed__))

/// __MODULE__ comes from the RVDS compiler that supports it
#define __MODULE__ __BASE_FILE__

/// define a variable as maybe unused, to avoid compiler warnings on it
#define __MAYBE_UNUSED __attribute__((unused))

// Mapping of these different elements is already handled in the map.txt file, so no need
// to define anything here
#define __SHAREDRAMIPC __attribute__ ((section("SHAREDRAMIPC")))
#define __SHAREDRAM __attribute__ ((section("SHAREDRAM")))
#define __LARAMMAC __attribute__ ((section("LARAM")))
#define __MIB __attribute__ ((section("MACHWMIB")))

#endif // _COMPILER_H_
