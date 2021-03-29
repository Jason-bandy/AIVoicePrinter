/**
 *****************************************************************************************
 *
 * @file ll.h
 *
 * @brief Declaration of low level functions.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 *****************************************************************************************
 */

#ifndef LL_H_
#define LL_H_

#ifndef __GNUC__
#error "File only included with ARM GCC"
#endif

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "co_int.h"
#include "critical_nesting.h"

/**
 *****************************************************************************************
 * @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 *****************************************************************************************
 */
#define GLOBAL_INT_START()                                                               \
do {                                                                                     \
    *(uint32_t *)0xE000E100 = 1;                                                         \
    __asm volatile("CPSIE i");                                                           \
    __asm volatile("DSB");                                                               \
    __asm volatile("ISB");                                                               \
} while(0)

/**
 *****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 *****************************************************************************************
 */
#define GLOBAL_INT_STOP()                                                                \
do {                                                                                     \
    __asm volatile("CPSID i");                                                           \
    *(uint32_t *)0xE000E100 = 0;                                                         \
} while(0)

/**
 *****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 *****************************************************************************************
 */
#define GLOBAL_INT_DISABLE()                                                             \
do {                                                                                     \
    uint32_t __l_tmp;                                                                    \
    uint32_t __l_irq_rest;                                                               \
    __asm volatile("MRS %0, primask" : "=r"(__l_irq_rest));                              \
    __asm volatile("MOV %0, #1" : "=r"(__l_tmp));                                        \
    __asm volatile("MSR primask, %0" : : "r"(__l_tmp));                                  \
    CRITICAL_NESTING_INC();

/**
 *****************************************************************************************
 * @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 *****************************************************************************************
 */
#define GLOBAL_INT_RESTORE()                                                             \
    CRITICAL_NESTING_DEC();                                                              \
    __asm volatile("MSR primask, %0" : : "r"(__l_irq_rest));                             \
    __asm volatile("ISB");                                                               \
} while(0)

/**
 *****************************************************************************************
 * @brief Force a memory barrier to be inserted
 *
 *****************************************************************************************
 */
#define BARRIER()  __asm volatile("DMB")

/**
 *****************************************************************************************
 * @brief Invoke the wait for interrupt procedure of the processor.
 * In our case, only the core of this function is implemented.  In case of later need,
 * this example is a good reference.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *****************************************************************************************
 */
#define WFI()                                                                            \
do {                                                                                     \
    __asm volatile("DSB");                                                               \
    __asm volatile("WFI");                                                               \
    __asm volatile("ISB");                                                               \
} while(0)

#endif // LL_H_
