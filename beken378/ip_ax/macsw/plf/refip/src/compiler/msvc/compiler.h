/**
 ****************************************************************************************
 *
 * @file msvc/compiler.h
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
#define __INLINE __forceinline

/// define the interrupt handler attribute for this compiler
#define __IRQ

/// function returns struct in registers (4 words max, var with gnuarm)
#define __VIR


#endif // _COMPILER_H_
