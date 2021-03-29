/**
 ****************************************************************************************
 * @file tdls.h
 *
 * @brief Tunneled direct-link setup (TDLS) module definition.
 *
 * Copyright (C) RivieraWaves 2016-2019
 *
 ****************************************************************************************
 */

#ifndef TDLS_H_
#define TDLS_H_

#if NX_UMAC_PRESENT && NX_TDLS
/**
 *****************************************************************************************
 * @defgroup TDLS TLDS
 * @ingroup LMAC
 * @brief TDLS module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"
#include "co_bool.h"

#include "co_utils.h"
#include "ke_timer.h"
#include "tdls_task.h"
#include "hal_desc.h"
#include "hal_machw.h"
#include "hal_dma.h"
#include "sta_mgmt.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Peer Traffic Indication Response timeout, in us
#define TDLS_PTI_RSP_TOUT_US 5000000
/// Keep-alive time, in us
#define TDLS_KEEP_ALIVE_US 10000000
/// Time spent to switch channel, in us
#define TDLS_CHSW_SWITCH_TIME_US 5000
/// Time spent to transmit a frame included SIFS, in us
#define TDLS_CHSW_TX_FRAME_TIME_US 1000
/// Max channel switch timeout, in us
#define TDLS_MAX_CHSW_SWITCH_TIME_US 15000
/// Delay of Channel Switch Request on TBTT time, in us
#define TDLS_CHSW_REQ_DELAY_US 10000
/// Number of keep-alive frames transmitted before considering the TDLS peer unreachable
#define TDLS_KA_RETRIES 2
/// Delay of Channel Switch Request when there is traffic with AP, in us
#define TDLS_CHSW_REQ_DELAY_AP_TRAFFIC_US 20000

// Forward declaration
struct vif_info_tag;

/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief This function is used to compute the time to the next TBTT.
 * The actual TBTT time taken into account is actually next_tbtt -
 * @ref TDLS_CHSW_REQ_DELAY_US in order to consider the delay to send the channel switch
 * request.
 *
 * @param[in] next_tbtt Next TBTT time
 * @param[in] now       Current time
 *
 * @return The time to the next TBTT.
 ****************************************************************************************
 */
__INLINE uint32_t tdls_get_dt_us(uint32_t next_tbtt, uint32_t now)
{
    return (uint32_t)((0xFFFFFFFF - now) + next_tbtt + 1) - TDLS_CHSW_REQ_DELAY_US;
}

/**
 ****************************************************************************************
 * @brief This function processes the received TDLS frames.
 *
 * @param[in] frame     Pointer to the frame
 * @param[in] vif       Pointer to the VIF entry
 * @param[in] sta_idx   Index of the TDLS sender station
 *
 * @return A boolean indicating if the frame is handled internally, or needs to be
 * forwarded to the host.
 ****************************************************************************************
 */
bool tdls_check_frame(uint8_t *frame, struct vif_info_tag *vif, uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Callback called upon TDLS channel switch request timer expiration.
 * This timer is started when channel switch is enabled and periodically transmits the
 * TDLS Channel Switch Request frame.
 * In case there is traffic with the AP, the transmission of TDLS Channel Switch Request
 * frame is delayed.
 * Next TDLS channel switch request timer time is set to 10 ms after next TBTT.
 *
 * @param[in] env   Pointer the VIF entry
 ****************************************************************************************
 */
void tdls_chsw_req_evt(void *env);

/**
 ****************************************************************************************
 * @brief TDLS channel switch to base channel indication.
 * This function sends the @ref TDLS_CHAN_SWITCH_BASE_IND message and sets the new state of
 * TDLS task as TDLS_BASE_CHANNEL.
 *
 * @param[in] roc_chan_ctxt pointer to the context allocated for the remain of channel
 *                          operation
 ****************************************************************************************
 */
void tdls_send_chan_switch_base_ind(struct chan_ctxt_tag *roc_chan_ctxt);

/**
 ****************************************************************************************
 * @brief TDLS peer power save indication.
 * This function sends the @ref TDLS_PEER_PS_IND message.
 *
 * @param[in] vif       pointer to VIF entry
 * @param[in] ps_on     Power save status
 * @param[in] sta_idx   Index of the TDLS sender station
 ****************************************************************************************
 */
void tdls_send_peer_ps_ind(struct vif_info_tag *vif, bool ps_on, uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Callback function indicating the completion of the TDLS Peer Traffic Indication
 * Request frame transmission.
 *
 * @param[in] env       TX callback parameter (in this case, pointer to TDLS peer)
 * @param[in] status    status of the transmission
 ****************************************************************************************
 */
void tdls_peer_traffic_ind_tx_cfm(void *env, uint32_t status);

/**
 ****************************************************************************************
 * @brief Add TDLS station.
 * This function initialises the TDLS station parameters.
 *
 * @param[in] sta           Pointer to TDLS station entry
 * @param[in] initiator     The STA is the TDLS link initiator
 * @param[in] chsw_allowed  TDLS Channel Switch is allowed with this STA
 ****************************************************************************************
 */
void tdls_add_sta(struct sta_info_tag *sta, bool initiator, bool chsw_allowed);

/**
 ****************************************************************************************
 * @brief Del TDLS station.
 * This function clears the TDLS station parameters.
 *
 * @param[in] sta       Pointer to TDLS station entry
 ****************************************************************************************
 */
void tdls_del_sta(struct sta_info_tag *sta);


#endif // NX_UMAC_PRESENT && NX_TDLS

/// @} end of group

#endif // TDLS_H_
