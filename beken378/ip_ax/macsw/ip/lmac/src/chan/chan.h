/**
 ****************************************************************************************
 * @file chan.h
 *
 * @brief MAC Channel Management module declarations.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _CHAN_H_
#define _CHAN_H_

/**
 *****************************************************************************************
 * @defgroup CHAN CHAN
 * @ingroup LMAC
 * @brief LMAC MAC Management module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "co_int.h"
#include "co_bool.h"
#include "co_utils.h"
#include "mm_task.h"
#include "mm_timer.h"

#if (NX_CHNL_CTXT)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of channel contexts for traffic
#define CHAN_TRAF_CTXT_CNT (NX_CHAN_CTXT_CNT)
/// Number of channel context (+1 for Scan and +1 for the Remain on Channel)
#define CHAN_CHAN_CTXT_CNT     (CHAN_TRAF_CTXT_CNT + 1 + 1)
/// Index of the Scan Channel Context
#define CHAN_SCAN_CTXT_IDX     (CHAN_TRAF_CTXT_CNT)
/// Index of the Remain on Channel Context
#define CHAN_ROC_CTXT_IDX      (CHAN_SCAN_CTXT_IDX + 1)
/// Index of a Channel Context that is not linked with a VIF
#define CHAN_CTXT_UNUSED       (0xFF)


#if BK_MAC && defined(CFG_SCAN_DURATION)  // TBD: hm modified
#define CHAN_SWITCH_DELAY   0
#else
/// Delay between the start of a channel switch procedure and the time the new channel
/// should be active (in us)
#define CHAN_SWITCH_DELAY   (4000 + phy_get_channel_switch_dur())
#endif //BK_MAC

/// Maximum number of scheduled switch
#define CHAN_SWITCH_CNT  4

/// Minimal presence duration on a channel (in us)
#define CHAN_MIN_PRES_DUR_US (5000)

/// Delay before switching on a Scan or a ROC channel, indicate minimal duration between
/// two successive scan/roc operations
#if BK_MAC
#define CHAN_CONN_LESS_DELAY   (9000)

#define CHAN_MIN_CONN_LESS_DELAY   (2 * 1000)
#define CHAN_MIN_SCAN_TIME         (10 * 1000)
#else
#define CHAN_CONN_LESS_DELAY   (30000)
#endif

/// Flags indicating that a scan ROC is pending
#define CHAN_ROC_SCAN_PENDING_MASK  (CO_BIT(CHAN_ENV_ROC_BIT) | CO_BIT(CHAN_ENV_SCAN_BIT))

/// Drift threshold of STA/P2P-CLI TBTT to re-schedule NOA, in us
#define CHAN_P2P_NOA_RESCHEDULE_THRESHOLD 2000

/// Drift threshold of STA/P2P-CLI TBTT to re-sync P2P GO TBTT with NOA, in us
#define CHAN_P2P_NOA_RESYNC_THRESHOLD 200

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Channel Context State Machine
enum chan_ctxt_status
{
    /// Context has not been scheduled
    CHAN_NOT_SCHEDULED = 0,
    /// Context has been scheduled but is not been programmed
    CHAN_NOT_PROG,
    /// HW is going to IDLE state
    CHAN_GOTO_IDLE,
    /// Wait for Notification of Absence confirmation
    CHAN_WAIT_NOA_CFM,
    /// Waiting for channel duration timer end
    CHAN_WAITING_END,
    /// Present on channel but End of Channel timer not running
    CHAN_PRESENT,
    /// Context is sending a notification of absence
    CHAN_SENDING_NOA,
};

/// TBTT Switch State Machine
enum chan_tbtt_status
{
    /// TBTT Timer is programmed
    CHAN_TBTT_PROG = CO_BIT(0),
    /// TBTT was scheduled, and end of TBTT period is expected
    CHAN_TBTT_WAIT_TO = CO_BIT(1),
    /// TBTT was skipped by CHAN module
    CHAN_TBTT_SKIPPED = CO_BIT(2),
    /// Next TBTT was added at the start of the TBTT, so no need to add it at the
    // end of the TBTT period
    CHAN_TBTT_ALREADY_ADDED = CO_BIT(3),
    /// Need to start P2P NOA once this TBTT has been programmed
    CHAN_TBTT_SCHEDULE_P2P_NOA = CO_BIT(4),
    /// Indicate that p2p_noa_tsf_update field is valid
    CHAN_TBTT_P2P_NOA_TSF_UPDATE = CO_BIT(5),
    /// Indicate that P2P NOA must be resynced
    CHAN_TBTT_P2P_NOA_RESYNC = CO_BIT(6),
};

/// Channel Environment status bits (see chan_env.status)
enum chan_env_status_bit
{
    /// Remain on Channel operation is waiting
    CHAN_ENV_ROC_WAIT_BIT = 0,
    /// Scan Operation is waiting
    CHAN_ENV_SCAN_WAIT_BIT,
    /// Remain on Channel operation is in progress
    CHAN_ENV_ROC_BIT,
    /// Scan Operation is in progress
    CHAN_ENV_SCAN_BIT,
    /// Connection Less Delay Timer is programmed
    CHAN_ENV_DELAY_PROG_BIT,
    /// Beacon Detection is in progress
    CHAN_ENV_BCN_DETECT_BIT,
    /// Fix channel delayed
    CHAN_ENV_FIX_CTXT_PENDING_BIT,
};

/*
 * STRUCTURES/TYPES DEFINITION
 ****************************************************************************************
 */

/// Structure containing TBTT switch information
struct chan_tbtt_tag
{
    /// List header for chaining
    struct co_list_hdr list_hdr;

    /// TBTT switch time
    uint32_t time;
    /// VIF Index
    uint8_t vif_index;
    /// TBTT status (@see enum chan_tbtt_status)
    uint8_t status;

    #if (NX_P2P)
    /// Index of the P2P GO vif on which this TBTT induce a NOA
    /// (INVALID_VIF_IDX if this TBTT doesn't induce any NOA)
    uint8_t p2p_noa_vif_index;

    /// Index of the P2P NOA induced. Undefined if p2p_noa_vif_index == INVALID_VIF_IDX
    uint8_t p2p_noa_index;

    /// Contains offset applied to TSF when starting periodic NOA.
    /// Valid only for P2P GO vif if bit CHAN_TBTT_P2P_NOA_TSF_UPDATE is set in status
    int32_t p2p_noa_tsf_update;

    /// Contains time, in us, between the TBTT and the end of the induced NOA when the
    /// NOA has been started.
    /// Valid only if p2p_noa_vif_index != INVALID_VIF_IDX
    int32_t p2p_noa_tbtt_to_end;

    /// TBTT drift, in us, between the latest TBTT and when the NOA has been started.
    /// Valid only if p2p_noa_vif_index != INVALID_VIF_IDX
    int16_t p2p_noa_drift;

    #endif //(NX_P2P)
};

/// Structure containing scheduled channel switch information
struct chan_switch_tag
{
    /// List header for chaining
    struct co_list_hdr list_hdr;

    /// Channel switch time
    uint32_t time;

    /// Context target (may be NULL)
    struct chan_ctxt_tag *ctxt;

    /// TBTT that trigger this switch (may be NULL)
    struct chan_tbtt_tag *tbtt;
};

/// Structure describing a channel context
struct chan_ctxt_tag
{
    /// List header for chaining
    struct co_list_hdr list_hdr;

    /// Parameters of the channel
    struct mac_chan_op channel;

    /// TaskID of the task to send the switch confirmation to (TASK_NONE if no confirmation)
    ke_task_id_t taskid;

    /// Status (@see enum chan_ctxt_status)
    uint8_t status;

    /// Duration, in us, to stay on the context (only for connection-less contexts)
    uint32_t duration_us;

    /// Index of the channel context
    uint8_t idx;
    /// Number of VIF currently linked with the channel
    uint8_t nb_linked_vif;
    /// VIF Index - For Scan and ROC Contexts ONLY
    uint8_t vif_index;

    #if (NX_P2P)
    /// Index of the P2P GO interface active of this channel (INVALID_VIF_IDX if none)
    uint8_t p2pgo_vif_index;
    #endif //(NX_P2P_GO)

    #if (NX_TDLS)
    /// Flag indicating whether this channel context is a TDLS RoC one
    bool roc_tdls;
    #endif
};

/// LMAC MAC Management Context
struct chan_env_tag
{
    /// List of free channel contexts
    struct co_list list_free_ctxt;
    /// List of scheduled channel contexts
    struct co_list list_sched_ctxt;

    /// List of TBTT Switch information
    struct co_list list_tbtt;

    /// List of free channel switch
    struct co_list list_free_switch;
    /// List of scheduled channel switch
    struct co_list list_switch;

    /// Operational channel
    struct chan_ctxt_tag *current_ctxt;
    /// Ongoing channel switch
    struct chan_ctxt_tag *switch_ctxt;

    /// Scan/RoC Delay Timer
    struct mm_timer_tag tmr_conn_less;
    /// Channel switch timer
    struct mm_timer_tag tmr_switch;
#if BK_MAC
    /* for more probe request*/
    struct mm_timer_tag secondary_active_tmr;
#endif

    /// Channel context currently fixed (NULL if no channel fixed)
    struct chan_ctxt_tag *fix_ctxt;
    /// End of fix period
    uint32_t fix_until;

    /// Status (@see enum chan_env_status_bit)
    uint8_t status;
    /// Number of TX confirmations awaited before channel switching
    uint8_t cfm_cnt;

    /// Number of Channel Context currently scheduled
    uint8_t nb_sched_ctxt;

    /// Number of TBTT window active
    uint8_t nb_active_tbtt;

    #if (NX_POWERSAVE)
    /// Flag saving the value of the PM bit
    uint8_t pm;
    #endif //(NX_POWERSAVE)
};

// Forward declaration
struct vif_info_tag;


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern struct chan_env_tag chan_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief MM Channel Module initialization function.
 * This function is called after reset and initializes all MM Channel related env and data.
 ****************************************************************************************
 */
void chan_init(void);

#if (NX_HW_SCAN)
/**
 ****************************************************************************************
 * @brief Request to go to a specific channel for scanning.
 * This function takes care on scheduling the channel switch at an appropriate time. Upon
 * switching time, the MM channel will inform the AP(s) on which we are connected that we
 * are going to power-save so that they start buffering the traffic for us.
 *
 * @param[in] chan         Channel definition
 * @param[in] duration_us  Duration of the scan on the requested channel
 * @param[in] vif_index    Index of VIF on which scan will be performed
 ****************************************************************************************
 */
void chan_scan_req(const struct mac_chan_def *chan, uint32_t duration_us,
                   uint8_t vif_index);

#endif //(NX_HW_SCAN)

/**
 ****************************************************************************************
 * @brief Request to go to a specific channel for a given duration.
 ****************************************************************************************
 */
uint8_t chan_roc_req(struct mm_remain_on_channel_req const *req, ke_task_id_t taskid);

/**
 ****************************************************************************************
 * @brief Request to allocate a new channel context.
 *
 * @param[in]  chan_req  Pointer to the channel parameter structure
 * @param[out] idx       Index of the allocated channel context
 *
 * @return CO_OK if allocation was successful, CO_FAIL otherwise
 ****************************************************************************************
 */
uint8_t chan_ctxt_add(struct mac_chan_op const *chan_req, uint8_t *idx);

/**
 ****************************************************************************************
 * @brief Free a previously allocated channel context.
 *
 * @param[in] idx      Index of the channel context to be freed
 ****************************************************************************************
 */
void chan_ctxt_del(uint8_t idx);

/**
 ****************************************************************************************
 * @brief Link a channel context to a VIF.
 *
 * @param[in] vif_idx   Index of the VIF
 * @param[in] chan_idx  Index of the channel context to be linked
 ****************************************************************************************
 */
void chan_ctxt_link(uint8_t vif_idx, uint8_t chan_idx);

/**
 ****************************************************************************************
 * @brief Unlink the channel context from a VIF.
 *
 * @param[in] vif_idx   Index of the VIF
 ****************************************************************************************
 */
void chan_ctxt_unlink(uint8_t vif_idx);

/**
 ****************************************************************************************
 * @brief Request to update the parameters of a channel context.
 *
 * @param[in]  update  Pointer to the new parameters structure
 ****************************************************************************************
 */
void chan_ctxt_update(struct mm_chan_ctxt_update_req const *update);

/**
 ****************************************************************************************
 * @brief Request to schedule the switch to a channel context.
 *
 * @param[in]  sched_req    Pointer to the channel scheduling structure
 * @param[in]  taskid       Kernel task ID of the task the confirmation has to be sent to
 ****************************************************************************************
 */
void chan_ctxt_sched(struct mm_chan_ctxt_sched_req const *sched_req, ke_task_id_t taskid);

/**
 ****************************************************************************************
 * @brief Link a channel context to the monitor interface.
 * This function is called every time a channel context is linked to a data interface and
 * links the monitor interface to the same channel context of the data interface. If there
 * is no data interface, this function links the monitor interface to the provided channel
 * context and sets the monitor interface as active.
 *
 * @param[in] chan_idx  Index of the channel context to be linked
 ****************************************************************************************
 */
void chan_ctxt_link_monitor(uint8_t chan_idx);

/**
 ****************************************************************************************
 * @brief Indicates that TBTT starts and provides date of the next TBTT.
 *
 * It is called by MM module to indicate that TBTT starts. It also indicates expected date
 * of the next TBTT.
 * As the next TBTT date may be updated after beacon reception (@ref chan_tbtt_updated)
 * this function simply checks if this TBTT is expected (from CHAN module point of view).
 * and waits for @ref chan_bcn_to_evt to process it.
 *
 * @param[in] vif             VIF Entry
 * @param[in] tbtt_time       Time of current TBTT
 * @param[in] next_tbtt_time  Time of next TBTT
 *
 * @return 0 If vif can send frame (i.e. vif's channel is the current active channel)
 * and !=0 otherwise
 ****************************************************************************************
 */
int chan_tbtt_start(struct vif_info_tag *vif, uint32_t tbtt_time, uint32_t next_tbtt_time);

/**
 ****************************************************************************************
 * @brief Indicates end of the TBTT window
 *
 * It is called by MM at the end of the TBTT window (i.e. when beacon has been received or
 * timeout has been reached).
 * If multiple channel contexts are active, it will add the next TBTT in the list and
 * scheduled channel switches until next scheduled TBTT.
 *
 * @param[in] vif VIF Entry
 ****************************************************************************************
 */
void chan_bcn_to_evt(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Called when TBTT timer has been updated after beacon reception.
 *
 * If this function is called in a "TBTT window", the fucntion will retrun immediately
 * as update will be handled in @ref chan_bcn_to_evt.
 *
 * If this function is called outside of "TBTT window" it will update TBTT info for this
 * vif.
 *
 * @param[in] vif VIF Entry
 ****************************************************************************************
 */
void chan_tbtt_updated(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Start a Remain on Channel procedure (duration is the beacon interval) in order
 *        to catch the beacon sent by a peer AP device.
 *
 * @param[in] vif VIF Entry
 ****************************************************************************************
 */
void chan_bcn_detect_start(struct vif_info_tag *vif);

#if (NX_P2P)
/**
 ****************************************************************************************
 * @brief Function used by the P2P module to notify the chan module about an update of
 *        presence status of a GO device.
 *
 * @param[in] ctxt     Context linked to the p2p vif
 * @param[in] absence  True if GO is absent, false else
 ****************************************************************************************
 */
void chan_p2p_absence_update(struct chan_ctxt_tag *ctxt, bool absence);
#endif //(NX_P2P)

/**
 ****************************************************************************************
 * @brief Check if the VIF passed as parameter has currently access to the channel.
 *
 * @param[in] vif         Pointer to the VIF structure
 *
 * @return true if the device is on channel, false otherwise
 ****************************************************************************************
 */
bool chan_is_on_channel(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Check if the VIF passed as parameter is currently on its operational channel.
 *
 * @param[in] vif          Pointer to the VIF structure
 *
 * @return true if the VIF is on its operational channel, false otherwise
 ****************************************************************************************
 */
bool chan_is_on_operational_channel(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Check if transmission is allowed on a vif's channel
 *
 * Transmission is allowed as long as the vif is on the current channel and no channel
 * switch is in progress.
 * Exception: Transmitting NULL frame for notification of absence
 * (i.e. status==CHAN_SENDING_NOA) is allowed during a channel switch
 *
 * @param[in] vif           Pointer to the VIF structure
 *
 * @return true if a frame can be pushed, false otherwise
 ****************************************************************************************
 */
bool chan_is_tx_allowed(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Update TX power for a channel context.
 * Set tx_power to the minimum value allowed among all vifs linked to this channel context.
 * This power value is used to configure tx power used in frame generated by HW (Ack,...).
 * Data frame will use the tx power configured at vif level.
 *
 * @param[in] ctxt Channel context to update.
 ****************************************************************************************
 */
void chan_update_tx_power(struct chan_ctxt_tag *ctxt);

#if BK_MAC
uint16_t chan_get_vif_frequency(struct vif_info_tag *vif);
#endif  /* BK_MAC */

#endif //(NX_CHNL_CTXT)

/// @} end of group

#endif //_CHAN_H_
