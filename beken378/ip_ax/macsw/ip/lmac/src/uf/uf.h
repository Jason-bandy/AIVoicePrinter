/**
 ****************************************************************************************
 * @file uf.h
 *
 * @brief Declarations of unsupported HT frame logger.
 *
 * Copyright (C) RivieraWaves 2011-2019
 ****************************************************************************************
 */

#ifndef _UF_H_
#define _UF_H_

/**
 ****************************************************************************************
 * @defgroup UF UF
 * @ingroup LMAC
 * @brief Management of unsupported frames received from MACBYPASS
 * @{
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"

// for co_list
#include "co_list.h"

// for co_ring
#include "co_ring.h"

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// Unsupported frame environment variable
extern struct uf_env_tag uf_env;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief This function initializes the unsupported frames manager
 ****************************************************************************************
 */
void uf_init(void);

/**
 ****************************************************************************************
 * @brief Indicate to the UF module that a unsupported frame has been captured by the MACBYPASS.
 *
 ****************************************************************************************
 */
void uf_event_ind(void);

/// @}

#endif /* _UF_H_ */
