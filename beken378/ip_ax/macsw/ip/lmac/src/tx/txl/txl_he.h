/**
 ****************************************************************************************
 *
 * @file txl_he.h
 *
 * @brief HE TX definitions.
 *
 * Copyright (C) RivieraWaves 2018-2020
 *
 ****************************************************************************************
 */

#ifndef _TXL_HE_H_
#define _TXL_HE_H_


/**
 *****************************************************************************************
 * @defgroup TX_HE TX_HE
 * @ingroup TX
 * @brief Functions specific to the HE transmissions (e.g. HE TB transmissions).
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "hal_desc.h"
#include "mm_timer.h"

#if NX_HE
struct sta_info_tag;
struct vif_info_tag;
struct txdesc;
struct rxdesc;
struct mm_set_mu_edca_req;
struct mm_set_uora_req;
/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
/// MU EDCA parameters for a given access category
struct mu_edca_param_tag
{
    /// MU-EDCA timer, used to re-activate EDCA normal values when no trigger frame is
    /// received
    struct mm_timer_tag timer;
    /// Pointer to the first frame exchange of the TX list
    struct tx_hd *first_frame_exch;
    /// MAC HW EDCA parameters;
    uint32_t edca;
    /// Timeout value
    uint8_t timeout;
    /// Current timer value
    uint8_t timer_current;
    /// EDCA off
    bool edca_off;
    /// Flag indicating if the queue had HE TB activity in the last MU EDCA period
    bool tb_active;
};


/// UORA parameters structure
struct uora_tag
{
    /// Minimum exponent of OFDMA Contention Window.
    uint8_t eocw_min;
    /// Maximum exponent of OFDMA Contention Window.
    uint8_t eocw_max;
};
/// MU EDCA operation structure
struct mu_edca_tag
{
    /// MU EDCA values to be applied to access categories (+1 dummy for beacon)
    struct mu_edca_param_tag params[NX_TXQ_CNT];
    /// Flag indicating whether the MU EDCA parameters are valid or not (i.e. it indicates
    /// if a MU EDCA element has been received from the AP
    bool valid;
};

/// Information structure about the received HE trigger frame
struct he_trigger_tag
{
    /// Peer device information structure
    struct sta_info_tag *sta;
    /// Value of the UPH control that shall be put in the HE TB MPDUs
    uint32_t uph;
    /// Maximum length of the HE TB PPDU
    uint32_t max_len;
    /// Transmitter Address of the trigger frame
    struct mac_addr ta;
    /// Minimum start spacing inside the HE TB PPDU
    uint16_t mmss;
    /// Legacy length for the HE TB
    uint16_t ul_length;
    /// Access category on which the transmission is done
    uint8_t ac;
    /// Type of trigger frame received
    uint8_t trig_type;
    /// Type of GI to use for the HE TB
    uint8_t gi_type;
    /// MCS to use for the HE TB
    uint8_t ul_mcs;
    /// NSS to use for the HE TB
    uint8_t ul_nss;
    /// BW of the HE TB PPDU
    uint8_t ul_bw;
    /// RU size for the HE TB
    uint8_t ru_size;
    /// Minimum AC for the HE TB
    uint8_t pref_ac;
    /// Minimum Start Spacing Factor for the HE TB
    uint8_t spacing_factor;
    /// Flag indicating whether the current HE TB is a UORA one or not
    bool uora;
    /// Flag indicating whether the received trigger frame is valid or not
    bool trig_valid;
    /// A-MPDU programmed for this HE trigger frame (only for debug purpose)
    struct tx_agg_desc *agg_desc;
};

/// Flags for HE transmission global status
enum txl_he_status
{
    // A HE-TB with 'real' data (i.e. associated to a txdesc) is currently being transmitted
    HE_TB_DATA_ON_GOING_BIT = CO_BIT(0),
    // A He-TB with no data (i.e. Qos Null, BSRP, TWT-PS-POLL) is currently being transmitted
    HE_TB_NON_DATA_ON_GOING_BIT = CO_BIT(1),
    // The next trigger based transmission will be cancelled by MACHW (so no need to prepare it)
    HE_TB_CANCEL_NEXT_BIT = CO_BIT(2),
};

/// Information structure about the HE transmission module
struct txl_he_env_tag
{
    #if NX_UMAC_PRESENT && NX_BEACONING
    /// Timer used in AP mode for scheduling the trigger frame transmissions
    struct mm_timer_tag trig_timer;
    #endif
    /// VIF for which HE TB is enabled
    struct vif_info_tag *tb_vif;
    /// MU EDCA parameters
    struct mu_edca_tag mu_edca;
    /// UORA parameters
    struct uora_tag uora;
    /// HE trigger frame information structure
    struct he_trigger_tag trigger;
    /// Global status (@ref txl_he_status)
    uint32_t status;
};

/*
 * DEFINES
 ****************************************************************************************
 */
/// Minimum time needed between end of A-MPDU split and chaining of the HE TB to HW (in us)
#define TX_HE_TB_PROG_TIME 6

/// Value to put in +HTC HE BSR to indicate the highest possible data buffering to the AP
#define TX_HE_BSR_FULL (MAC_HTC_HE_CTRL_ID_BSR | MAC_HTC_HE_BSR_QSIZE_ALL_MSK |          \
                        MAC_HTC_HE_BSR_QSIZE_HIGH_MSK | MAC_HTC_HE_BSR_SCALING_FAC_MSK | \
                        MAC_HTC_HE_BSR_DELTA_TID_MSK)

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
/// TX HE context variable
extern struct txl_he_env_tag txl_he_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/** @name External API
 @{ */

/**
 ****************************************************************************************
 * @brief Get the value of the UPH field computed for the on-ongoing HE TB PPDU
 *
 * @return The value of the UPH field computed for the on-ongoing HE TB PPDU
 ****************************************************************************************
 */
__INLINE uint32_t txl_he_tb_uph_get(void)
{
    return (txl_he_env.trigger.uph);
}

/**
 ****************************************************************************************
 * @brief Check whether the packet pointed by the TX descriptor is sent over HE TB or not,
 * and return the value (UPH or BSR) to be put in the +HTC field accordingly.
 *
 * The check is done on a status bit (@ref AGG_TB) of the AGG descriptor that may be
 * attached to the TX descriptor. If an AGG descriptor is attached (this is the case for
 * all MPDUs, including singleton, sent as HE TB), and the @ref AGG_TB bit is set in its
 * status, then the packet is currently sent as a HE TB PPDU and the UPH value is
 * returned. In all other cases, the latest BSR computed is returned.
 * This function is called when the MAC Header of the packet is built, i.e. after buffer
 * allocation.
 *
 * @param[in] txdesc TX descriptor to be checked
 * @param[in] sta Pointer to the STA entry to which this transmission is linked
 *
 * @return The value (UPH or BSR) to put in the +HTC field
 ****************************************************************************************
 */
uint32_t txl_he_htc_get(struct txdesc *txdesc, struct sta_info_tag *sta);

/**
 ****************************************************************************************
 * @brief Check whether the rate programmed in the ratecntrlinfo field is a HE SU one.
 *
 * @param[in] rate_info Rate information field.
 *
 * @return true if programmed rate is HE SU, false otherwise
 ****************************************************************************************
 */
__INLINE bool txl_he_is_he_su(uint32_t rate_info)
{
    uint32_t format = (rate_info & FORMAT_MOD_TX_RCX_MASK) >> FORMAT_MOD_TX_RCX_OFT;

    return (format == FORMATMOD_HE_SU);
}

/**
 ****************************************************************************************
 * @brief Check whether EDCA is disallowed or not on the given access category
 *
 * This is used as part of the MU EDCA operation.
 *
 * @param[in] access_category Access category to be checked.
 *
 * @return true if EDCA is disallowed, false otherwise
 ****************************************************************************************
 */
__INLINE bool txl_he_is_edca_off(uint8_t access_category)
{
    return (txl_he_env.mu_edca.params[access_category].edca_off);
}

/**
 ****************************************************************************************
 * @brief Set the LTF type and DCM bit in the powercntrlinfo field of the policy table,
 * based on the information in the ratecntrlinfo field.
 *
 * @param[in] rate_info Rate information field as defined in ratecntrlinfo field of
 *                      policy table
 * @param[in] dcm       Boolean indicating whether DCM is enabled
 * @param[in,out] pwr_info Power information field to be put in the policy table
 ****************************************************************************************
 */
__INLINE void txl_he_pwr_info_set(uint32_t rate_info, bool dcm, uint32_t *pwr_info)
{
    int he_ltf_type = TX_2x_HE_LTF_FOR_6_4_US;
    // Check GI type and update HE-LTF type accordingly
    if ((rate_info & HE_GI_TYPE_TX_RCX_MASK) == GI_TYPE_3_2)
        he_ltf_type = TX_4x_HE_LTF_FOR_12_8_US;

    *pwr_info &= ~TX_HE_LTF_TYPE_PT_RCX_MASK;
    *pwr_info |= he_ltf_type;
    if (dcm)
        *pwr_info |= TX_HE_DCM_BIT;
    else
        *pwr_info &= ~TX_HE_DCM_BIT;
}

/**
 ****************************************************************************************
 * @brief Check whether a HE TB PPDU transmission is ongoing.
 *
 * @param[out] ac   The access category of the HE TB PPDU, if applicable
 *
 * @return true if a HE TB PPDU transmission is ongoing, false otherwise
 ****************************************************************************************
 */
__INLINE bool txl_he_tb_ongoing(uint8_t *ac)
{
    *ac = txl_he_env.trigger.ac;

    return (txl_he_env.status & HE_TB_DATA_ON_GOING_BIT);
}

/**
 ****************************************************************************************
 * @brief Check whether EDCA transmissions can be programmed for a given access category.
 *
 * Two cases disallow EDCA programming:
 *     - EDCA is off due to MU EDCA parameters
 *     - A HE TB transmission is ongoing on the given access category
 *
 * @param[in] ac The access category to be checked
 *
 * @return true if EDCA is allowed, false otherwise
 ****************************************************************************************
 */
__INLINE bool txl_he_tb_can_chain_edca(uint8_t ac)
{
    uint8_t tb_ac;

    return (!txl_he_is_edca_off(ac) && (!txl_he_tb_ongoing(&tb_ac) || (tb_ac != ac)));
}

/**
 ****************************************************************************************
 * @brief Set the value of the new EDCA head pointer.
 *
 * This function is called when a packet is pushed for TX while EDCA is disabled or a HE
 * TB is ongoing, and the queue is currently empty.
 *
 * @param[in] thd New THD head
 * @param[in] ac The access category on which the packet is pushed
 ****************************************************************************************
 */
__INLINE void txl_he_tb_new_edca_head(struct tx_hd *thd, uint8_t ac)
{
    struct mu_edca_param_tag *mu_edca_param = &txl_he_env.mu_edca.params[ac];
    mu_edca_param->first_frame_exch = thd;
}

/**
 ****************************************************************************************
 * @brief Get the queue size
 *
 * The Queue Size subfield is an 8-bit field that indicates the amount of buffered traffic
 * at STA for transmission to the HE AP.
 *
 * @param[out] buffered   Buffered trafic at the STA in bytes
 *
 * @return the queue size
 ****************************************************************************************
 */
__INLINE uint8_t txl_he_get_queue_size(uint32_t buffered)
{
    uint8_t queue_size, sf = 0, uv = 0;
    const uint8_t sf_oft = 6;

    if(buffered <= 1008)
    {
        sf = 0;
        uv = (CO_ALIGNx_HI(buffered, 16)) / 16;
    }
    else if (buffered <= 1024)
    {
        sf = 1;
        uv = 0;
    }
    else if (buffered <= 17152)
    {
        sf = 1;
        uv = (CO_ALIGNx_HI(buffered - 1024, 16)) / 16;
    }
    else if (buffered <= 17408)
    {
        sf = 2;
        uv = 0;
    }
    else if (buffered <= 146432)
    {
        sf = 2;
        uv = (CO_ALIGNx_HI(buffered - 17408, 2048)) / 2048;
    }
    else if (buffered <= 148480)
    {
        sf = 3;
        uv = 0;
    }
    else if (buffered <= 2147328)
    {
        sf = 3;
        uv = (CO_ALIGNx_HI(buffered - 148480, 32768)) / 32768;
    }
    else if (buffered < 4294967295)
    {
        sf = 3;
        uv = 62;
    }
    else // Queue size is unspecified or unknown
    {
        sf = 3;
        uv = 63;
    }

    queue_size = (sf << sf_oft) | uv;

    return queue_size;
}
/**
 ****************************************************************************************
 * @brief Initialize the TX HE module.
 *
 * This primitive initializes all the elements used in the TX HE module.
 *
 ****************************************************************************************
 */
void txl_he_init(void);

/**
 ****************************************************************************************
 * @brief Reset the TX HE module.
 *
 * This function is called as part of the recovery procedure
 ****************************************************************************************
 */
void txl_he_reset(void);

/**
 ****************************************************************************************
 * @brief This function is called by the RX path to indicate the reception of a HE
 * trigger frame, that will require the transmission of a HE TB PPDU by the TX path.
 *
 * The trigger frame is released by the RX path as soon as this function returns.
 *
 * @param[in] rxdesc RX descriptor containing the information about the trigger frame
 ****************************************************************************************
 */
void txl_he_trigger_push(struct rxdesc *rxdesc);

/**
 ****************************************************************************************
 * @brief This function is called upon reception of a HE TB protocol trigger interrupt.
 *
 * This interrupt is asserted by the MAC HW following the reception of a HE trigger
 * frame, when a HE TB PPDU transmission has to be programmed by the MAC SW.
 * This function will call the RX path to get the received trigger frame, decode it and
 * program the HE TB PPDU.
 ****************************************************************************************
 */
void txl_he_tb_prot_trigger(void);

/**
 ****************************************************************************************
 * @brief This interrupt service routine is called upon the cancellation of a HE TB
 * transmission by the MAC HW.
 *
 * Such cancellation occurs when the trigger frame CS bit is set and the CCA or VCS is
 * high at the time the HE TB should be transmitted.
 * In this case the programmed HE TB PPDU is considered as not acknowledged by the AP,
 * and the MPDUs will be indicated as failed to the upper layers. Retransmissions will
 * then happen as in the normal transmission failure procedure.
 ****************************************************************************************
 */
void txl_he_tb_transmit_cancelled(void);

/**
 ****************************************************************************************
 * @brief This function is called upon reception of a HE TB transmit trigger interrupt.
 *
 * This interrupt is asserted by the MAC HW each time a HE PPDU, or the components of a HE
 * PPDU (e.g. A-MPDU sub-frames) have been transmitted.
 * The function goes through the list of descriptors, check their status, and put them in
 * the correct list (cfm or waiting block-ack). It is also responsible for freeing
 * transmitted buffers, and allocate new buffers for subsequent transmissions.
 ****************************************************************************************
 */
void txl_he_tb_transmit_trigger(void);

/**
 ****************************************************************************************
 * @brief Get the 32us length from the HE table for a given base index, bw pair
 *
 * @param[in] base_idx Base index in the table
 * @param[in] bw       Bandwidth of transmission
 *
 * @return The 32us byte length
 ****************************************************************************************
 */
uint32_t txl_he_idx_to_32us_len_get(int base_idx, uint8_t bw);

/**
 ****************************************************************************************
 * @brief Get the 1us length from the HE table for a given base index, bw pair
 *
 * @param[in] base_idx Base index in the table
 * @param[in] bw       Bandwidth of transmission
 *
 * @return The 1us byte length
 ****************************************************************************************
 */
uint16_t txl_he_idx_to_1us_len_get(int base_idx, uint8_t bw);

/**
 ****************************************************************************************
 * @brief Compute the base rate index that should be used to compute the Maximum PHY
 * length of the HE A-MPDU and the minimum A-MPDU subframe length.
 *
 * @param[in]  agg_desc    Pointer to AGG descriptor whose constraints are computed
 * @param[in]  format_mod  Modulation format (HT or VHT)
 * @param[out] max_len_sta Maximum A-MPDU length that the STA could receive
 * @param[out] nss         Number of spatial streams for the transmission
 *
 * @return The base rate index that should be used to get the maximum A-MPDU length that
 * could be transmitted on the requested PHY rate
 ****************************************************************************************
 */
int txl_he_ampdu_param_get(struct tx_agg_desc *agg_desc,
                           uint8_t format_mod,
                           uint32_t *max_len_sta,
                           uint32_t *nss);

/**
 ****************************************************************************************
 * @brief This function checks if the received Multi-STA BA contains acknowledgment
 * information for the A-MPDU we transmitted.
 *
 * @param[in] badesc  Pointer to Rx descriptor of BA frame
 * @param[in] agg_desc Pointer to the A-MPDU descriptor
 *
 * @return true if we find matching acknowledgment information for the A-MPDU,
 *         false otherwise.
 ****************************************************************************************
 */
bool txl_he_decode_m_ba(struct rxdesc *badesc, struct tx_agg_desc *agg_desc);

/**
 ****************************************************************************************
 * @brief Set the MU EDCA parameters extracted from the beacon.
 *
 * @param[in]  param      Pointer to the MM_SET_MU_EDCA_REQ parameters received from UMAC
 ****************************************************************************************
 */
void txl_he_mu_edca_param_set(struct mm_set_mu_edca_req const *param);

/**
 ****************************************************************************************
 * @brief Set the UORA parameters extracted from the beacon.
 *
 * @param[in]  param      Pointer to the MM_SET_UORA_REQ parameters received from UMAC
 ****************************************************************************************
 */
void txl_he_uora_param_set(struct mm_set_uora_req const *param);

/**
 ****************************************************************************************
 * @brief Indicate that nothing can be sent in the HE TB PPDU because of the MCS/RU
 * allocation that is not enough to fit at least one MPDU.
 *
 * This function requests the MU EDCA process to be stopped on the access category in
 * order to re-enable EDCA.
 *
 * @param[in] thd Pointer to the TX header descriptor that should have been chained to the
 *                HE TB PPDU
 * @param[in] ac  Blocked access category
 ****************************************************************************************
 */
void txl_he_mu_edca_blocked(struct tx_hd *thd, uint8_t ac);

/**
 ****************************************************************************************
 * @brief Check if HE TB operation is allowed and perform configuration accordingly.
 *
 * This function is called each time a VIF is registered/unregistered. It checks if HE
 * TB is allowed (i.e. if only one VIF is registered, and of STA type), and enable or
 * disable this feature accordingly.
 ****************************************************************************************
 */
void txl_he_tb_config(void);

/**
 ****************************************************************************************
 * @brief Disable HE TB interrupts and MU EDCA operation.
 ****************************************************************************************
 */
void txl_he_tb_disable(void);

/**
 ****************************************************************************************
 * @brief Enable HE TB interrupts.
 ****************************************************************************************
 */
void txl_he_tb_enable(void);


/**
 ****************************************************************************************
 * @brief Chain a new frame exchange in the non-EDCA list.
 *
 * This function is called when EDCA is stopped on a HW queue. It chains the given frame
 * exchange in the non-EDCA list, in order to keep track of the packets currently
 * supposed to be chained to HW.
 * The non-EDCA list is then used to populate the HW queue when restarting the EDCA
 * mechanism.
 *
 * @param[in] first_thd         Pointer to the first THD of the frame exchange
 * @param[in] last_thd          Pointer to the last THD of the frame exchange
 * @param[in] access_category   Access category
 *
 ****************************************************************************************
 */
void txl_he_non_edca_chain(struct tx_hd *first_thd,
                           struct tx_hd *last_thd,
                           uint8_t access_category);

#if NX_UMAC_PRESENT && NX_BEACONING
/**
 ****************************************************************************************
 * @brief Start the HE trigger scheduler for the STA passed as parameter.
 *
 * @note The HE trigger scheduler is only used for testing of HE TB in STA role. It is not
 * intended to be used in a real HE AP role.
 *
 * @param[in] sta Pointer to the STA for which the scheduler is started.
 ****************************************************************************************
 */
void txl_he_start_trigger_scheduler(struct sta_info_tag *sta);
#endif // NX_UMAC_PRESENT && NX_BEACONING

/**
 ****************************************************************************************
 * @brief Check whether an A-MPDU shall be protected using a RTS/CTS based on the
 * TXOP duration RTS/CTS threshold.
 *
 * If this is the case then the phy_flags are updated in the TX descriptor. This function
 * shall be called when the A-MPDU under formation is closed. If it is ready to be
 * chained, the calling function shall update the first rate control entry of the policy
 * table with the updated phy_flags.
 *
 * @param[in,out]  txdesc  Pointer to the descriptor of the first MPDU in the A-MPDU
 ****************************************************************************************
 */
void txl_he_txop_dur_rtscts_thres_ampdu_check(struct txdesc *txdesc);

/**
 ****************************************************************************************
 * @brief Check whether a singleton MPDU shall be protected using a RTS/CTS based on the
 * TXOP duration RTS/CTS threshold.
 *
 * The check is done for each rate control step of the policy table. This function shall
 * be called only when the policy table is available in the shared memory.
 *
 * @param[in,out]  txdesc  Pointer to the MPDU descriptor
 ****************************************************************************************
 */
void txl_he_txop_dur_rtscts_thres_mpdu_check(struct txdesc *txdesc);

/**
 ****************************************************************************************
 * @brief Compute the buffer status (BSR) value to be put in the +HTC field for the STA
 * entry linked to the VIF.
 *
 * Currently BSR is used only in case VIF is a HE STA connected to an HE AP, so the
 * function does the required checks and compute the BSR only in that case.
 *
 * @param[in] vif Pointer to the VIF structure
 ****************************************************************************************
 */
void txl_he_bsr_compute(struct vif_info_tag *vif);

#endif // NX_HE

/// @}
/// @}

#endif // _TXL_HE_H_
