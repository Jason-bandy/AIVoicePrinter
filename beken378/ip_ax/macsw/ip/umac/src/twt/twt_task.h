/**
****************************************************************************************
*
* @file twt_task.h
*
* Copyright (C) RivieraWaves 2011-2020
*
* @brief Declaration of the TWT state machine.
*
****************************************************************************************
*/
#ifndef _TWT_TASK_H_
#define _TWT_TASK_H_

#include "ke_task.h"

/** @defgroup TASK_TWT TASK_TWT
 * @ingroup TWT
 * @brief Declaration of all structures and functions used by the TWT task.
 * @{
 */

/*
 * DEFINES
 ****************************************************************************************
 */
/// Number of TWT Task instances
#define TWT_IDX_MAX        (1)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/// Message API of the TWT task
enum twt_msg_tag
{
    /// Request Individual TWT Establishment
    TWT_SETUP_REQ = KE_FIRST_MSG(TASK_TWT),
    /// Confirm Individual TWT Establishment
    TWT_SETUP_CFM,
    /// Indicate TWT Setup response from peer
    TWT_SETUP_IND,
    /// Request to destroy a TWT Establishment or all of them
    TWT_TEARDOWN_REQ,
    /// Confirm to destroy a TWT Establishment or all of them
    TWT_TEARDOWN_CFM,

    /// MAX number of messages
    TWT_MAX,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
///TWT Flow configuration
struct twt_conf_tag
{
    /// Flow Type (0: Announced, 1: Unannounced)
    uint8_t flow_type;
    /// Wake interval Exponent
    uint8_t wake_int_exp;
    /// Unit of measurement of TWT Minimum Wake Duration (0:256us, 1:tu)
    bool wake_dur_unit;
    /// Nominal Minimum TWT Wake Duration
    uint8_t min_twt_wake_dur;
    /// TWT Wake Interval Mantissa
    uint16_t wake_int_mantissa;
};

///TWT Setup request message structure
struct twt_setup_req
{
    /// VIF Index
    uint8_t vif_idx;
    /// Setup request type, enum twt_setup_commands
    uint8_t setup_type;
    /// TWT Setup configuration
    struct twt_conf_tag conf;
};

///TWT Setup confirmation message structure
struct twt_setup_cfm
{
    /// Status (0 = TWT Setup Request has been transmitted to peer)
    uint8_t status;
};

/// TWT Teardown request message structure
struct twt_teardown_req
{
    /// TWT Negotiation type
    uint8_t neg_type;
    /// All TWT
    uint8_t all_twt;
    /// TWT flow ID
    uint8_t id;
    /// VIF Index
    uint8_t vif_idx;
};

///TWT Teardown confirmation message structure
struct twt_teardown_cfm
{
    /// Status (0 = TWT Teardown Request has been transmitted to peer)
    uint8_t status;
};

///TWT Setup indication message structure
struct twt_setup_ind
{
    /// Response type
    uint8_t resp_type;
    /// STA Index
    uint8_t sta_idx;
    /// TWT Setup configuration
    struct twt_conf_tag conf;
};

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */
/// Default state handler of the TWT task.
extern const struct ke_state_handler twt_default_handler;
/// @}

#endif // _TWT_TASK_H_
