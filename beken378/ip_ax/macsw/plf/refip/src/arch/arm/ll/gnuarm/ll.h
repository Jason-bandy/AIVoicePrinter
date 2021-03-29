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
    uint32_t __l_cpsr_tmp;                                                               \
    __asm volatile("MRS %0, CPSR" : "=r"(__l_cpsr_tmp));                                 \
    __asm volatile("BIC %0, %1, #0x40" : "=r"(__l_cpsr_tmp) :                            \
                 "r"(__l_cpsr_tmp));                                                     \
    __asm volatile("MSR CPSR_cxsf, %0" : : "r"(__l_cpsr_tmp));                           \
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
    uint32_t __l_cpsr_tmp;                                                               \
    __asm volatile("MRS %0, CPSR" : "=r"(__l_cpsr_tmp));                                 \
    __asm volatile("ORR %0, %1, #0x40" : "=r"(__l_cpsr_tmp) :                            \
                 "r"(__l_cpsr_tmp));                                                     \
    __asm volatile("MSR CPSR_cxsf, %0" : : "r"(__l_cpsr_tmp));                           \
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
    uint32_t __l_cpsr_tmp;                                                               \
    uint32_t __l_irq_rest;                                                               \
    __asm volatile("MRS %0, CPSR" : "=r"(__l_cpsr_tmp));                                 \
    __asm volatile("AND %0, %1, #0x40" : "=r"(__l_irq_rest) :                            \
                 "r"(__l_cpsr_tmp));                                                     \
    __asm volatile("ORR %0, %1, #0x40" : "=r"(__l_cpsr_tmp) :                            \
                 "r"(__l_cpsr_tmp));                                                     \
    __asm volatile("MSR CPSR_cxsf, %0" : : "r"(__l_cpsr_tmp));                           \
    CRITICAL_NESTING_INC();

/**
 *****************************************************************************************
 * @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 *****************************************************************************************
 */
#define GLOBAL_INT_RESTORE()                                                             \
    CRITICAL_NESTING_DEC();                                                              \
    __asm volatile("MRS %0, CPSR" : "=r"(__l_cpsr_tmp));                                 \
    __asm volatile("BIC %0, %1, #0x40" : "=r"(__l_cpsr_tmp) :                            \
                 "r"(__l_cpsr_tmp));                                                     \
    __asm volatile("ORR %0, %1, %2" : "=r"(__l_cpsr_tmp) :                               \
                 "r"(__l_cpsr_tmp), "r"(__l_irq_rest));                                  \
    __asm volatile("MSR CPSR_cxsf, %0" : : "r"(__l_cpsr_tmp));                           \
} while(0)

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
 * The following is the code extract from the Linux ARM926EJ-S support file:
 * @code
 *  ENTRY(cpu_arm926_do_idle)
 *      mov r0, #0
 *      mrc p15, 0, r1, c1, c0, 0   @ Read control register
 *      mcr p15, 0, r0, c7, c10, 4  @ Drain write buffer
 *      bic r2, r1, #1 << 12
 *      mrs r3, cpsr                @ Disable FIQs while Icache
 *      orr ip, r3, #PSR_F_BIT      @ is disabled
 *      msr cpsr_c, ip
 *      mcr p15, 0, r2, c1, c0, 0   @ Disable I cache
 *      mcr p15, 0, r0, c7, c0, 4   @ Wait for interrupt
 *      mcr p15, 0, r1, c1, c0, 0   @ Restore ICache enable
 *      msr cpsr_c, r3              @ Restore FIQ state
 *      mov pc, lr
 * @endcode
 * In our case, only the core of this function is implemented.  In case of later need,
 * this example is a good reference.
 *
 * @warning It is suggested that this macro is called while the interrupts are disabled
 * to have performed the checks necessary to decide to move to sleep mode.
 *****************************************************************************************
 */
#define WFI()                                                                            \
do {                                                                                     \
    uint32_t __l_rd;                                                                     \
    __asm volatile("MOV %0, #0" : "=r"(__l_rd));                                         \
    __asm volatile("MCR p15, 0, %0, c7, c0, 4" : "=r"(__l_rd));                          \
} while(0)

#endif // LL_H_
