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

#ifndef __arm__
#error "File only included with RVDS!"
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
#define GLOBAL_INT_START() __enable_fiq()

/**
 *****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 *****************************************************************************************
 */
#define GLOBAL_INT_STOP() __disable_fiq()

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
    int _fiq_was_masked = __disable_fiq();                                               \
    CRITICAL_NESTING_INC();

/**
 *****************************************************************************************
 * @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 *****************************************************************************************
 */
#define GLOBAL_INT_RESTORE()                                                             \
    CRITICAL_NESTING_DEC();                                                              \
    if (!_fiq_was_masked)                                                                \
        __enable_fiq();                                                                  \
} while(0)

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
#if defined (__TARGET_CPU_ARM926EJ_S)
#define WFI()                                                                            \
do {                                                                                     \
    uint32_t __l_rd;                                                                     \
    __asm("                                                                              \
        MOV     __l_rd, #0;                                                              \
        MCR p15, 0, __l_rd, c7, c0, 4;                                                   \
    ");                                                                                  \
} while (0)
#elif defined (__TARGET_CPU_ARM7TDMI_S)
#include "reg_intc.h"
#define WFI()                                                                            \
do {                                                                                     \
    GLOBAL_INT_DISABLE();                                                                \
    while(intc_fiq_status_get() == 0);                                                   \
    GLOBAL_INT_RESTORE();                                                                \
} while (0)
#else
#error "No target CPU defined"
#endif
#endif // LL_H_
