/**
 ****************************************************************************************
 *
 * @file intc.h
 *
 * @brief Declaration of the Reference Interrupt Controller (INTC) API.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _INTC_H_
#define _INTC_H_

/**
 ****************************************************************************************
 * @defgroup INTC INTC
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the Reference Interrupt Controller API
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// for __IRQ and __FIQ
#include "compiler.h"

/**
 ****************************************************************************************
 * @defgroup INTC_MAPPING Mapping of the peripherals interrupts in the INTC.
 * @{
 ****************************************************************************************
 */
#define INTC_LLI15 (31)
#define INTC_LLI14 (30)
#define INTC_LLI13 (29)
#define INTC_LLI12 (28)
#define INTC_LLI11 (27)
#define INTC_LLI10 (26)
#define INTC_LLI9 (25)
#define INTC_LLI8 (24)
#define INTC_LLI7 (23)
#define INTC_LLI6 (22)
#define INTC_LLI5 (21)
#define INTC_LLI4 (20)
#define INTC_LLI3 (19)
#define INTC_LLI2 (18)
#define INTC_LLI1 (17)
#define INTC_LLI0 (16)
#define INTC_DMAEOT (15)
#define INTC_IPC3 (14)
#define INTC_IPC2 (13)
#define INTC_IPC1 (12)
#define INTC_IPC0 (11)
#define INTC_MACINTGEN (10)
#define INTC_MACPROT (9)
#define INTC_MACTX (8)
#define INTC_MACRX (7)
#define INTC_MACOTHER (6)
#define INTC_MACTIMER (5)
#define INTC_PHY (4)
#define INTC_UNUSED3 (3)
#define INTC_UNUSED2 (2)
#define INTC_UNUSED1 (1)
#define INTC_UNUSED0 (0)

/// @} INTC_MAPPING


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize and configure the reference INTCTRL.
 * This function configures the INTC according to the system needs.
 ****************************************************************************************
 */
void intc_init(void);

/// IRQ handler.
__IRQ void intc_irq(void);

/// FIQ handler.
__FIQ void intc_fiq(void);


/// @}

#endif // _INTC_H_
