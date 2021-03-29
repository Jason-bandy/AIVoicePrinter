/**
 ****************************************************************************************
 *
 * @file txl_cntrl.h
 *
 * @brief LMAC TxPath definitions.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _TXL_CNTRL_H_
#define _TXL_CNTRL_H_


/**
 *****************************************************************************************
 * @defgroup TX_CNTRL TX_CNTRL
 * @ingroup TX
 * @brief Controller of TX data path.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"

// for AC_MAX
#include "mac.h"
// for co_status_t
#include "co_status.h"
// for co_list
#include "co_list.h"
#include "tx_swdesc.h"
#include "ke_event.h"
#include "hal_machw.h"

#include "sta_mgmt.h"

#if NX_AMPDU_TX
#include "tx_swdesc.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */
/// AC0 queue default timeout
#define TX_AC0_TIMEOUT      5000  ///< Background - 5s
/// AC1 queue default timeout
#define TX_AC1_TIMEOUT      2000  ///< Best effort - 2s
/// AC2 queue default timeout
#define TX_AC2_TIMEOUT      400   ///< Video - 400ms
/// AC3 queue default timeout
#define TX_AC3_TIMEOUT      200   ///< Voice - 200ms
/// BCN queue timeout
#define TX_BCN_TIMEOUT      50    ///< Beacon - 50ms

/// Tx trigger descriptor status check states
enum
{
    /// State for checking MPDU THD done status
    THD_CHK_STATE,
    /// State for checking BAR THD done status
    BAR_THD_CHK_STATE,
    #if NX_HE
    /// State for checking A-THD done status
    ATHD_CHK_STATE,
    #endif
};


#if NX_AMPDU_TX
/// Aggregation process status bits
enum
{
    /// Status indicating that the aggregation formatting is done
    AGG_FORMATTED = 0x01,
    /// Status indicating that enough data of the A-MPDU has been downloaded
    AGG_DOWNLOADED = 0x02,
    /// Status indicating that enough data of the A-MPDU is allocated to be chained
    AGG_ALLOC = 0x04,
    /// Status indicating that the A-MPDU was closed in a trigger frame interrupt
    AGG_CLOSED_IN_HE_TB = 0x08,
    /// Status indicating that the A-MPDU can be confirmed to upper MAC
    AGG_DONE = 0x10,
    /// Status indicating that a BlockAck has been received for this frame
    AGG_BA_RECEIVED = 0x20,
    /// Status indicating that the received BlockAck is valid
    AGG_BA_VALID = 0x40,
    /// Status indicating that this aggregate descriptor is an intermediate one, i.e. not
    /// the final aggregate descriptor of a MU-MIMO PPDU
    AGG_INT = 0x80,
    /// Status indicating that this aggregate descriptor is part of a MU-MIMO PPDU
    AGG_MU = 0x100,
    /// Status indicating that this aggregate descriptor is used for a HE TB PPDU
    AGG_TB = 0x200,
    /// Status indicating that the aggregate descriptor belongs to a split A-MPDU, and
    /// can be re-used for the A-MPDU re-configuration
    AGG_REUSABLE = 0x400,
};
#endif

/// Status of a packet pushed for aggregation, when MU-MIMO is enabled
enum
{
    /// Packet will be sent in a SU PPDU
    SU_PACKET,
    #if RW_MUMIMO_TX_EN
    /// Packet will be sent in a MU PPDU
    MU_PACKET,
    /// Packet cannot be handled immediately, user position is closed for now
    MU_PAUSED,
    /// MU-MIMO check processing shall be restarted after closure of pending MU-MIMO PPDU
    MU_RESTART_CHECK,
    #endif
};

/*
 * MACROS
 ****************************************************************************************
 */
/// Conversion from Access Category to corresponding DMA LLI
#define TX_AC2LLI(ac) ((ac) + IPC_DMA_LLI_DATA_AC0_TX)

/// Conversion from DMA LLI to corresponding Access Category
#define TX_LLI2AC(idx) ((idx) - IPC_DMA_LLI_DATA_AC0_TX)

/// Conversion from Access Category to corresponding TX timer
#define TX_AC2TIMER(ac) ((ac) + HAL_AC0_TIMER)

/// Conversion from Access Category to corresponding bridge DMA control field
#define TX_LLICTRL(ac, irqenable)                                                        \
    IPC_DMA_LLI_COUNTER_EN|(TX_AC2LLI(ac) << IPC_DMA_LLI_COUNTER_POS) |                  \
    ((irqenable)?(IPC_DMA_LLI_IRQ_EN|(TX_AC2LLI(ac) << IPC_DMA_LLI_IRQ_POS)) : 0)

/// Index of the beacon queue
#define AC_BCN         AC_MAX

#if RW_MUMIMO_TX_EN
/// Default TX antenna mask
#define MU_USER_MASK     (CO_BIT(RW_USER_MAX) - 1)
#endif

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
#if NX_AMPDU_TX
/// Structure containing the information about the current A-MPDU being built
struct txl_agg_build_tag
{
    /// Pointer to the allocated aggregation descriptor
    struct tx_agg_desc *desc;
    /// Pointer to the first TX descriptor of the A-MPDU
    struct txdesc *txdesc_first;
    /// Pointer to the last TX descriptor of the A-MPDU
    struct txdesc *txdesc_last;
    /// Maximum length of the A-MPDU
    uint32_t max_len[NX_BW_LEN_STEPS];
    /// Minimum number of bytes per A-MPDU sub frame (to fulfill the spacing requirement)
    uint16_t mmss_bytes;
    /// Number of blank delimiters set in current desc to ensure minimal spacing from previous MPDU
    uint16_t nb_delims;
    /// Maximum number of MPDUs inside the A-MPDU
    uint8_t max_cnt;
    /// Current number of MPDUs inside the A-MPDU
    uint8_t curr_cnt;
    /// BW of transmission of the A-MPDU
    uint8_t bw;
    #if NX_BW_LEN_ADAPT
    /// Current step of per-bandwidth length
    uint8_t bw_idx;
    #endif
};
#endif

#if RW_MUMIMO_TX_EN
/// Structure containing the information about the MU-MIMO PPDU being built
struct txl_mumimo_build_info_tag
{
    /// Lists of TX descriptors attached to this MU-MIMO PPDU
    struct co_list tx[RW_USER_MAX];
    /// TX descriptor saved while the corresponding user position queue is paused
    struct txdesc *txdesc[RW_USER_MAX];
    /// Rate information of the A-MPDU under construction
    uint32_t rateinfo[RW_USER_MAX];
    /// Length of the different A-MPDUs of the MU-MIMO PPDU being built
    uint32_t length[RW_USER_MAX];
    /// PHY flags (GI, BW) used for this MU-PPDU
    uint32_t phy_flags;
    /// Number of users present in the MU-MIMO PPDU
    uint8_t nb_users;
    /// Bitfield showing which user positions are already involved in the MU-MIMO PPDU
    uint8_t users;
    /// Bitfield showing which user positions are still accepting new MPDUs
    uint8_t open;
    /// GroupId of this MU-MIMO transmission
    uint8_t group_id;
    /// First user position to be handled when unblocking the queues
    uint8_t first_user_pos;
};
#endif

/// Structure of a per-AC Tx list
struct txl_list
{
    /// Pointer to the last frame exchange chained to HW
    struct tx_hd *last_frame_exch;
    /// List of descriptors being transmitted over WLAN interface
    struct co_list transmitting[RW_USER_MAX];
    /// Pointer to the first descriptor for which the buffer allocation is required
    struct txdesc *first_to_download[RW_USER_MAX];
    /// Counter of bridge DMA transfers done
    uint16_t bridgedmacnt;
    /// State for done status check of THDs at TX trigger interrupt
    uint8_t chk_state;
    #if NX_AMPDU_TX
    ///List of aggregation control structures, directly linked to AMPDU THDs in pool
    struct co_list aggregates;
    /// Pointer to the A-MPDU descriptor for which we are currently waiting for the BA
    struct tx_agg_desc *agg_desc;
    /// Pointer to the previous A-MPDU
    struct tx_agg_desc *agg_desc_prev;
    /// Current aggregates properties
    struct txl_agg_build_tag agg[RW_USER_MAX];
    #if RW_MUMIMO_TX_EN
    /// MU-MIMO parameters of the list
    struct txl_mumimo_build_info_tag mumimo;
    #endif
    /// Number of PPDUs currently in the TX queue
    uint8_t ppdu_cnt;
    #endif
    #if NX_AMSDU_TX
    /// Pointer to the last buffer descriptor of the queue
    struct tx_pbd *last_pbd[RW_USER_MAX];
    ///Index of the payload to download from the current TX descriptor
    uint8_t dwnld_index[RW_USER_MAX];
    ///Index of the payload for which a TX confirmation is awaited from the current TX descriptor
    uint8_t tx_index[RW_USER_MAX];
    #endif

};

/// Context of the Tx Control block
struct txl_cntrl_env_tag
{
    /// Transmission list per AC
    struct txl_list txlist[NX_TXQ_CNT];

    #if NX_POWERSAVE
    /// Number of packets currently in the TX path
    uint32_t pck_cnt;
    #endif
    #if RW_MUMIMO_TX_EN
    /// MU-MIMO PPDUs currently chained to HW
    uint16_t mumimo_ppdu_cnt;
    #endif

    /// Non-QoS sequence number
    uint16_t seqnbr;
    #if (NX_TX_FRAME)
    /// Indicate if we are resetting the TX path
    bool reset;
    #endif //(NX_TX_FRAME)
};

// Forward declaration
struct vif_info_tag;

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
/// Table mapping the TX prepare event bit to the queue index
extern const uint32_t txl_prep_evt_bit[NX_TXQ_CNT];

/// Tx Control context variable
extern struct txl_cntrl_env_tag txl_cntrl_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/** @name External API
 @{ */


#if NX_POWERSAVE
/**
 ****************************************************************************************
 * @brief Get the status of TX path, i.e if some packets are currently queued for
 * transmission. It is used by the PS module to know if the TX path allows sleeping or
 * not.
 *
 * @return true if the TX path allows sleeping (i.e. no packets queued), and false
 * otherwise.
 *
 ****************************************************************************************
 */
__INLINE bool txl_sleep_check(void)
{
    return (txl_cntrl_env.pck_cnt == 0);
}
#endif

/**
 ****************************************************************************************
 * @brief Get the current sequence control field to be used for non-QoS frames.
 * The sequence control is also incremented by this function.
 *
 * @return The sequence control field to be used
 *
 ****************************************************************************************
 */
__INLINE uint16_t txl_get_seq_ctrl(void)
{
    // Increment the sequence number
    txl_cntrl_env.seqnbr++;

    // Build the sequence control
    return (txl_cntrl_env.seqnbr << MAC_SEQCTRL_NUM_OFT);
}



/**
 ****************************************************************************************
 * @brief Initialize the LMAC TX path.
 *
 * This primitive initializes all the elements used in the TX path.
 *
 ****************************************************************************************
 */
void txl_cntrl_init(void);

/**
 ****************************************************************************************
 * @brief TX path reset function.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It flushes all the packets currently in the TX path and exits when all of them
 * have been confirmed to the driver
 ****************************************************************************************
 */
void txl_reset(void);

#if (NX_TX_FRAME)
/**
 ****************************************************************************************
 * @brief Check if a packet can be transmitted on a given VIF. Various checks are made
 * in this function, e.g. on the current channel, the PS state of the peer device in case
 * of P2P, etc.
 *
 * @param[in] vif         The pointer to the VIF we have to check.
 *
 * @return true if the transmission is allowed for this VIF, false otherwise.
 ****************************************************************************************
 */
bool txl_cntrl_tx_check(struct vif_info_tag *vif);
#endif //(NX_CHNL_CTXT || NX_P2P)

/**
 ****************************************************************************************
 * @brief Push a Tx descriptor in an RA/TID queue
 *
 * This primitive allocates the different descriptors required for the MPDU described by
 * txdesc, updates it with the required information and pushes it into the queue.
 *
 * @param[in] txdesc          Descriptor of the packet to be pushed into the Tx queue
 * @param[in] access_category Access category on which the packet has to be pushed
 *
 * @return true if the IPC queue has to be paused, false otherwise.
 *
 ****************************************************************************************
 */
bool txl_cntrl_push(struct txdesc *txdesc, uint8_t access_category);

#if NX_TX_FRAME
/**
 ****************************************************************************************
 * @brief Push an internal frame Tx descriptor for transmission
 *
 * @param[in] txdesc          Descriptor of the packet to be pushed into the Tx queue
 * @param[in] access_category Access category on which the frame is sent
 *
 * @return true if the internal frame was correctly pushed, false otherwise.
 *
 ****************************************************************************************
 */
bool txl_cntrl_push_int(struct txdesc *txdesc, uint8_t access_category);
#endif


/**
 ****************************************************************************************
 * @brief Function to be called on transmission trigger IRQ
 *
 * This primitive will chain new frame exchanges to HW, confirm transmitted ones and
 * prepare subsequent transmission.
 * The function retrieves the access category which generated the interrupt from the
 * MAC HW status register.
 *
 ****************************************************************************************
 */
void txl_transmit_trigger(void);

/**
 ****************************************************************************************
 * @brief Function to be called on TX protocol trigger IRQ (e.g. BW drop or HE TB trigger)
 *
 ****************************************************************************************
 */
void txl_prot_trigger(void);

/**
 ****************************************************************************************
 * @brief Perform operations on payloads that have been transfered from host memory
 *
 * This primitive is called by the interrupt controller ISR. It performs LLC translation
 * and MIC computing if required.
 *
 ****************************************************************************************
 */
#if BK_MAC
void txl_cntrl_dma_isr(uint8_t access_category);
#else
void txl_cntrl_dma_isr(void);
#endif

/**
 ****************************************************************************************
 * @brief Halt the MAC DMA of the specified queue
 *
 * @param[in] access_category  Access category of the queue to be halted
 *
 ****************************************************************************************
 */
void txl_cntrl_halt_ac(uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Flush all the descriptors of the queue passed as parameter
 *
 * @param[in] access_category  Access category of the queue to be flushed
 * @param[in] status           Status to be sent in the confirmation
 *
 ****************************************************************************************
 */
void txl_cntrl_flush_ac(uint8_t access_category, uint32_t status);

/**
 ****************************************************************************************
 * @brief This function returns the pointer to the first TX Header descriptor
 * chained to the MAC HW queue passed as parameter.
 * It is used for error logging when an issue is detected in the LMAC.
 *
 * @param[in] access_category TX queue for which the first THD is retrieved
 * @param[out] thd Pointer to the first tX Header descriptor chained to the MAC HW
 *
 ****************************************************************************************
 */
void txl_current_desc_get(int access_category, struct tx_hd **thd);

#if NX_UMAC_PRESENT
/**
 ****************************************************************************************
 * @brief This function increments the TX path packet counter.
 ****************************************************************************************
 */
void txl_cntrl_inc_pck_cnt(void);
#endif // NX_UMAC_PRESENT

#if !NX_FULLY_HOSTED
/**
 ****************************************************************************************
 * @brief Prepare the transfer of payload from host memory to emb memory
 *
 * This primitive first allocates a buffer of the required size, and then updates the
 * bridge descriptor according to the allocated buffer.
 *
 * @param[in] txdesc           Descriptor of the packet to be pushed into the Tx list
 * @param[in] access_category  Access category where to push the descriptor
 * @param[in] user_idx         User index (for MU-MIMO TX only, 0 otherwise)
 *
 * @return The status of the transfer (TRUE: programmed, FALSE: not programmed due to buffer full)
 *
 ****************************************************************************************
 */
bool txl_payload_alloc(struct txdesc *txdesc, uint8_t access_category, uint8_t user_idx);
#endif // !NX_FULLY_HOSTED

/**
 ****************************************************************************************
 * @brief Update the header pointer and set the new head bit for the specified access
 * category.
 *
 * @param[in] desc              Pointer to the header descriptor
 * @param[in] access_category   Access category
 *
 ****************************************************************************************
 */
void txl_cntrl_newhead(uint32_t desc,
                       uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Chain a new frame exchange to the MAC HW.
 * Depending if there is already a frame exchange ongoing or not, this function will
 * proceed to a newTail or newHead action.
 *
 * @param[in] first_thd         Pointer to the first THD of the frame exchange
 * @param[in] last_thd          Pointer to the last THD of the frame exchange
 * @param[in] access_category   Access category
 *
 ****************************************************************************************
 */
void txl_frame_exchange_chain(struct tx_hd *first_thd,
                              struct tx_hd *last_thd,
                              uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Set the TX timeout values according to upper layer setting.
 *
 * @param[in] timeout Array of timeout values to be set
 *
 ****************************************************************************************
 */
void txl_cntrl_set_timeout(const uint16_t timeout[]);

#if BK_MAC
uint32_t txl_check_hd_is_current(uint32_t ac, struct txdesc *txd);
#endif

#if !NX_FULLY_HOSTED
/**
 ****************************************************************************************
 * @brief Free a MPDU that is marked as transmitted by the HW, and launch the download of
 * a subsequent MPDU.
 *
 * This function shall be called from a MAC HW transmit trigger interrupt.
 *
 * @param[in] txdesc           Descriptor of the packet to be freed
 * @param[in] access_category  Access category of the descriptor
 * @param[in] user_idx         User index (for MU-MIMO TX only, 0 otherwise)
 ****************************************************************************************
 */
void txl_free_done_mpdu(struct txdesc *txdesc, uint8_t access_category, uint8_t user_idx);

#if NX_AMSDU_TX
/**
 ****************************************************************************************
 * @brief Check transmission status of A-MSDU sub-frames.
 *
 * If the sub-frame is done, then the buffer is freed an a new buffer is allocated for a
 * a subsequent packet.
 *
 * This function shall be called from a MAC HW transmit trigger interrupt.
 *
 * @param[in] txdesc           Descriptor of the packet to be freed
 * @param[in] access_category  Access category of the descriptor
 * @param[in] user_idx         User index (for MU-MIMO TX only, 0 otherwise)
 ****************************************************************************************
 */
void txl_check_done_amsdu_subframe(struct txdesc *txdesc, uint8_t access_category,
                                   uint8_t user_idx);
#endif
#endif

/**
 ****************************************************************************************
 * @brief Initialization of the HW descriptors, after payload allocation/download
 *
 * This primitive configures the HW descriptors prior to transmission. In case of FullMAC
 * it is called at buffer allocation. Otherwise it is caused as soon as the payload is
 * available in memory (e.g immediately in @ref txl_cntrl_push() for FullyHosted, and upon
 * payload download interrupt handling for SoftMAC).
 *
 * @param[in] txdesc          Descriptor of the packet to be pushed into the Tx queue
 * @param[in] access_category TX queue index
 *
 ****************************************************************************************
 */
void txl_hwdesc_config_post(struct txdesc *txdesc, uint8_t access_category);

#if NX_AMSDU_TX
/**
 ****************************************************************************************
 * @brief Check which MCS will be used for a A-MSDU transmission, and if this MCS is lower
 * than the minimum allowed one, update it to this minimal value.
 *
 * @note This function is a temporary workaround to avoid PHY errors in some conditions.
 *
 * @param[in,out] rc Pointer to the ratectrlinfo0 field of the policy table used for TX
 ****************************************************************************************
 */
void txl_correct_amsdu_mcs(uint32_t *rc);
#endif


/// @}
/// @}

#endif // _TXL_CNTRL_H_
