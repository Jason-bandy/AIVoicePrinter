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
// standard integer functions
#include "co_int.h"
#include "interrupt.h"
#include "critical_nesting.h"

/**
 *****************************************************************************************
 * @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 *
 *****************************************************************************************
 */
#define GLOBAL_INT_START()                                                               \
    do {                                                                                 \
        __asm__ volatile                                                                 \
            ("    movhi   r7, #3  \n"                                                    \
             "    mov     psr, r7" : : : "r7", "memory");                                \
    } while(0)


/**
 *****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupts
 * it could handle.
 *
 *****************************************************************************************
 */

#define GLOBAL_INT_STOP()                                                                \
    do {                                                                                 \
        __asm__ volatile                                                                 \
            ("    movhi   r7, #2  \n"                                                    \
             "    mov     psr, r7" : : : "r7", "memory");                                \
    } while(0)


/**
 *****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 *
 *****************************************************************************************
 */

#define GLOBAL_INT_DISABLE()                                                             \
do {                                                                                     \
    uint32_t __l_irq_rest;                                                               \
    uint32_t __l_temp;                                                                   \
                                                                                         \
    /* Load psr */                                                                       \
    __asm volatile ("mov %0, psr" :"=r" (__l_temp)::);                                   \
                                                                                         \
    /* Store psr.pien, psr.ien in local variable */                                      \
    __l_irq_rest = __l_temp;                                                             \
                                                                                         \
    /* Clear psr.ien bit */                                                              \
    __l_temp &= 0xFFFEFFFF;                                                              \
                                                                                         \
    /* Store psr */                                                                      \
    __asm volatile ("mov   psr, %0" ::"r" (__l_temp):);                                  \
                                                                                         \
    /* Increment the critical section nesting level */                                   \
    CRITICAL_NESTING_INC();


/**
 *****************************************************************************************
 * @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 *
 *****************************************************************************************
 */

#define GLOBAL_INT_RESTORE()                                                             \
    /* Decrement the critical section nesting level */                                   \
    CRITICAL_NESTING_DEC();                                                              \
                                                                                         \
    /* Load pien,ien and store into psr */                                               \
    __asm volatile ("mov  psr, %0"::"r" (__l_irq_rest) );                                \
} while(0);


/**
 *****************************************************************************************
 * @brief Force a memory barrier to be inserted
 *
 *****************************************************************************************
 */

#define BARRIER()  __asm volatile("" : : : "memory");

/**
 *****************************************************************************************
 * @brief Invoke the wait for interrupt procedure of the processor.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *****************************************************************************************
 */
void rwnxl_wakeup(void);
#ifdef SUPPORT_PROCESSOR_IDLE
#define WFI()                                                                            \
    /* The stop instruction signals external hardware that it should stop     */         \
    /* the processor clock so that the processor can enter a low power mode.  */         \
    __asm volatile ("stop":::);
#else
#define WFI()                                                                            \
do {                                                                                     \
    DBG_CPU_SLEEP_START();                                                               \
    while(!ic_get_global_interrupt_status());                                            \
    DBG_CPU_SLEEP_END();                                                                 \
} while (0)
#endif // SUPPORT_PROCESSOR_IDLE


/**
 *****************************************************************************************
 * @brief Return address to return to.
 *
 * To get the correct return address this function must be called before the first
 * function call (if any), otherwise ra will be overwritten.
 *
 * @return return r15
 *****************************************************************************************
 */
static inline uint32_t return_address(void)
{
     register uint32_t retadd asm("r15");
     return retadd;
}

#endif // LL_H_
