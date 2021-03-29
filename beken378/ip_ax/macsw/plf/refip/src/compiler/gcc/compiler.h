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
#define __IRQ

/// define the interrupt handler attribute for this compiler
#define __FIQ


/// function returns struct in registers (4 words max, var with gnuarm)
#define __VIR

/// symbol is defined weakly (silently replaced by NULL at link if not defined)
/// GCC 3.4 on cygwin does not accept weak references, but we do not
/// need it for unittests
//#if __GNUC__ >= 4
//#define __WEAK __attribute__((weak))
//#else
//#define __WEAK __attribute__((weak))
//#endif

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__((aligned(4)))

/// define a variable as maybe unused, to avoid compiler warnings on it
#define __MAYBE_UNUSED __attribute__((unused))

#endif // _COMPILER_H_
