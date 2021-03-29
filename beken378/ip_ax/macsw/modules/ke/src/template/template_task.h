/**
 ****************************************************************************************
 *
 * @file template_task.h
 *
 * @brief Template task kernel interface declaration.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */
#ifndef _TEMPLATE_TASK_H_
#define _TEMPLATE_TASK_H_

/**
 ****************************************************************************************
 * @defgroup TASK_TEMPLATE TASK_TEMPLATE
 * @ingroup TEMPLATE
 * @brief Description of the task.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
// inclusion to retrieve the task index
#include "ke_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Task max index number.
#define TEMPLATE_IDX_MAX 1

/// Possible states of the task.
enum
{
    /// STATE 1 description.
    TEMPLATE_STATE1,
    /// State 2 description.
    TEMPLATE_STATE2,
    /// State 3 description.
    TEMPLATE_STATE3,
    /// Number of states.
    TEMPLATE_STATE_MAX
};

/// Messages that are logically related to the task.
enum
{
    /// MESSAGE1_REQ description.
    TEMPLATE_MESSAGE1_REQ = KE_FIRST_MSG(TASK_TEMPLATE),
    /// MESSAGE1_CFM description.
    TEMPLATE_MESSAGE1_CFM,
    /// EXAMPLE1_IND description.
    TEMPLATE_EXAMPLE1_IND,
    /// EXAMPLE2_IND description.
    TEMPLATE_EXAMPLE2_IND
};

/// MESSAGE1_REQ parameters structure description.
struct template_message1_req
{
    /// Parameter description.
    uint8_t parameter1;
    /// Parameter description.
    uint32_t parameter2;
};

/// MESSAGE1_CFM parameters structure description.
struct template_message1_cfm
{
    /// Parameter description.
    uint8_t parameter1;
    /// Parameter description.
    uint32_t parameter2;
};

/// EXAMPLE1_IND parameters structure description.
struct template_example1_ind
{
    /// Parameter description.
    uint8_t parameter1;
    /// Parameter description.
    uint32_t parameter2;
};

/// EXAMPLE2_IND parameters structure description.
struct template_example2_ind
{
    /// Parameter description.
    uint8_t parameter1;
    /// Parameter description.
    uint32_t parameter2;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern const struct ke_state_handler template_state_handler[TEMPLATE_STATE_MAX];

extern const struct ke_state_handler template_default_handler;

extern ke_state_t template_state[TEMPLATE_IDX_MAX];



/// @} end of group
#endif // _TEMPLATE_TASK_H_
