/**
 *****************************************************************************************
 *
 * @file critical_nesting.h
 *
 * @brief Declaration of the macros and functions used to manipulate the critical nesting
 * level variable.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 *****************************************************************************************
 */

#ifndef CRITICAL_NESTING_H_
#define CRITICAL_NESTING_H_

#ifdef CFG_RTOS
/*
 * In case an RTOS is used, the following two functions need to be defined. They are
 * used to increment and decrement the critical section nesting level possibly used
 * by the RTOS.
 * It allows continuing using our own critical section implementation (that has the
 * advantage to be usable in functions that can be executed both from interrupt and
 * background) while ensuring that a RTOS function called from a critical section
 * and itself using a critical section will not reenable the interrupts too early.
 */
/**
 *****************************************************************************************
 * @brief Increase the critical section nesting level.
 *****************************************************************************************
 */
void critical_nesting_inc(void);
/**
 *****************************************************************************************
 * @brief Decrease the critical section nesting level.
 *****************************************************************************************
 */
void critical_nesting_dec(void);

/// Macro used to increase the critical section nesting level
#define CRITICAL_NESTING_INC()  critical_nesting_inc()
/// Macro used to decrease the critical section nesting level
#define CRITICAL_NESTING_DEC()  critical_nesting_dec()
#else
/// Macro used to increase the critical section nesting level
#define CRITICAL_NESTING_INC()
/// Macro used to decrease the critical section nesting level
#define CRITICAL_NESTING_DEC()
#endif

#endif // CRITICAL_NESTING_H_
