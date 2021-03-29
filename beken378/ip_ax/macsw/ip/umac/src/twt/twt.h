/**
****************************************************************************************
*
* @file twt.h
*
* Copyright (C) RivieraWaves 2011-2020
*
* @brief Declaration of the TWT module.
*
****************************************************************************************
*/
#ifndef _TWT_H_
#define _TWT_H_

#include "rxu_task.h"
#include "sta_mgmt.h"
#include "twt_task.h"

/** @defgroup TWT TWT
 * @ingroup PS
 * @brief Declaration of all structures and functions used by the TWT module.
 * @{
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// TWT Default channel
#define TWT_CHANNEL_DEFAULT                 0
/// Conversion factor between raw minimum wake duration value and us
#define TWT_MIN_WAKE_DUR_RAW2US             256
/// TWT Implicit default value (0=Explicit, 1=Implicit)
#define TWT_IMPLICIT_DEFAULT                1
/// TWT NDP Paging default value
#define TWT_NDP_PAG_DEFAULT                 0
/// TWT Responder PM Mode default value
#define TWT_RESP_PM_MODE_DEFAULT            0

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/// TWT Flow type
enum twt_flow_type
{
    /// Announced TWT (with PS-Poll)
    TWT_ANNOUNCED,
    /// Unannounced TWT (without PS-Poll)
    TWT_UNANNOUNCED
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
///TWT Group assignment
struct twt_grp_assignment_tag
{
    /// TWT Group assignment byte array
    uint8_t grp_assignment[9];
};

/// Definition of status and properties of TWT Flow
enum twt_ctrl
{
   /// TWT Flow is valid
   TWT_CTRL_VALID = CO_BIT(0),
   /// Information Frames are disabled for the TWT Flow
   TWT_CTRL_INFO_FRM_DIS = CO_BIT(1),
   /// Unit of measurement of TWT Minimum Wake Duration (0:256us, 1:tu)
   TWT_CTRL_WAKE_DUR_UNIT = CO_BIT(2),
   /// TWT Flow is Active
   TWT_CTRL_ACTIVE = CO_BIT(3),
   /// TWT Flow is trigger based
   TWT_CTRL_TRIGGER = CO_BIT(4),
   /// TWT Flow is implicit
   TWT_CTRL_IMPLICIT = CO_BIT(5),
   /// TWT Flow is suspended
   TWT_CTRL_SUSPENDED = CO_BIT(6)
};

///TWT Flow properties structure
struct twt_flow_tag
{
    /// Bitfield indicating status and properties of TWT Flow
    uint16_t control;
    /// TWT Flow Identifier
    uint8_t id;
    /// TWT setup configuration
    struct twt_conf_tag conf;
    /// Wake duration in microseconds
    uint32_t wake_dur;
    /// TWT Wake Interval in microseconds
    uint32_t wake_int;
    /// Peer's TSF for next SP
    uint64_t next_sp_start;
    /// TWT Group Assignment
    struct twt_grp_assignment_tag grp_assignment;
    /// TWT Channel
    uint8_t channel;
    /// NDP Paging
    uint8_t ndp_paging[4];
    /// STA idx
    uint8_t sta_idx;
    /// Vif idx
    uint8_t vif_idx;
    /// Drift
    uint32_t drift;
    /// TWT SP timer structure
    struct mm_timer_tag sp_timer;
};

/// TWT module environment
struct twt_env_tag
{
    /// Total number of flows
    uint8_t flows;
    /// Number of TWT Flows in active mode
    uint8_t active_flows;
    /// Indicates for every VIF if PS-POLL/U-APSD has to be sent during the SP (Announced mode)
    uint8_t ann_wake_up_ctrl_send;
};

/// TWT Environment Information
extern struct twt_env_tag twt_env;
/// TWT Flow Information
extern struct twt_flow_tag twt_flow_tab[NX_TWT_FLOW_NB];

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize the TWT flows
 ****************************************************************************************
 */
void twt_init(void);

/**
 ****************************************************************************************
 * @brief Stop active TWT if there is no HE TB ongoing and TWT is active
 *
 * @param[in] twt_flow              Information about TWT flow
****************************************************************************************
 */
void twt_sp_end(struct twt_flow_tag *twt_flow);

/**
 ****************************************************************************************
 * @brief Delete TWT flow
 *
 * @param[in] twt_flow              Information about TWT flow
****************************************************************************************
 */
void twt_delete_flow(struct twt_flow_tag *twt_flow);

/**
 ****************************************************************************************
 * @brief Get TWT Flow pointer associated to a certain vif and station having the ID
 * passed as parameter.
 *
 * @param[in] sta_idx       Index of the STA associated with the flows
 * @param[in] vif_idx       Index of the VIF associated with the flow
 * @param[in] id            TWT flow ID
 *
 * @return information about the TWT Flow
****************************************************************************************
 */
struct twt_flow_tag *twt_get_flow(uint8_t sta_idx, uint8_t vif_idx, uint8_t id);

/**
 ****************************************************************************************
 * @brief Get TWT flows associated to a certain vif and station passed as parameter and
 * return the size of the buffer
 *
 * @param[out] flow_buf     Buffer to fill with matched TWT flows
 * @param[in] sta_idx       Index of the STA associated with the flows
 * @param[in] vif_idx       Index of the VIF associated with the flows
 *
 * @return size of filled buffer
****************************************************************************************
 */
uint8_t twt_get_flows(struct twt_flow_tag **flow_buf, uint8_t sta_idx, uint8_t vif_idx);

/**
 ****************************************************************************************
 * @brief Handle reception of S1G Unprotected Action frame (Setup, Information or
 * Teardown frame).
 *
 * @param[in] param         Pointer to the parameters of the message.
****************************************************************************************
 */
void twt_handle_action_frame(struct rxu_mgt_ind const *param);

/**
 ****************************************************************************************
 * @brief Generate a Unprotected S1G TWT Setup Action frame (REQUEST, SUGGEST, DEMAND,
 * GROUPING, ACCEPT, ALTERNATE, DICTATE, REJECT) and push it to the TX path for
 * transmission to the peer station
 *
 * @param[in] vif           Information about VIF on which frame has to be sent.
 * @param[in] sta           Information about STA to which frame will be sent.
 * @param[in] twt_flow      Information about TWT flow
 * @param[in] type          TWT setup type
 *
 * @return true if frame has been properly built and sent, false otherwise
 ****************************************************************************************
 */
bool twt_send_setup_frame(struct vif_info_tag *vif,
                          struct sta_info_tag *sta,
                          struct twt_flow_tag *twt_flow,
                          uint8_t type);

/**
 ****************************************************************************************
 * @brief Generate a TWT Teardown frame and push it to the TX path for transmission
 * to the peer station
 *
 * @param[in] vif_idx       Information about VIF to which frame will be sent.
 * @param[in] sta_idx       Information about STA on which frame has to be sent.
 * @param[in] teardown      TWT teardown message to transmit
 *
 * @return true if frame has been properly built and sent, false otherwise
 ****************************************************************************************
 */
bool twt_send_teardown_frame(uint8_t vif_idx, uint8_t sta_idx, uint8_t teardown);

/**
 ****************************************************************************************
 * @brief Check received frame for terminating TWT SP
 *
 * @param[in] vif               VIF pointer
 * @param[in] framectrl         Frame Control field
 * @param[in] machdr            Pointer to the MAC Header
 ****************************************************************************************
 */
void twt_check_end_of_sp(struct vif_info_tag *vif, uint16_t framectrl, struct mac_hdr *machdr);
/// @}

#endif // _TWT_TASK_H_
