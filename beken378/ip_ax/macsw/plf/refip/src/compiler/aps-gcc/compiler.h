/**
 ****************************************************************************************
 *
 * @file gcc/compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

/// define the force inlining attribute for this compiler
#define __INLINE static __attribute__((__always_inline__)) inline

/// define the interrupt handler attribute for this compiler
#if 0
#define __IRQ __attribute__((interrupt))
#else
#define __IRQ
#endif

/// define the BT IRQ handler attribute for this compiler
#define __BTIRQ

/// define the BLE IRQ handler attribute for this compiler
#define __BLEIRQ

/// define the interrupt handler attribute for this compiler
#define __FIQ

/// __MODULE__ comes from the RVDS compiler that supports it
#define __MODULE__ __BASENAME_FILE__

/// Pack a structure field
#define __PACKED16 __attribute__ ((__packed__))
#define __PACKED __attribute__ ((__packed__))

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__((aligned(4)))

/// function returns struct in registers (4 words max, var with gnuarm)
#define __VIR

/// define a variable as maybe unused, to avoid compiler warnings on it
#define __MAYBE_UNUSED __attribute__((unused))

// Mapping of these different elements is already handled in the map.txt file, so no need
// to define anything here
#define __SHAREDRAMIPC __attribute__ ((section("SHAREDRAMIPC")))
#define __SHAREDRAM __attribute__ ((section("SHAREDRAM")))
#define __LARAMMAC __attribute__ ((section("LARAM")))
#define __MIB __attribute__ ((section("MACHWMIB")))

#endif // _COMPILER_H_
