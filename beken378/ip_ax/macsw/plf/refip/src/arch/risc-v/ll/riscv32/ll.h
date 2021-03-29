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
#include "co_math.h"
#include "critical_nesting.h"

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#ifdef CFG_ZERORISCY
#define INTE_EN CO_BIT(3)
#else
#define INTE_EN CO_BIT(0)
#endif

/**
 ****************************************************************************************
 * @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 ****************************************************************************************
 */
#define GLOBAL_INT_START()                                                        \
do {                                                                              \
    int mstatus;                                                                  \
    __asm__ volatile ("csrrsi %0, mstatus, %1" : "=r" (mstatus) : "i" (INTE_EN)); \
} while(0)

/**
 ****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 ****************************************************************************************
 */
#define GLOBAL_INT_STOP()                                                         \
do {                                                                              \
    int mstatus;                                                                  \
    __asm__ volatile ("csrrci %0, mstatus, %1" : "=r" (mstatus) : "i" (INTE_EN)); \
} while(0)

/**
 ****************************************************************************************
 * @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since
 * this last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 ****************************************************************************************
 */
#define GLOBAL_INT_DISABLE()                                                       \
    do {                                                                           \
    int irq_rest;                                                                  \
    __asm__ volatile ("csrrci %0, mstatus, %1" : "=r" (irq_rest) : "i" (INTE_EN)); \
                                                                                   \
    /* Increment the critical section nesting level */                             \
    CRITICAL_NESTING_INC();


/**
 ****************************************************************************************
 * @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 ****************************************************************************************
 */
#define GLOBAL_INT_RESTORE()                                                      \
    /* Decrement the critical section nesting level */                            \
    CRITICAL_NESTING_DEC();                                                       \
    /* Restore mstatus to its previous value */                                   \
    __asm__ volatile ("csrw mstatus, %0" : /* no output */ : "r" (irq_rest));     \
} while(0);

/**
 ****************************************************************************************
 * @brief Force a memory barrier to be inserted
 *
 ****************************************************************************************
 */
#define BARRIER()  __asm volatile("" : : : "memory");

void rwnxl_wakeup(void);

#include "reg_intc.h"

#define WFI()                                                                            \
do {                                                                                     \
    DBG_CPU_SLEEP_START();                                                               \
    __asm__ volatile ("wfi":::);                                                           \
    DBG_CPU_SLEEP_END();                                                                 \
} while (0)

/**
 ****************************************************************************************
 * @brief Change the stack pointer
 ****************************************************************************************
 */
#define CHANGE_SP(new)                                                              \
    __asm__ volatile ("mv sp, %0" : : "r" ((uint32_t) new):);

/**
 *****************************************************************************************
 * @brief Return address to return to.
 *
 * To get the correct return address this function must be called before the first
 * function call (if any), otherwise ra will be overwritten.
 *
 * @return return ra (aka x1)
 *****************************************************************************************
 */
static inline uint32_t return_address(void)
{
     register uint32_t retadd asm("ra");
     return retadd;
}

/**
 *****************************************************************************************
 * @brief Put a break instruction
 *
 *****************************************************************************************
 */
static inline void ebreak(void)
{
    __asm__ volatile ("ebreak");
}

/**
 *****************************************************************************************
 * @brief Get mstatus
 *
 * @return value of mstatus register
 *****************************************************************************************
 */
static inline uint32_t MSTATUS(void)
{
    uint32_t mstatus;
    __asm__ volatile ("csrr %0, mstatus": "=r" (mstatus));
    return mstatus;
}

/**
 *****************************************************************************************
 * @brief Get mpec
 *
 * @return value of mpec register
 *****************************************************************************************
 */
static inline uint32_t MEPC(void)
{
    uint32_t mepc;
    __asm__ volatile ("csrr %0, mepc": "=r" (mepc));
    return mepc;
}

/**
 *****************************************************************************************
 * @brief Get stack pointer
 *
 * @return value of sp register
 *****************************************************************************************
 */
static inline uint32_t SP(void)
{
    register uint32_t sp asm("sp");
    return sp;
}


#endif // LL_H_
