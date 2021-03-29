/**
 ****************************************************************************************
 *
 * @file apm.h
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 * @brief Declaration of the APM module environment.
 *
 ****************************************************************************************
 */

#ifndef _APM_H_
#define _APM_H_

/** @defgroup APM APM
* @ingroup UMAC
* @brief Declaration of all structures and functions used by the APM module.
* @{
*/

#include "mac.h"
#include "co_list.h"
#include "rwnx_config.h"
#include "tx_swdesc.h"
#include "sta_mgmt.h"

#if NX_BEACONING

/// APM environment declaration
struct apm
{
    /// Pointer to the AP parameters
    struct apm_start_req const *param;
    /// List of BSS configuration messages to send
    struct co_list bss_config;
    /// Used in the Aging Algorithm
    uint8_t aging_sta_idx;
};


/// Aging Duration : The time period is 100 sec
#define AGING_DURATION       25 * TU_DURATION * TU_DURATION
/// number of stations to be checked for aging
#define MAC_AGING_STA                    10
/// the threshold for null frame
#define STAID_NOTRAFFIC_THRESHOLD_NULL   3
/// the value of the QOS capability info
#define QOS_CAPA_VALUE                  0x10

// Forward declaration
struct vif_info_tag;

/*
* FUNCTION PROTOTYPES
****************************************************************************************
*/

/**
****************************************************************************************
* @brief Initialize the APM context
****************************************************************************************
*/
void apm_init(void);

/**
****************************************************************************************
* @brief Send the AP starting confirmation to the upper layers
*
* @param[in] status Status of the AP starting procedure
****************************************************************************************
*/
void apm_start_cfm(uint8_t status);


/**
****************************************************************************************
* @brief Set the BSS parameters to the LMAC/MACHW
****************************************************************************************
*/
void apm_set_bss_param(void);

/**
 ****************************************************************************************
 * @brief Send the next BSS parameter message present in the list
 *
 ****************************************************************************************
 */
void apm_bss_config_send(void);

/**
****************************************************************************************
* @brief Send the AP beacon information to the Lower MAC
****************************************************************************************
*/
void apm_bcn_set(void);

/**
****************************************************************************************
* @brief Stop the AP
*
* @param[in] vif Pointer to the VIF instance
****************************************************************************************
*/
void apm_stop(struct vif_info_tag *vif);

/**
****************************************************************************************
* @brief Checks if an internal frame can be sent
*
* Checks if an internal frame should be postponed because the peer is in power save mode.
* Does nothing if sending vif is not of type @ref VIF_AP.
*
* @param[in] txdesc TX desc of the frame to check
* @return false if the frame is about to be sent via an AP interface to a station that
* is currently in power save mode, true otherwise
****************************************************************************************
*/
bool apm_tx_int_ps_check(struct txdesc *txdesc);

/**
****************************************************************************************
* @brief Updates status after postponing an internal frame
*
* If an internal frame has been postponed because peer in in PS (@ref apm_tx_int_ps_check),
* this function will update internal status and TIM IE if necessary.
*
* @param[in] txdesc TX desc of the frame postponed
* @param[in] sta    STA entry for the destination of the frame
****************************************************************************************
*/
void apm_tx_int_ps_postpone(struct txdesc *txdesc, struct sta_info_tag *sta);

/**
****************************************************************************************
* @brief Updates status after sending a postponed frame.
*
* It is called before sending a postponed frame. If the postpone reason was PS then this
* function will update TIM IE or set MORE_DATA flag in the frame.
*
* @param[in] txdesc TX desc of the frame postponed
* @param[in] sta    STA entry for the destination of the frame
****************************************************************************************
*/
void apm_tx_int_ps_sent(struct txdesc *txdesc, struct sta_info_tag *sta);

/**
****************************************************************************************
* @brief Get the next txdesc to push from postpone queue.
*
* When a PS service period (legacy PS or U_APSD) is in progress, this function will
* extract the next frame to push from the postpone queue.
* Since both legacy PS and U-APSD frames may be present in the postpone queue, the
* function will take care to extract the first frame that correspond to the service
* period. After selecting a frame, the rest of the queue is checked to see there there
* are pending frame to correctly set the MORE_DATA flag.
*
* On a non AP interface or outside of a service period, it immediately return NULL and
* set @p stop to 0.
*
* @param[in]  vif  VIF entry that will send the frame
* @param[in]  sta  STA entry for the destination of the frame
* @param[out] stop Indicates if more postponed frames can be pushed
*
* @return txdesc to push. If txdesc is NULL then it is necessary to test the value of
* @p stop parameter. If set to 1 it means that no more frame can be pushed for this
* service period. If set to 0, it means that there is no service period in progress
* and postpone queue can be pushed in its current order.
***************************************************************************************
*/
struct txdesc *apm_tx_int_ps_get_postpone(struct vif_info_tag *vif,
                                          struct sta_info_tag *sta,
                                          int *stop);

/**
****************************************************************************************
* @brief Clears PS status regarding internal frame
*
* Call to push all postponed frames and clear PS status of internal PS traffic, when a
* STA exit PS state.
* Does nothing when called on a non AP interface.
*
* @param[in]  vif     VIF entry.
* @param[in]  sta_idx Index of the sta that exit PS.
***************************************************************************************
*/
void apm_tx_int_ps_clear(struct vif_info_tag *vif, uint8_t sta_idx);

/**
****************************************************************************************
* @brief Probe client of an AP
*
* Send a NULL frame to a client to check whether it is still reachable or not.
*
* @param[in]  vif       VIF entry.
* @param[in]  sta_idx   Index of the sta to probe.
* @param[out] probe_id  Pointer to retrieve probe id
* @return CO_OK if probe has been pushed and != CO_OK otherwise
***************************************************************************************
*/
uint8_t apm_probe_client(struct vif_info_tag *vif, uint8_t sta_idx, uint32_t *probe_id);

#if BK_MAC
void apm_send_action_csa_frame(struct vif_info_tag *vif);
#endif

/// APM module environment declaration.
extern struct apm apm_env;

#endif

/// @}

#endif // _APM_H_
