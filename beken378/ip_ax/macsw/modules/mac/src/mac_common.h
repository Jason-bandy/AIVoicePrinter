/**
 ****************************************************************************************
 *
 * @file mac_common.h
 *
 * @brief MAC SW common definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MAC_COMMON_H_
#define _MAC_COMMON_H_

/**
 ****************************************************************************************
 * @defgroup CO_MAC_COMMON CO_MAC_COMMON
 * @ingroup CO_MAC
 * @brief  Common defines,structures
 *
 * This module contains defines commonly used for MAC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"

// for target dependent configurations
#include "rwnx_config.h"

#include "scanu.h"

/*
 * DEFINES
 ****************************************************************************************
 */
///Maximum number of scan results that can be stored.
#ifndef MAX_BSS_LIST
#define MAX_BSS_LIST            SCANU_MAX_RESULTS
#endif



/// @}

#endif // _MAC_COMMON_H_
