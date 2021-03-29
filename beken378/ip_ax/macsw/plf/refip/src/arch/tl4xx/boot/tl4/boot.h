/**
 ****************************************************************************************
 *
 * @file boot.h
 *
 * @brief This file contains the declarations of the boot related variables.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _BOOT_H_
#define _BOOT_H_

// standard integer functions
#include "co_int.h"

/// linker variable containing start address of shared memory
extern uint32_t _sshram[];
/// linker variable containing end address of shared memory
extern uint32_t _eshram[];
#endif // _BOOT_H_
