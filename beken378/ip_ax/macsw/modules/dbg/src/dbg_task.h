/**
 ****************************************************************************************
 *
 * @file dbg_task.h
 *
 * @brief File containing the definitions related to debug task management.
 *
 * File containing the definition of all the debug messages required by the debug task.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _DBG_TASK_H_
#define _DBG_TASK_H_

/**
 ****************************************************************************************
 * @defgroup DBG_TASK DBG_TASK
 * @ingroup DEBUG
 * @brief Debug Task management module
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"

// for ke_state_handler
#include "ke_task.h"

// for mac address
#include "mac.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Messages related to Debug Task
enum dbg_msg_tag
{
    /// Memory read request
    DBG_MEM_READ_REQ = KE_FIRST_MSG(TASK_DBG),
    /// Memory read confirm
    DBG_MEM_READ_CFM,
    /// Memory write request
    DBG_MEM_WRITE_REQ,
    /// Memory write confirm
    DBG_MEM_WRITE_CFM,
    /// Module filter request
    DBG_SET_MOD_FILTER_REQ,
    /// Module filter confirm
    DBG_SET_MOD_FILTER_CFM,
    /// Severity filter request
    DBG_SET_SEV_FILTER_REQ,
    /// Severity filter confirm
    DBG_SET_SEV_FILTER_CFM,
    /// Fatal error indication
    DBG_ERROR_IND,
    /// Request to get system statistics
    DBG_GET_SYS_STAT_REQ,
    /// COnfirmation of system statistics
    DBG_GET_SYS_STAT_CFM,


    /*
     * Section of internal DBG messages. No DBG API messages should be defined below this point
     */
    /// Timer allowing resetting the system statistics periodically to avoid
    /// wrap around of timer
    DBG_SYS_STAT_TIMER,
};

/// Structure containing the parameters of the @ref DBG_MEM_READ_REQ message.
struct dbg_mem_read_req
{
    /// Address to be read
    uint32_t memaddr;
};

/// Structure containing the parameters of the @ref DBG_MEM_READ_CFM message.
struct dbg_mem_read_cfm
{
    /// Address that was read
    uint32_t memaddr;
    /// Data that was read
    uint32_t memdata;
};

/// Structure containing the parameters of the @ref DBG_MEM_WRITE_REQ message.
struct dbg_mem_write_req
{
    /// Address to be written
    uint32_t memaddr;
    /// Data to be written
    uint32_t memdata;
};

/// Structure containing the parameters of the @ref DBG_MEM_WRITE_CFM message.
struct dbg_mem_write_cfm
{
    /// Address that was written
    uint32_t memaddr;
    /// Data that was written
    uint32_t memdata;
};

/// Structure containing the parameters of the @ref DBG_SET_MOD_FILTER_REQ message.
struct dbg_set_mod_filter_req
{
    /// Bit field indicating for each module if the traces are enabled or not
    uint32_t mod_filter;
};

/// Structure containing the parameters of the @ref DBG_SET_MOD_FILTER_REQ message.
struct dbg_set_sev_filter_req
{
    /// Bit field indicating the severity threshold for the traces
    uint32_t sev_filter;
};

/// Structure containing the parameters of the @ref DBG_GET_SYS_STAT_CFM message.
struct dbg_get_sys_stat_cfm
{
    /// Time spent in CPU sleep since last reset of the system statistics
    uint32_t cpu_sleep_time;
    /// Time spent in DOZE since last reset of the system statistics
    uint32_t doze_time;
    /// Total time spent since last reset of the system statistics
    uint32_t stats_time;
};

/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
extern const struct ke_state_handler dbg_default_handler;

/// @}  // end of group TASK_DBG

#endif // _DBG_TASK_H_
