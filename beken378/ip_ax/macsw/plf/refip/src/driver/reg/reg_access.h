/**
 ****************************************************************************************
 *
 * @file reg_access.h
 *
 * @brief File implementing the basic primitives for register accesses
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef REG_ACCESS_H_
#define REG_ACCESS_H_

/**
 ****************************************************************************************
 * @defgroup REG REG
 * @ingroup PLATFORM_DRIVERS
 *
 * @brief Basic primitives for register access.
 *
 * @{
 ****************************************************************************************
 */
#include "co_utils.h"

/*
 * MACROS
 ****************************************************************************************
 */
/// Macro to read a platform register
#define REG_PL_RD(addr)              (*(volatile uint32_t *)(HW2CPU(addr)))

/// Macro to write a platform register
#define REG_PL_WR(addr, value)       (*(volatile uint32_t *)(HW2CPU(addr))) = (value)



/// @} REG

#endif // REG_ACCESS_H_
