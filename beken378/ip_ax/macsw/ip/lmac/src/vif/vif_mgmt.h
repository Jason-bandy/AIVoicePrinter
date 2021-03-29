/**
 ****************************************************************************************
 *
 * @file vif_mgmt.h
 *
 * @brief Virtual Interface Management definitions.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _VIF_MGMT_H_
#define _VIF_MGMT_H_

/**
 ****************************************************************************************
 * @defgroup VIF_MGMT VIF_MGMT
 * @ingroup LMAC
 * @brief Virtual interfaces Management.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// for linked list definitions
#include "co_list.h"
// for mac_addr and other structure definitions
#include "mac.h"
// for MM timer structure
#include "mm_timer.h"
// for MM channel structures
#include "chan.h"
// for TX frame structures
#include "txl_frame.h"
// for band
#include "phy.h"

#if (NX_UMAC_PRESENT)
#include "me.h"
#endif
#if (NX_AMPDU_RX)
// for RX definitions
#include "rxl_cntrl.h"
#endif //(NX_AMPDU_RX)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Duration before a Timeout is raised when waiting for a beacon
#define VIF_MGMT_BCN_TO_DUR     (10000)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
/// Macro defining an invalid VIF index
#define INVALID_VIF_IDX 0xFF

/// Macro defining an unknown tx power
#define VIF_UNDEF_POWER 0x7F

/// Bitfield for Broadcast/Multicast traffic status
enum VIF_AP_BCMC_STATUS
{
    /// BCMC traffic buffered
    VIF_AP_BCMC_BUFFERED = CO_BIT(0),
    /// Last BCMC traffic has been sent with more_data flag set
    VIF_AP_BCMC_MOREDATA = CO_BIT(1),
};

/// VIF Info Table
struct vif_info_tag
{
    /// linked list header
    struct co_list_hdr list_hdr;
    /// Bitfield indicating if this VIF currently allows sleep or not
    uint32_t prevent_sleep;
    /// EDCA parameters of the different TX queues for this VIF
    uint32_t txq_params[AC_MAX];

    #if (NX_MULTI_ROLE || NX_CHNL_CTXT || (NX_P2P_GO && NX_POWERSAVE))
    /// TBTT timer structure
    struct mm_timer_tag tbtt_timer;
    /// TSF offset between local tsf time and peer tsf time
    int64_t tsf_offset;
    #endif //(NX_MULTI_ROLE || NX_CHNL_CTXT || (NX_P2P_GO && NX_POWERSAVE))

    #if (NX_P2P || NX_CHNL_CTXT)
    /// Timer used for Beacon Reception Timeout
    struct mm_timer_tag tmr_bcn_to;
    #endif //(NX_P2P || NX_CHNL_CTXT)

    #if (NX_MULTI_ROLE || NX_TDLS)
    /// BSSID this VIF belongs to
    struct mac_addr bssid;
    #endif //(NX_MULTI_ROLE)

    #if (NX_CHNL_CTXT)
    /// Channel context on which this VIF is attached
    struct chan_ctxt_tag *chan_ctxt;
    /// TBTT Switch Information
    struct chan_tbtt_tag tbtt_switch;
    #endif //(NX_CHNL_CTXT)

    /// MAC address of the VIF
    struct mac_addr mac_addr;

    /// Type of the interface (@ref VIF_STA, @ref VIF_IBSS, @ref VIF_MESH_POINT or @ref VIF_AP)
    uint8_t type;
    /// Index of the interface
    uint8_t index;
    /// Flag indicating if the VIF is active or not
    bool active;

    /// TX power configured for the interface (dBm)
    int8_t tx_power;

    #if NX_UMAC_PRESENT
    /// TX power configured for the interface (dBm) by user space
    /// (Taken into account only if lower than regulatory one)
    int8_t user_tx_power;
    #endif // NX_UMAC_PRESENT

    union
    {
        /// STA specific parameter structure
        struct
        {
            #if (!NX_UMAC_PRESENT && NX_MULTI_ROLE)
            /*
             * In SMAC configuration, when STA, beacon interval is provided before
             * sta structure allocation. Hence it cannot be stored yet, ap_id value
             * is INVALID_STA_IDX.
             * When this case occurs, the beacon interval value will be stored in this
             * variable until reception of MM_STA_ADD_REQ message.
             * Value is in TUs
             */
            uint16_t ap_bcn_intv;
            #endif //(!NX_UMAC_PRESENT && NX_MULTI_ROLE)
            #if NX_POWERSAVE
            /// Listen interval
            uint16_t listen_interval;
            /// Flag indicating if we are expecting BC/MC traffic or not
            bool dont_wait_bcmc;
            /// Number of error seen during transmission of last NULL frame indicating PS change
            uint8_t ps_retry;
            #endif
            /// Index of the station being the peer AP
            uint8_t ap_id;
            /// nonTransmitted BSSID index, in case the STA is connected to a
            /// nonTransmitted BSSID of a Multiple BSSID set. Should be set to 0 otherwise
            uint8_t bssid_index;
            /// Maximum BSSID indicator, in case the STA is connected to a nonTransmitted
            /// BSSID of a Multiple BSSID set.
            uint8_t max_bssid_ind;
            #if NX_UAPSD
            /// Time of last UAPSD transmitted/received frame
            uint32_t uapsd_last_rxtx;
            /// Bitfield indicating which queues are U-APSD enabled
            uint8_t uapsd_queues;
            /// UAPSD highest TID, used in the QoS-NULL trigger frames
            uint8_t uapsd_tid;
            #endif
            #if NX_CONNECTION_MONITOR
            /// Time of last keep-alive frame sent to AP
            uint32_t mon_last_tx;
            /// CRC of last received beacon
            uint32_t mon_last_crc;
            /// Number of beacon losses since last beacon reception
            uint8_t beacon_loss_cnt;
#if BK_MAC
            /// CRC of fixed beacon ie temp count
            uint8_t mon_ie_crc_cnt;
            /// CRC of fixed beacon ie
            uint32_t mon_ie_crc;
#endif
            #endif

            #if (NX_P2P)
            /// Last computed TSF Offset
            int32_t last_tsf_offset;
            /// Addition duration to be added to the CTWindow, due to the TBTT_DELAY + drift value computed in mm_tbtt_compute
            uint32_t ctw_add_dur;
            /// Status indicated if Service Period has been paused due to GO absence
            bool sp_paused;
            #endif //(NX_P2P)

            #if (NX_P2P_GO)
            // Indicate if AP Beacon has been received at least one time
            bool bcn_rcved;
            #endif //(NX_P2P_GO)

            // Current RSSI
            int8_t rssi;
            // RSSI threshold (0=threshold not set)
            int8_t rssi_thold;
            // RSSI hysteresis
            uint8_t rssi_hyst;
            // Current status of RSSI (0=RSSI is high, 1=RSSI is low)
            bool rssi_status;

            #if NX_UMAC_PRESENT
            /// Current CSA counter
            uint8_t csa_count;
            /// Indicate if channel switch (due to CSA) just happened
            bool csa_occured;
            #endif // NX_UMAC_PRESENT

#if BK_MAC
            uint8_t mm_retry;
#endif
            #if (NX_TDLS)
            /// TDLS station index which requested the channel switch
            uint8_t tdls_chsw_sta_idx;
            #endif
        } sta;
        /// AP specific parameter structure
        struct
        {
            uint32_t dummy;
            #if NX_BCN_AUTONOMOUS_TX
            /// Frame descriptor that will be used for the beacon transmission
            struct txl_frame_desc_tag bcn_desc;
            /// Current length of the beacon (except TIM element)
            uint16_t bcn_len;
            /// Current length of the TIM information element
            uint16_t tim_len;
            /// Number of bit currently set in the TIM virtual bitmap
            uint16_t tim_bitmap_set;
            /// Beacon Interval used by AP or MP VIF (in TUs)
            uint16_t bcn_int;
            #if (NX_UMAC_PRESENT)
            /// Number of TBTT interrupts between each Beacon we have to send on this VIF
            uint8_t bcn_tbtt_ratio;
            /// Number of expected TBTT interrupts before our next Beacon transmission
            uint8_t bcn_tbtt_cnt;
            #endif //(NX_UMAC_PRESENT)
            /// Flag indicating if the beacon has been configured (i.e downloaded from host)
            bool bcn_configured;
            /// Current value of the DTIM counter
            uint8_t dtim_count;
            /// Byte index of the LSB set in the TIM virtual bitmap
            uint8_t tim_n1;
            /// Byte index of the MSB set in the TIM virtual bitmap
            uint8_t tim_n2;
            /// Status bitfield of the BC/MC traffic bufferized (@ref VIF_AP_BCMC_STATUS)
            uint8_t bc_mc_status;
            #if (!NX_UMAC_PRESENT)
            /// Number of BC/MC packet buffered
            uint8_t bc_mc_nb;
            #endif
            /// Current CSA counter
            uint8_t csa_count;
#if BK_MAC
            uint8_t csa_action_count;
#endif
            /// Current CSA offset in beacon
            uint8_t csa_oft[BCN_MAX_CSA_CPT];
            #endif
            #if (NX_UMAC_PRESENT || NX_P2P_GO)
            /// Flag indicating how many connected stations are currently in PS
            uint8_t ps_sta_cnt;
            #endif //(NX_UMAC_PRESENT || NX_P2P_GO)
            #if NX_UMAC_PRESENT
            /// Control port ethertype
            uint16_t ctrl_port_ethertype;
            #endif
        } ap;
    } u;    ///< Union of AP/STA specific parameter structures

    #if (NX_UMAC_PRESENT || NX_TD_STA)
    /// List of stations linked to this VIF
    struct co_list sta_list;
    #endif //(NX_UMAC_PRESENT || NX_TD_STA)

    #if NX_UMAC_PRESENT
    /// Information about the BSS linked to this VIF
    struct me_bss_info bss_info;
    /// Default key information
    #if NX_MFP
    struct key_info_tag key_info[MAC_DEFAULT_MFP_KEY_COUNT];
    #else
    struct key_info_tag key_info[MAC_DEFAULT_KEY_COUNT];
    #endif
    /// Pointer to the current default key used
    struct key_info_tag *default_key;
    #if NX_MFP
    /// Pointer to the current default mgmt key used
    struct key_info_tag *default_mgmt_key;
    #endif
    /// Connection flags
    uint32_t flags;
    /// Destination channel of CSA
    struct mac_chan_op csa_channel;
    #endif

    #if NX_HE
    /// TXOP RTS threshold
    uint16_t txop_dur_rts_thres;
    #endif
    #if (NX_P2P)
    /// Indicate if this interface is configured for P2P operations
    bool p2p;
    /// Index of the linked P2P Information structure
    uint8_t p2p_index;
    /// Contain current number of registered P2P links for the VIF
    uint8_t p2p_link_nb;
    #endif //(NX_P2P)

    #if (RW_UMESH_EN)
    /// Mesh ID - Index of the used mesh_vif_info_tag structure when type is VIF_MESH_POINT
    uint8_t mvif_idx;
    #endif //(RW_UMESH_EN)

#if BK_MAC
    uint32_t pre_rx_timepoint;

    // for private structs, such as lwip- netif
    void *priv;
#endif
};

/// VIF management module environment
struct vif_mgmt_env_tag
{
    /// list of all the free VIF entries
    struct co_list free_list;
    /// list of all the used VIF entries
    struct co_list used_list;
    /// Index of the monitor interface (INVALID_VIF_IDX if no monitor vif exist)
    uint8_t monitor_vif;
    #if NX_MULTI_ROLE
    /// Number of STA VIFs registered
    uint8_t vif_sta_cnt;
    /// Number of AP VIFs registered
    uint8_t vif_ap_cnt;
    #if (RW_MESH_EN)
    /// Number of MP VIFs registered
    uint8_t vif_mp_cnt;
    #endif //(RW_MESH_EN)
    #endif //(NX_MULTI_ROLE)
    #if (NX_P2P && NX_CHNL_CTXT)
    /// Number of registered P2P VIFs
    uint8_t nb_p2p_vifs;
    #endif //(NX_P2P && NX_CHNL_CTXT)
    #if (NX_UMAC_PRESENT)
    /// Index of VIF using the lowest Beacon Interval
    uint8_t low_bcn_int_idx;
    #endif //(NX_UMAC_PRESENT)
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// VIF management environment variable
extern struct vif_mgmt_env_tag vif_mgmt_env;

/// VIF information table
extern struct vif_info_tag vif_info_tab[NX_VIRT_DEV_MAX];


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
#if NX_MULTI_ROLE
/**
 ****************************************************************************************
 * @brief Get the current number of used VIFs
 *
 * @return The number of used VIFs
 ****************************************************************************************
 */
__INLINE int vif_mgmt_used_cnt(void)
{
    return(vif_mgmt_env.vif_ap_cnt + vif_mgmt_env.vif_sta_cnt);
}
#endif


/**
 ****************************************************************************************
 * @brief Get the pointer to the first VIF in the used list
 *
 * @return A pointer to the VIF instance
 ****************************************************************************************
 */
__INLINE struct vif_info_tag *vif_mgmt_first_used(void)
{
    return((struct vif_info_tag *)co_list_pick(&vif_mgmt_env.used_list));
}


/**
 ****************************************************************************************
 * @brief Get the pointer to the next VIF following the one passed as parameter.
 *
 * @param[in] vif        Pointer to the current VIF instance
 *
 * @return A pointer to the next VIF instance
 ****************************************************************************************
 */
__INLINE struct vif_info_tag *vif_mgmt_next(struct vif_info_tag *vif)
{
    return((struct vif_info_tag *)co_list_next(&vif->list_hdr));
}

/**
 ****************************************************************************************
 * @brief Set BSSID mask according to BSSID index and Maximum BSSID indicator
 *
 * The function checks if only one VIF is registered, of STA type and connected to a
 * nonTransmitted BSSID.
 ****************************************************************************************
 */
void vif_mgmt_set_bssid_mask(void);

/**
 ****************************************************************************************
 * @brief Initialize all the entries of the station table.
 ****************************************************************************************
 */
void vif_mgmt_init(void);

#if (NX_TX_FRAME)
/**
 ****************************************************************************************
 * @brief Tries to send all the internal frames postponed during a reset procedure once
 * the TX path has been fully reset.
 ****************************************************************************************
 */
void vif_mgmt_reset(void);
#endif //(NX_TX_FRAME)

/**
 ****************************************************************************************
 * @brief Allocates new VIF entry, initializes it and fills it with information passed
 * as parameters.
 *
 * @param[in]  mac_addr  MAC address of the VIF
 * @param[in]  vif_type  VIF type
 * @param[in]  p2p       P2P Interface
 * @param[out] vif_idx   Pointer to the allocated VIF index
 *
 * @return  CO_OK if allocation was successful, CO_FAIL otherwise.
 ****************************************************************************************
 */
uint8_t vif_mgmt_register(struct mac_addr const *mac_addr,
                          uint8_t  vif_type,
                          bool p2p,
                          uint8_t *vif_idx);

/**
 ****************************************************************************************
 * @brief Deletes the VIF entry
 *
 * @param[in]  vif_idx   Index of the VIF to be deleted.
 ****************************************************************************************
 */
void vif_mgmt_unregister(uint8_t vif_idx);

#if (NX_UMAC_PRESENT)
/**
 ****************************************************************************************
 * @brief Add a key for a VIF
 *
 * @param[in]  param      Pointer to the key parameters.
 * @param[in]  hw_key_idx Index of the key in the HW key RAM
 ****************************************************************************************
 */
void vif_mgmt_add_key(struct mm_key_add_req const *param, uint8_t hw_key_idx);

/**
 ****************************************************************************************
 * @brief delete a key for a VIF
 *
 * @param[in]  vif      Pointer to the VIF structure
 * @param[in]  keyid    Key index
 ****************************************************************************************
 */
void vif_mgmt_del_key(struct vif_info_tag *vif, uint8_t keyid);

/**
 ****************************************************************************************
 * @brief Return the type of the VIF the index of which is passed as parameter
 *
 * @param[in]  vif_idx      Index of the VIF
 *
 * @return The VIF type
 ****************************************************************************************
 */
__INLINE uint8_t vif_mgmt_get_type(uint8_t vif_idx)
{
    return (vif_info_tab[vif_idx].type);
}

#if BK_MAC
__INLINE uint8_t vif_mgmt_is_ap_inface(uint8_t vif_idx)
{
    return (vif_info_tab[vif_idx].type == VIF_AP);
}

__INLINE uint8_t vif_mgmt_is_sta_inface(uint8_t vif_idx)
{
    return (vif_info_tab[vif_idx].type == VIF_STA);
}
#endif

/**
 ****************************************************************************************
 * @brief Return IDX of a STA linked to the vif
 *
 * @param[in]  vif       Pointer to the VIF structure
 * @param[in]  sta_addr  MAC address of the STA
 *
 * @return Index of the STA, and INVALID_STA_IDX if there no such STA linked to this VIF.
 ****************************************************************************************
 */
uint8_t vif_mgmt_get_staid(const struct vif_info_tag *vif, const struct mac_addr *sta_addr);

#endif //(NX_UMAC_PRESENT)

#if NX_TX_FRAME
/**
 ****************************************************************************************
 * @brief Return band of the channel used by a vif
 *
 * If channel contexts are not used, return band of the current channel.
 *
 * @param[in]  vif Pointer to the vif structure
 * @return The band index, and PHY_BAND_MAX if no channel is associated to the vif
 ****************************************************************************************
 */
__INLINE uint8_t vif_mgmt_get_band(struct vif_info_tag *vif)
{
    #if (!NX_CHNL_CTXT)
    struct phy_channel_info phy_info;
    phy_get_channel(&phy_info, PHY_PRIM);
    return PHY_INFO_BAND(phy_info);
    #else
    if (vif->chan_ctxt)
        return vif->chan_ctxt->channel.band;
    else
        return PHY_BAND_MAX;
    #endif
}

/**
 ****************************************************************************************
 * @brief Return TX type to used to send frame on this vif
 *
 * Select tx type based on the band used by the channel associated to the vif.
 * If channel contexts are not used, select type based on band of current channel.
 *
 * @param[in]  vif Pointer to the vif structure
 * @return @ref TX_DEFAULT_24G or @ref TX_DEFAULT_5G
 ****************************************************************************************
 */
__INLINE int vif_mgmt_get_txtype(struct vif_info_tag *vif)
{
    uint8_t band = vif_mgmt_get_band(vif);
    ASSERT_ERR(band != PHY_BAND_MAX);

    #if (NX_P2P)
    if (vif->p2p)
        // If P2P interface, 11b rates are forbidden
        return TX_DEFAULT_5G;
    #endif //(NX_P2P)

    if (band == PHY_BAND_2G4)
        return TX_DEFAULT_24G;
    else
        return TX_DEFAULT_5G;
}
#endif // NX_TX_FRAME

#if (RW_BFMER_EN || NX_UMAC_PRESENT)
/**
 ****************************************************************************************
 * @brief Get VIF MAC address
 *
 * @param[in] vif_idx VIF index
 * @return MAC address of the VIF
 ****************************************************************************************
 */
__INLINE struct mac_addr *vif_mgmt_get_addr(uint8_t vif_idx)
{
    return (&vif_info_tab[vif_idx].mac_addr);
}
#endif //(RW_BFMER_EN || NX_UMAC_PRESENT)

#if (NX_TX_FRAME)
/**
 ****************************************************************************************
 * @brief Try to send TX frame whose transmission had been postponed due to bad current
 * channel and peer absence.
 *
 * @param[in] vif           Pointer to the VIF environment structure
 ****************************************************************************************
 */
void vif_mgmt_send_postponed_frame(struct vif_info_tag *vif);
#endif //(NX_TX_FRAME)

#if (NX_CHNL_CTXT || NX_P2P)
/**
 ****************************************************************************************
 * @brief Program TimeOut for beacon reception.
 *
 * This function programs a timer that will expire if a beacon is not received by the VIF
 * after @ref VIF_MGMT_BCN_TO_DUR. Only used for STA VIF.
 *
 * @param[in] vif         Pointer to the VIF structure
 ****************************************************************************************
 */
void vif_mgmt_bcn_to_prog(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Update Channel status after reception of BEACON.
 *
 * This function is called after reception of BEACON by a STA interface. If will clear
 * the timer started by @ref vif_mgmt_bcn_to_prog and called the corresponding callback
 * @ref vif_mgmt_bcn_to_evt.
 * If Power Save is enabled but the VIF is not currently in power save mode, this function
 * will do nothing and callback will only be called when timer expires.
 *
 * @param[in] vif         Pointer to the VIF structure
 ****************************************************************************************
 */
void vif_mgmt_bcn_recv(struct vif_info_tag *vif);
#endif //(NX_CHNL_CTXT || NX_P2P)

/**
 ****************************************************************************************
 * @brief Set the Beacon Interval value for an AP or a Mesh Point VIF (should have been
 * verified before any call to this function).
 * It is also considered that the driver provides only beacon intervals whose value is a
 * multiple of a previously provided beacon interval.
 *
 * @param[in]   vif             VIF Entry for which the Beacon Interval must be set
 * @param[in]   bcn_int         Beacon Interval
 ****************************************************************************************
 */
void vif_mgmt_set_ap_bcn_int(struct vif_info_tag *vif, uint16_t bcn_int);

#if NX_UMAC_PRESENT
/**
 ****************************************************************************************
 * @brief Execute Channel switch at the end of a CSA
 *
 * This function will take care of changing the channel for a specific vif (that is
 * already active with a context channel).
 * It will first unlink the current channel ctx, create a new channel ctxt (channel
 * configuration is read from vif_info_tag.csa_channel) and link this new context.
 * Once done:
 * For STA: restart timer/counter for beacon
 * For AP: stop beacon transmission until driver send the updated beacon
 *
 * @param[in] vif           Pointer to the VIF structure
 *
 ****************************************************************************************
 */
void vif_mgmt_switch_channel(struct vif_info_tag *vif);
#endif

/**
 ****************************************************************************************
 * @brief Check if only one VIF is registered, and is of STA type.
 *
 * @return A pointer to the VIF structure if available, NULL otherwise
 ****************************************************************************************
 */
struct vif_info_tag *vif_mgmt_get_single_sta_vif(void);

#if BK_MAC
uint32_t vif_mgmt_first_used_staid(uint32_t *id);
uint8_t vif_mgmt_get_index_by_mac(const uint8_t *mac);
struct vif_info_tag *vif_mgmt_get_entry(uint8_t id);
struct vif_info_tag *vif_get_entry_by_mac_addr(struct mac_addr *addr);
extern struct vif_info_tag* get_appoint_type_vif(int type);
struct vif_info_tag *vif_mgmt_get_first_ap_entry(void);
uint32_t vif_mgmt_get_first_ap_index(void);
#endif

/// @}

#endif // _VIF_MGMT_H_

