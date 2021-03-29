/**
 ****************************************************************************************
 *
 * @file tx_swdesc.h
 *
 * @brief Definition of Tx SW descriptors
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _TX_SWDESC_H_
#define _TX_SWDESC_H_

/**
 ****************************************************************************************
 * @defgroup TX TX
 * @ingroup LMAC
 * @brief Definition of TX Path functionality
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup TX_SWDESC TX_SWDESC
 * @ingroup TX
 * @brief Definition and management of Tx SW descriptors
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
#include "co_bool.h"

#include "rwnx_config.h"
#include "mac.h"
#include "hal_desc.h"
#if NX_UMAC_PRESENT
#include "txu_cntrl.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

#if (NX_AMPDU_TX)
///AMDPU delimiter length in bytes
#define DELIMITER_LEN             4
#endif //(NX_AMPDU_TX)

/// Extra TX flags
enum tx_swdesc_umac_flags
{
    /// Singleton frame can be transmitted using beamforming
    TX_SWDESC_UMAC_BEAMFORM_BIT = CO_BIT(0),
    /// Singleton trial frame can be transmitted using beamforming
    TX_SWDESC_UMAC_TRIAL_BEAMFORM_BIT = CO_BIT(1),
    /// Trial frame can be transmitted using STBC
    TX_SWDESC_UMAC_TRIAL_STBC_BIT = CO_BIT(2),
    /// Singleton frame can be transmitted with +HTC field
    TX_SWDESC_UMAC_HTC_BIT = CO_BIT(3),
    /// Singleton trial frame can be transmitted with +HTC field
    TX_SWDESC_UMAC_TRIAL_HTC_BIT = CO_BIT(4),
    /// Flag indicating that this is a UAPSD trigger frame
    TX_SWDESC_UMAC_CNTRL_UAPSD_TRIGGER_BIT = CO_BIT(5)
};

/// TXdesc status flags
enum txdesc_status
{
    /// Set when initial background handling on a TX descriptor is completed
    TXDESC_PUSHED_BIT = CO_BIT(0),
    /// Set when enough data has been programmed for download to push it to MACHW
    /// Only set for MPDU out of A-MPDU. For A-MPDU @ref tx_agg_desc.status
    TXDESC_BUFFER_PUSHED_BIT = CO_BIT(1),
    /// Set when payload has been downloaded
    /// Only set for MPDU out of A-MPDU. For A-MPDU @ref tx_agg_desc.status
    TXDESC_BUFFER_READY_BIT = CO_BIT(2),
};

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */

struct tx_agg_desc;
struct txl_buffer_tag;
struct tx_dmadesc;
#if BK_MAC
typedef void (*mgmt_tx_cb_t)(void *param);
#endif

/// Control descriptor containing information about the transmission rates to use, the
/// Beamforming/HTC/STBC flags, etc.
struct txl_buffer_control
{
    union
    {
        /// Policy table (filled by the upper MAC)
        struct tx_policy_tbl policy_tbl;
        /// Compressed policy table (used for MU-MIMO if enabled)
        struct tx_compressed_policy_tbl comp_pol_tbl;
    };
    /// MAC Control Information field (filled by the upper MAC)
    uint32_t mac_control_info;
    /// PHY Control Information field (filled by the upper MAC)
    uint32_t phy_control_info;
    #if NX_UMAC_PRESENT
    /**
     * Flags from UMAC which do not fit with tx_hd.macctrlinfo2 format
     * (see @ref tx_swdesc_umac_flags)
     */
    uint32_t tx_flags;
    #endif // NX_UMAC_PRESENT
    #if !NX_UMAC_PRESENT
    /// Status field
    uint32_t status;
    #endif
};

/// Descriptor filled by the Host
struct hostdesc
{
    #if NX_FULLY_HOSTED
    /// Pointer to the network stack buffer structure
    void *buf;
    #endif
    #if NX_AMSDU_TX
#if BK_MAC
    uint32_t orig_addr[NX_TX_PAYLOAD_MAX];
#endif
    /// Pointers to packet payloads
    uint32_t packet_addr[NX_TX_PAYLOAD_MAX];
    /// Sizes of the MPDU/MSDU payloads
    uint16_t packet_len[NX_TX_PAYLOAD_MAX];
    /// Number of payloads forming the MPDU
    uint8_t packet_cnt;
    #else
    /// Pointer to packet payload
    uint32_t packet_addr;
    /// Size of the payload
    uint16_t packet_len;
    #endif //(NX_AMSDU_TX)

    #if NX_UMAC_PRESENT
    #if !NX_FULLY_HOSTED
    /// Address of the status descriptor in host memory (used for confirmation upload)
    uint32_t status_desc_addr;
    #endif //!NX_FULLY_HOSTED
    /// Destination Address
    struct mac_addr eth_dest_addr;
    /// Source Address
    struct mac_addr eth_src_addr;
    /// Ethernet Type
    uint16_t ethertype;
    /// PN that was used for first transmission of this MPDU
    uint16_t pn[4];
    /// Sequence Number used for first transmission of this MPDU
    uint16_t sn;
    /// Timestamp of first transmission of this MPDU
    uint16_t timestamp;
    #else
    #if NX_AMPDU_TX
    /// Sequence Number for AMPDU MPDUs - for quick check if it's allowed within window
    uint16_t sn;
    #endif
    /// Padding between the buffer control structure and the MPDU in host memory
    uint8_t padding;
    #endif
    /// Packet TID
    uint8_t tid;
    /// VIF index
    uint8_t vif_idx;
    /// Station Id (0xFF if station is unknown)
    uint8_t staid;
    #if RW_MUMIMO_TX_EN
    /// MU-MIMO information (GroupId and User Position in the group) - The GroupId
    /// is located on bits 0-5 and the User Position on bits 6-7. The GroupId value is set
    /// to 63 if MU-MIMO shall not be used
    uint8_t mumimo_info;
    #endif

    #if BK_MAC && !NX_FULLY_HOSTED
    mgmt_tx_cb_t callback;
    void *param;

    void *msdu_node;
    #endif
    #if NX_UMAC_PRESENT
    /// TX flags
    uint16_t flags;
    #endif
#if BK_MAC
    //push to tx descriptor immediate
    uint8_t no_wait;
#endif
    #if NX_FULLY_HOSTED
    /// TX confirmation callback (Only used for mgmt frame)
    void *cfm_cb;
    /// TX confirmation callback argument
    void *cfm_cb_arg;
    /// TX confirmation info
    struct tx_cfm_tag cfm;
    /// RX environment structure that is resent
    struct fhost_rx_buf_tag *buf_rx;
    #endif
};

/// Descriptor filled by the UMAC
struct umacdesc
{
    #if !NX_UMAC_PRESENT
    /**
     * Flags from UMAC which do not fit with tx_hd.macctrlinfo2 format
     * (see @ref tx_swdesc_umac_flags)
     */
    uint16_t tx_flags;
    #endif // !NX_UMAC_PRESENT
    #if NX_AMPDU_TX
    ///First Sequence Number of the BlockAck window
    uint16_t sn_win;
    /// Flags from UMAC (match tx_hd.macctrlinfo2 format)
    uint32_t flags;
    /// PHY related flags field - nss, rate, GI type, BW type - filled by driver
    uint32_t phy_flags;
    #endif //(NX_AMPDU_TX)
    #if (NX_UMAC_PRESENT)
    /// Pointer to the buffer control to use
    struct txl_buffer_control *buf_control;
    /// Length of the payload
    uint16_t payl_len;
    /// MAC Header length
    uint8_t machead_len;
    /// Total header length (MAC + IV + EIV + 802.2 Header)
    uint8_t head_len;
    /// 802.2 header length
    uint8_t hdr_len_802_2;
    /// Tail length (MIC + ICV)
    uint8_t tail_len;
    /// Rate control information
    uint8_t rc_control;
    #if (RW_MESH_EN)
    /// Indicate if Mesh Control field is present in the sent frame
    bool has_mesh_ctrl;
    /// Number of External Addresses to be inserted in the Mesh Control field
    uint8_t nb_ext_addr;
    /// Index of the path to be used
    uint8_t path_idx;
    /// Index of the proxy information to be used
    uint8_t proxy_idx;
    #endif //(RW_MESH_EN)
    #endif //(NX_UMAC_PRESENT)
};

/// Descriptor filled by the LMAC
struct lmacdesc
{
    /// Pointer to the optional Aggregation descriptor
    struct tx_agg_desc *agg_desc;
    #if NX_AMSDU_TX
    /// Pointer to the embedded buffers
    struct txl_buffer_tag *buffer[NX_TX_PAYLOAD_MAX];
    #else
    /// Pointer to the embedded buffer
    struct txl_buffer_tag *buffer;
    #endif //(NX_AMSDU_TX)
    #if (RW_BFMER_EN)
    /// BFR Node used for the transmission
    struct bfr_mem_node *bfr_node;
    #endif //(RW_BFMER_EN)
    /// Pointer to the TX confirmation structure
    struct tx_hw_desc *hw_desc;
    #if !NX_FULLY_HOSTED
    /// Buffer length already available (i.e. allocated) for this descriptor
    uint16_t available_len;
    #endif
    /// Status flags (@ref txdesc_status)
    uint16_t status;
};

/// LMAC Tx Descriptor
#if BK_MAC
#define TXDESC_STA_IDLE                 0
#define TXDESC_STA_USED                 1
#endif

struct txdesc
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// Information provided by Host
    struct hostdesc host;
    /// Information provided by UMAC
    struct umacdesc umac;
    /// Information provided by LMAC
    struct lmacdesc lmac;

#if BK_MAC
    uint32_t status;
#endif
};

/// Description of the TX API
struct txdesc_api
{
    /// Information provided by Host
    struct hostdesc host;
    #if (!NX_UMAC_PRESENT)
    /// Information provided by UMAC
    struct umacdesc umac;
    #endif
};


/// Descriptor used for Host/Emb TX frame information exchange
struct txdesc_host
{
    /// Flag indicating if the TX descriptor is ready (different from 0) or not (equal to 0)
    uint32_t ready;

    /// API of the embedded part
    struct txdesc_api api;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
#if !NX_FULLY_HOSTED
/// Pointer to the TX descriptors arrays
extern struct txdesc *txdesc_array[NX_TXQ_CNT][RW_USER_MAX];
#endif

/// Number of TX descriptors per queue
extern const int nx_txdesc_cnt[NX_TXQ_CNT];

/// Maximum number of users per queue
extern const int nx_txuser_cnt[NX_TXQ_CNT];
#if BK_MAC
extern uint32_t tx_desc_idx[NX_TXQ_CNT];
extern const int nx_txdesc_cnt_msk[NX_TXQ_CNT];
#endif

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/// @name External API
/// @{

/**
 ****************************************************************************************
 * @brief Initializes the Tx descriptor pool
 *
 ****************************************************************************************
 */
void tx_txdesc_init(void);

/**
 ****************************************************************************************
 * @brief Return a boolean indicating if a MPDU is split accross several host buffers (e.g.
 * A-MSDU)
 * @param[in] txdesc  Txdesc pointer to access its packet count.
 * @return Boolean True if MPDU is split and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_int_frame(struct txdesc *txdesc)
{
    #if NX_AMSDU_TX
    return (txdesc->host.packet_addr[0] == 0);
    #else
    return (txdesc->host.packet_addr == 0);
    #endif
}

#if NX_AMSDU_TX
/**
 ****************************************************************************************
 * @brief Return a boolean indicating if a MPDU is split across several host buffers (e.g.
 * A-MSDU)
 * @param[in] txdesc  Txdesc pointer to access its packet count.
 * @return Boolean True if MPDU is split and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_split(struct txdesc *txdesc)
{
    return (txdesc->host.packet_cnt > 1);
}
#endif

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being first in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is first in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_qos_data(struct txdesc *txdesc)
{
    return (txdesc->host.tid != 0xFF);
}

/**
 ****************************************************************************************
 * @brief Return if a frame pointed by the given TX descriptor can include a +HTC if
 * transmitted as a singleton MPDU
 * @param[in] txdesc TX Descriptor to be checked
 * @return true if the frame could include a +HTC when sent as a singleton MPDU, false
 * otherwise
 ****************************************************************************************
 */
__INLINE bool is_htc_sglt_allowed(struct txdesc *txdesc)
{
    #if NX_UMAC_PRESENT
    // Check if the frame can be beamformed as a unicast
    if (txdesc->host.flags & TXU_CNTRL_RC_TRIAL)
    {
        return ((txdesc->umac.buf_control->tx_flags & TX_SWDESC_UMAC_TRIAL_HTC_BIT) != 0);
    }
    else
    {
        return ((txdesc->umac.buf_control->tx_flags & TX_SWDESC_UMAC_HTC_BIT) != 0);
    }
    #else
    return ((txdesc->umac.tx_flags & TX_SWDESC_UMAC_HTC_BIT) != 0);
    #endif
}

/**
 ****************************************************************************************
 * @brief Return the TX descriptor following the one passed as parameter.
 * @param[in] txdesc  Tx descriptor pointer.
 * @return The pointer to the next TX descriptor
 ****************************************************************************************
 */
__INLINE struct txdesc *tx_desc_next(struct txdesc *txdesc)
{
    return (struct txdesc *)co_list_next(&txdesc->list_hdr);
}

/**
 ****************************************************************************************
 * @brief Return the THD attached to the descriptor passed as parameter.
 * @param[in] txdesc  Tx descriptor pointer.
 * @return The pointer to the THD attached to the descriptor
 ****************************************************************************************
 */
__INLINE struct tx_hd *tx_desc_thd(struct txdesc *txdesc)
{
    return &txdesc->lmac.hw_desc->thd;
}


/// @}
/// @}

#endif // _CO_SWDESC_H_
