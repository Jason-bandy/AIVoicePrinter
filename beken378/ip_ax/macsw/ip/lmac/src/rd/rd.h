/**
 ****************************************************************************************
 * @file rxl_cntrl.h
 *
 * @brief Declarations of radar detection driver.
 *
 * Copyright (C) RivieraWaves 2011-2020
 ****************************************************************************************
 */

#ifndef _RD_H_
#define _RD_H_

/**
 ****************************************************************************************
 * @defgroup RD RD
 * @ingroup LMAC
 * @brief Management of the radar pulses received from PHY
 * @{
 *
 ****************************************************************************************
 */
#include "co_list.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Indicate to the RD module that a radar event has been detected by the PHY.
 *
 * @param[in] rd_idx Index of the radar detection chain on which pulses have been detected
 *
 ****************************************************************************************
 */
void rd_event_ind(int rd_idx);

/// @}

#endif // _RD_H_
