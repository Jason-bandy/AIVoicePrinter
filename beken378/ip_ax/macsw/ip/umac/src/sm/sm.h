/**
****************************************************************************************
*
* @file sm.h
*
* Copyright (C) RivieraWaves 2011-2020
*
* @brief Declaration of all structures and functions used by the SM module.
*
****************************************************************************************
*/
#ifndef _SM_H_
#define _SM_H_

/** @defgroup SM SM
 * @ingroup UMAC
 * @brief The Station Manager (SM) module is responsible for all the procedures related
 * to the STA role.
 * It implements the state machines and functions that are involved in the connection and
 * disconnection procedures.
 * @{
 */

// for mac_addr and other definitions
#include "mac.h"
#include "ke_task.h"

/// Maximum length of the AssocReq IEs
#define SM_MAX_IE_LEN   256

struct rxu_mgt_ind;
struct vif_info_tag;

/// station environment structure
struct sm_env_tag
{
    /// Pointer to the connection parameters
    struct sm_connect_req const *connect_param;
    /// Pointer to the structure used for the connect indication upload
    struct sm_connect_ind *connect_ind;
    /// Pointer to the message structure used for the disconnect indication
    struct ke_msg *disconnect_ind;
    /// List of BSS configuration messages to send
    struct co_list bss_config;
    /// Indicate if pending disconnection was requested by host or not
    bool host_disconnect;
    /// Indicate if passive scan has been used during join procedure
    bool join_passive;
    /// Whether Reassociation should be used instead of association
    bool reassoc;
    /// MAC address of previous AP - valid only if reassoc is true
    struct mac_addr prev_bssid;
    /// Pointer to the VIF structure for which the disconnection procedure is pending
    struct vif_info_tag *vif_disconnect;
};

/**
 ****************************************************************************************
 * @brief Initialize the SM context.
 ****************************************************************************************
 */
void sm_init(void);

/**
 ****************************************************************************************
 * @brief Search for the BSSID and Channel information in the scan results and/or the
 * connection parameters.
 *
 * @param[out] bssid  Pointer to the BSSID to join
 * @param[out] chan   Pointer to the channel on which the BSSID is
 ****************************************************************************************
 */
void sm_get_bss_params(struct mac_addr const **bssid,
                       struct mac_chan_def const **chan);

/**
 ****************************************************************************************
 * @brief Try to join the BSS indicated by the parameters.
 *
 * @param[in] bssid   Pointer to the BSSID to join
 * @param[in] chan    Pointer to the channel on which the BSSID is
 * @param[in] passive Indicate if passive scan has to be started
 ****************************************************************************************
 */
void sm_join_bss(struct mac_addr const *bssid,
                 struct mac_chan_def const *chan,
                 bool passive);

/**
 ****************************************************************************************
 * @brief Launch the scan to find the target BSS.
 *
 * @param[in] bssid   Pointer to the BSSID to join, if known. If not the WildCard BSSID
 *                    will be used for the scan.
 * @param[in] chan    Pointer to the channel on which the BSSID is, if known. If not the
 *                    scan is performed on all channels.
 ****************************************************************************************
 */
void sm_scan_bss(struct mac_addr const *bssid,
                 struct mac_chan_def const *chan);

/**
 ****************************************************************************************
 * @brief Function called at any time during the connection, used to indicate to the
 *        host the completion of the procedure (either successful or not).
 *
 * @param[in] status  Status of the connection procedure (@ref MAC_ST_SUCCESSFUL or any
 *                    other 802.11 status code)
 ****************************************************************************************
 */
void sm_connect_ind(uint16_t status);

/**
 ****************************************************************************************
 * @brief Function called upon reception of a AUTH frame from the AP.
 *
 * @param[in] param  Pointer to the kernel message containing the AUTH frame.
 ****************************************************************************************
 */
void sm_auth_handler(struct rxu_mgt_ind const *param);

/**
 ****************************************************************************************
 * @brief Function called upon reception of a ASSOC_RSP frame from the AP.
 *
 * @param[in] param  Pointer to the kernel message containing the ASSOC_RSP frame.
 ****************************************************************************************
 */
void sm_assoc_rsp_handler(struct rxu_mgt_ind const *param);

/**
 ****************************************************************************************
 * @brief Function called upon reception of a DEAUTH or DISASSOC frame from the AP.
 *
 * @param[in] param  Pointer to the kernel message containing the DEAUTH/DISASSOC frame.
 *
 * @return The message status to be returned to the kernel (@ref KE_MSG_CONSUMED or
 *         @ref KE_MSG_SAVED)
 ****************************************************************************************
 */
int sm_deauth_handler(struct rxu_mgt_ind const *param);

#ifdef NX_MFP
/**
 ****************************************************************************************
 * @brief Function called upon reception of a SA QUERY action frame.
 *
 * Only SA QUERY Request received on STA interfaces are handled.
 *
 * @param[in] param  Pointer to the kernel message containing SA_QUERY Action frame.
 ****************************************************************************************
 */
void sm_sa_query_handler(struct rxu_mgt_ind const *param);
#endif // NX_MFP

/**
 ****************************************************************************************
 * @brief Add the channel context for the new connection
 *
 * @param[in,out] chan_idx      Pointer to the value that will contain the allocated channel
 *                              index.
 *
 * @return the status of the MM_CHAN_CTXT_ADD_REQ handling
 ****************************************************************************************
 */
uint8_t sm_add_chan_ctx(uint8_t *chan_idx);

/**
 ****************************************************************************************
 * @brief Set the BSS parameters
 * This function prepares the list of BSS configuration messages that will be transmitted
 * to the Lower MAC.
 *
 ****************************************************************************************
 */
void sm_set_bss_param(void);

/**
 ****************************************************************************************
 * @brief Send the next BSS parameter message present in the list
 *
 ****************************************************************************************
 */
void sm_bss_config_send(void);

/**
 ****************************************************************************************
 * @brief Start the disconnection procedure upon the reception of a
 * @ref SM_DISCONNECT_REQ.
 *
 * @param[in] vif_index   Index of the VIF that has to disconnect
 * @param[in] reason_code 802.11 reason code that needs to be included in the DEAUTH
 *                        frame.
 ****************************************************************************************
 */
void sm_disconnect(uint8_t vif_index, uint16_t reason_code);

/**
 ****************************************************************************************
 * @brief Terminates the disconnection procedure after reception of a DEAUTH/DISASSOC or
 *        the transmission of a DEAUTH frame.
 *
 * @param[in] vif    Pointer to the environment structure of the VIF that is disconnected
 * @param[in] reason 802.11 reason code that will be forwarded to the host in the @ref
 *                   SM_DISCONNECT_IND
 ****************************************************************************************
 */
void sm_disconnect_process(struct vif_info_tag *vif, uint16_t reason);

/**
 ****************************************************************************************
 * @brief Association completed operations.
 *
 * @param[in] aid Association Identifier provided by AP on connection (0 if IBSS).
 ****************************************************************************************
 */
void sm_assoc_done(uint16_t aid);

/**
 ****************************************************************************************
 * @brief Send MAC_FCTRL_ASSOCREQ or MAC_FCTRL_REASSOCREQ to the air
 ****************************************************************************************
 */
void sm_assoc_req_send(void);

/**
 ****************************************************************************************
 * @brief Send Authentication frame
 *
 * @param[in] auth_seq  Authentication sequence
 * @param[in] challenge Pointer on authentication challenge. Only needed for third
                        sequence of SHARED_KEY authentication. Should be NULL otherwise
 ****************************************************************************************
 */
void sm_auth_send(uint16_t auth_seq, uint32_t *challenge);

/**
 ****************************************************************************************
 * @brief Start External Authentication procedure
 *
 * Used when fw doesn't support an authentication method.
 * In this case the authentication is offloaded to the host.
 *
 * @param[in] akm  Authentication Key Management used for this connection
 ****************************************************************************************
 */
void sm_external_auth_start(uint32_t akm);

/**
 ****************************************************************************************
 * @brief End External Authentication procedure
 *
 * Called when host send status for external authentication.
 *
 * @param[in] status  Status (as in Status code of AUTH frame) of the external
 *                    authentication procedure
 ****************************************************************************************
 */
void sm_external_auth_end(uint16_t status);

/**
 ****************************************************************************************
 * @brief Check if external authentication is in progress
 * @return true if external authentication is in progress and false otherwise
 ****************************************************************************************
 */
bool sm_external_auth_in_progress(void);

/**
 ****************************************************************************************
 * @brief Starts FT over the air transitional state
 *
 * During FT over the air MIC must be computed between authentication and association.
 * As firmware doesn't support this, ask host to compute it. This function sends the
 * computation request to the host.
 *
 * @param[in] vif_idx    Index of the VIF
 * @param[in] ft_ie      HW address of the FT elements buffer
 * @param[in] ft_ie_len  Length, in bytes, of the ft_ie buffer
 ****************************************************************************************
 */
void sm_ft_auth_over_air_start(uint8_t vif_idx, uint32_t ft_ie, uint16_t ft_ie_len);

/**
 ****************************************************************************************
 * @brief Ends FT over the air transitional state
 *
 * Once host send back the updated elements, the association can continue.
 * The structure sm_connect_req of FT is reused and filled with missing information.
 * The current sm_connect_req structure stored inside sm_env.connect_param is updated and
 * replaced with the new one
 *
 * @param[in] param      Pointer to the structure sm_connect_req to use. To avoid several
 * copies of IEs elements,the FT uses directly a structure sm_connect_req message
 ****************************************************************************************
 */
void sm_ft_auth_over_air_end(struct sm_connect_req *param);

/**
 ****************************************************************************************
 * @brief Return PMKID count from RSN IE
 *
 * Find PMKID count field of RSN IE in a buffer of several IE.
 *
 * @param[in] ies     HW Address of IEs buffer
 * @param[in] ies_len Size, in bytes, of IEs buffer
 * @return PMKID count inside RSN IE. If RSN IE is not present or PMKID count is not
 * present then 0 is returned
 ****************************************************************************************
 */
int sm_get_rsnie_pmkid_count(uint32_t ies, uint16_t ies_len);

#if BK_MAC
int sm_disassoc_handler(struct rxu_mgt_ind const *param);
#endif

/// SM module environment
extern struct sm_env_tag sm_env;

void sm_build_broadcast_deauthenticate(void);

int sm_disassoc_handler(struct rxu_mgt_ind const *param);

/// @} //end of group

#endif // _SM_H_
