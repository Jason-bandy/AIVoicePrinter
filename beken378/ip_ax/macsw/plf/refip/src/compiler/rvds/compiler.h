/**
 ****************************************************************************************
 *
 * @file rvds/compiler.h
 *
 * @brief Definitions of compiler specific directives.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifndef __ARMCC_VERSION
#error "File only included with RVDS!"
#endif

/// define the force inlining attribute for this compiler
#define __INLINE __forceinline static

/// define the IRQ handler attribute for this compiler
#define __IRQ __irq

/// define the FIQ handler attribute for this compiler
#define __FIQ __irq

/// function returns struct in registers (4 words max, var with gnuarm)
#define __VIR __value_in_regs

/// function has no side effect and return depends only on arguments
#define __PURE __pure

/// symbol is defined weakly (silently replaced by NULL at link if not defined)
//#define __WEAK __weak

/// Align instantiated lvalue or struct member on 4 bytes. Despite what the doc is saying,
/// this keyword is different to __align(x) which cannot be used for a struct member.
#define __ALIGN4 __attribute__((aligned(4)))

// allow anonymous unions or structures (in gnuarm, this is a command line option).
#pragma anon_unions

#endif // _COMPILER_H_
