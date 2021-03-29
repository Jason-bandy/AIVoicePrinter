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

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include <asm-dsp.h>
#include "critical_nesting.h"

/*
 * MACROS
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 * @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 *****************************************************************************************
 */
#define GLOBAL_INT_START()                                                               \
do                                                                                       \
{                                                                                        \
    long __l_mod1;                                                                       \
                                                                                         \
    /* Load mod1 register */                                                             \
    __asm__ volatile ("mov mod1, %0" :"=&dhjlx" (__l_mod1):);                            \
                                                                                         \
    /* Set IM0 bit */                                                                    \
    __l_mod1 |= 0x00000002;                                                              \
                                                                                         \
    /* Add some nops */                                                                  \
    __asm__ volatile ("nop 2");                                                          \
                                                                                         \
    /* Store mod1 */                                                                     \
    __asm__ volatile ("mov %0, mod1" :: "zdlx" (__l_mod1));                              \
                                                                                         \
    /* Add some nops */                                                                  \
    __asm__ volatile ("nop 2");                                                          \
                                                                                         \
    /* Enable global maskable interrupt bit */                                           \
    __asm__ volatile ("eint");                                                           \
                                                                                         \
    /* Add some nops */                                                                  \
    __asm__ volatile ("nop 2");                                                          \
} while(0)

/**
 *****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 *****************************************************************************************
 */
#define GLOBAL_INT_STOP() _dsp_asm("dint")

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
    long __l_irq_rest;                                                                   \
    long __l_temp;                                                                       \
                                                                                         \
    /* Load mod1 register */                                                             \
    __asm__ volatile ("mov mod1, %0" :"=&dhjlx" (__l_temp):);                            \
                                                                                         \
    /* Store mod1 in local variable for later restoring */                               \
    __l_irq_rest = __l_temp;                                                             \
                                                                                         \
    /* Clear IE bit */                                                                   \
    __l_temp &= 0xFFFFFFFE;                                                              \
                                                                                         \
    /* Add some nops */                                                                  \
    __asm__ volatile ("nop 2");                                                          \
                                                                                         \
    /* Store mod1 */                                                                     \
    __asm__ volatile ("mov %0, mod1" :: "zdlx" (__l_temp));                              \
                                                                                         \
    /* Increment the critical section nesting level */                                   \
    CRITICAL_NESTING_INC();

/**
 *****************************************************************************************
 * @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 *****************************************************************************************
 */
#define GLOBAL_INT_RESTORE()                                                             \
    /* Decrement the critical section nesting level */                                   \
    CRITICAL_NESTING_DEC();                                                              \
                                                                                         \
    /* Restore mod1 to its previous value */                                             \
    __asm__ volatile ("mov %0, mod1" :: "zdlx" (__l_irq_rest));                          \
} while(0)

/**
 *****************************************************************************************
 * @brief Force a memory barrier to be inserted
 *
 *****************************************************************************************
 */
#define BARRIER()  __asm__ volatile("" : : : "memory");

void rwnxl_wakeup(void);

#include "reg_intc.h"
/**
 *****************************************************************************************
 * @brief Invoke the wait for interrupt procedure of the processor.
 *****************************************************************************************
 */
#define WFI()                                                                            \
do {                                                                                     \
    DBG_CPU_SLEEP_START();                                                               \
    while(!(intc_irq_status_get(0) || intc_irq_status_get(1)));                          \
    DBG_CPU_SLEEP_END();                                                                 \
} while (0)

/**
 *****************************************************************************************
 * @brief Return address to return to.
 *
 * Return address is saved in retreg register. However in "big" function compiler will
 * automatically add a call to _call_saved_store_xxx function at the beginning which will
 * override retreg register before this function can be called. In such cases previous
 * value of retreg register is available in r2 when returning form this function.
 * As such this function MUST be called at the very beginning (before r2 is overwritten
 * with something else).
 * It other cases (i.e. "small" functions) this function will return an undefined value.
 * Adding a call to trace function to print this value is enough to include a call to
 * _call_save_store_xxx so for now always trace r2.
 *
 * @return return r2
 *****************************************************************************************
 */
__INLINE uint32_t return_address(void)
{
    register uint32_t retadd asm("r2");
    return retadd;
}

#endif // LL_H_
