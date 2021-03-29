/**
 ****************************************************************************************
 *
 * @file ll.h
 *
 * @brief Declaration of low level functions.
 *
 * Copyright (C) RivieraWaves 2017-2019
 *
 ****************************************************************************************
 */

#ifndef LL_H_
#define LL_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <asm-dsp.h>
#include "critical_nesting.h"

/*
 * MACROS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 ****************************************************************************************
 */
#define GLOBAL_INT_START()                                                             \
do                                                                                     \
{                                                                                      \
    /* Enable global maskable interrupt bit */                                         \
    __asm__ volatile ("eint");                                                         \
                                                                                       \
    /* Add some nops */                                                                \
    __asm__ volatile ("nop");                                                          \
    __asm__ volatile ("nop");                                                          \
                                                                                       \
    /* Store mod1 */                                                                   \
    _dsp_asm("setp {imask} #0x01");                                                    \
} while(0)

/**
 ****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 ****************************************************************************************
 */
#define GLOBAL_INT_STOP() _dsp_asm("dint")

/**
 ****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 ****************************************************************************************
 */
#define GLOBAL_INT_DISABLE()                                                           \
do {                                                                                   \
    unsigned long __l_irq_rest;                                                        \
    __asm__ volatile("nop");                                                           \
    __asm__ volatile("nop");                                                           \
    /* Load mod1 register */                                                           \
    /* Nop has to be added in the __asm__ instruction in order to pass dependency      \
       checking Adding _dsp_asm("nop"); right after this instruction leads to an error \
       in the compilation */                                                           \
    __asm__ volatile ("mov modA.ui, %0.ui\n\t"                                         \
                      "nop #0x2" : "=&r"(__l_irq_rest):);                              \
    /* Disable interrupts */                                                           \
    __asm__ volatile("dint");                                                          \
    __asm__ volatile("nop");                                                           \
    __asm__ volatile("nop");                                                           \
                                                                                       \
    /* Increment the critical section nesting level */                                 \
    CRITICAL_NESTING_INC();


    /**
 ****************************************************************************************
 * @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 ****************************************************************************************
 */
#define GLOBAL_INT_RESTORE()                                                           \
    /* Decrement the critical section nesting level */                                 \
    CRITICAL_NESTING_DEC();                                                            \
                                                                                       \
    if ((__l_irq_rest & 0x00000010) == 0x00000010)                                     \
    {                                                                                  \
        __asm__ volatile("eint");                                                      \
        __asm__ volatile("nop");                                                       \
        __asm__ volatile("nop");                                                       \
    }                                                                                  \
} while(0)

/**
 ****************************************************************************************
 * @brief Force a memory barrier to be inserted
 *
 ****************************************************************************************
 */
#define BARRIER()  __asm__ volatile("" : : : "memory");

void rwnxl_wakeup(void);

#include "reg_intc.h"
/**
 ****************************************************************************************
 * @brief Invoke the wait for interrupt procedure of the processor.
 ****************************************************************************************
 */
#define WFI()                                                                          \
do {                                                                                   \
    DBG_CPU_SLEEP_START();                                                             \
    while(!(intc_irq_status_get(0) || intc_irq_status_get(1)));                        \
    DBG_CPU_SLEEP_END();                                                               \
} while (0)


#endif // LL_H_
