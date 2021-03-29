/**
****************************************************************************************
*
* @file scanu_task.h
*
* Copyright (C) RivieraWaves 2011-2020
*
* @brief Declaration of all structures and functions used by the SCAN module.
*
****************************************************************************************
*/

#ifndef _SCANU_TASK_H_
#define _SCANU_TASK_H_

#include "co_int.h"
// inclusion to retrieve the task index
#include "ke_task.h"
// for mac_addr and mac_ssid
#include "mac.h"
#include "scan.h"

/** @defgroup TASK_SCANU TASK_SCANU
 * @ingroup SCANU
 * @brief Declaration of all structures and functions used by the SCAN module.
 * @{
 */

/// Task max index number.
#define SCANU_IDX_MAX 1

/// Possible states of the task.
enum
{
    /// Idle State.
    SCANU_IDLE,
    /// SCANNING State.
    SCANU_SCANNING,
    /// Max number of states
    SCANU_STATE_MAX
};

/// Messages that are logically related to the task.
enum
{
    /// Scan request from host.
    SCANU_START_REQ = KE_FIRST_MSG(TASK_SCANU),
    /// Scanning start Confirmation.
    SCANU_START_CFM,
    /// Join request
    SCANU_JOIN_REQ,
    /// Join confirmation.
    SCANU_JOIN_CFM,
    /// Scan result indication.
    SCANU_RESULT_IND,
    /// Get Scan result request.
    SCANU_GET_SCAN_RESULT_REQ,
    /// Scan result confirmation.
    SCANU_GET_SCAN_RESULT_CFM,
#if BK_MAC
    /// Fast scan request from any other module.
    SCANU_FAST_REQ,
    /// Confirmation of fast scan request.
    SCANU_FAST_CFM,
#endif
};

/// Structure containing the parameters of the @ref SCANU_START_REQ and
/// @ref SCANU_JOIN_REQ messages
struct scanu_start_req
{
    /// List of channel to be scanned
    struct mac_chan_def chan[SCAN_CHANNEL_MAX];
    /// List of SSIDs to be scanned
    struct mac_ssid ssid[SCAN_SSID_MAX];
    /// BSSID to be scanned (or WILDCARD BSSID if no BSSID is searched in particular)
    struct mac_addr bssid;
    /// Address (in host memory) of the additional IEs that need to be added to the ProbeReq
    /// (following the SSID element)
    uint32_t add_ies;
    /// Length of the additional IEs
    uint16_t add_ie_len;
    /// Index of the VIF that is scanning
    uint8_t vif_idx;
    /// Number of channels to scan
    uint8_t chan_cnt;
    /// Number of SSIDs to scan for
    uint8_t ssid_cnt;
    /// no CCK - For P2P frames not being sent at CCK rate in 2GHz band.
    bool no_cck;
    /// Scan duration, in us. If 0 use default values
    uint32_t duration;
};

/// Structure containing the parameters of the @ref SCANU_START_CFM and
/// @ref SCANU_JOIN_CFM messages
struct scanu_start_cfm
{
    /// Index of the VIF that was scanning
    uint8_t vif_idx;
    /// Status of the request
    uint8_t status;
    /// Number of scan results available
    uint8_t result_cnt;
};

/// Structure containing the parameters of the @ref SCANU_GET_SCAN_RESULT_REQ message
struct scanu_get_scan_result_req
{
    /// index of the scan element
    uint8_t idx;
};

/// Structure containing the parameters of the @ref SCANU_GET_SCAN_RESULT_CFM message
struct scanu_get_scan_result_cfm
{
    /// Structure for scan result element
    struct mac_scan_result scan_result;
};

#if BK_MAC
/// Structure containing the parameters of the message.
struct scanu_fast_req
{
    /// The SSID to scan in the channel.
    struct mac_ssid ssid;
    /// BSSID.
    struct mac_addr bssid;
    /// Probe delay.
    uint16_t probe_delay;
    /// Minimum channel time.
    uint16_t minch_time;
    /// Maximum channel time.
    uint16_t maxch_time;
    /// The channel number to scan.
    uint16_t ch_nbr;
};
#endif

extern const struct ke_state_handler scanu_state_handler[SCANU_STATE_MAX];

extern const struct ke_state_handler scanu_default_handler;

extern ke_state_t scanu_state[SCANU_IDX_MAX];

/// @}

#endif // _SCANU_TASK_H_


