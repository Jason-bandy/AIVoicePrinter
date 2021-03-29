/**
 ****************************************************************************************
 *
 * @file boot.h
 *
 * @brief This file contains the definitions used for boot code.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _BOOT_H_
#define _BOOT_H_

/**
 ****************************************************************************************
 * @defgroup BOOT
 * @ingroup DRIVERS
 * @brief Definition of boot code values.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE
 ****************************************************************************************
 */
// standard integer functions
#include "co_int.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Stack initialization pattern
#define STACK_INIT_PATTERN     0xF3F3F3F3

/*
 * LINKER VARIABLES
 ****************************************************************************************
 */

/// Low/high boundaries of data sections (from linker script)
extern uint32_t _bss[], _ebss[],
               __heap_bottom[],__heap_top[],__stack_bottom[],__stack_top[],
               _debugger_register_save_area[],
               _unloaded_area_start[], _unloaded_area_end[], _sshram[], _eshram[];


/// @} BOOT
#endif // _BOOT_H_
