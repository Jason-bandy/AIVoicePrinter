/**
 ****************************************************************************************
 *
 * @file interrupt.h
 *
 * @brief This file contains the definitions of the Cortus interrupt handler.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/**
 ****************************************************************************************
 * @defgroup BOOT
 * @ingroup DRIVERS
 * @brief Definition of Cortus interrupt controller values.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE
 ****************************************************************************************
 */
// standard integer functions
#include "co_int.h"
#include <machine/ic.h>

/*
 * DEFINES
 ****************************************************************************************
 */
/// Debug trap interrupt index
#define IC_DEBUG_STOP_IRQ  4
/// WiFi interrupt index
#define IC_WIFI_IRQ        5
#ifdef CFG_RTOS
/// Tick interrupt index
#define IC_TICK_IRQ        6
/// RTOS yield interrupt index
#define IC_YIELD_IRQ       7
#endif

///Interrupt enable/disable
enum ic_stat
{
    IC_DISABLE = 0,
    IC_ENABLE  = 1
};

///Enumeration of interrupt priority levels
enum ic_priority
{
    IC_PRIO_LOW = 0,
    IC_PRIO_HIGH = 1
};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Enable debugger IRQ.
 * This enables interrupt for debugger. Debugger IRQ is enabled here as there is no specific
 * driver managing debugger on APS platform.
 ****************************************************************************************
 */
void ic_enable_debug_irq(void);

/**
 ****************************************************************************************
 * @brief Read global interrupt status.
 * This function reads global interrupt status from interrupt controller register.
 * @return    status     Global interrupt status
 ****************************************************************************************
 */
enum ic_stat ic_get_global_interrupt_status(void);

/**
 ****************************************************************************************
 * @brief Configure interrupt priority.
 * This function configures IRQ priority in interrupt controller.
 *
 * @param[in]  irq_number    Interrupt source selection.
 * @param[in]  irq_prio      Priority level to set.
 ****************************************************************************************
 */
void ic_set_interrupt_priority(uint8_t irq_number, enum ic_priority irq_prio);

/**
 ****************************************************************************************
 * @brief Enable an interrupt.
 * This function enables IRQ in interrupt controller.
 *
 * @param[in]  irq_number    Interrupt source selection.
 * @param[in]  irq_stat      Enable/disable selected interrupt source.
 ****************************************************************************************
 */
void ic_set_interrupt_enable(uint8_t irq_number, enum ic_stat irq_stat);


/// @} BOOT
#endif // _INTERRUPT_H_
