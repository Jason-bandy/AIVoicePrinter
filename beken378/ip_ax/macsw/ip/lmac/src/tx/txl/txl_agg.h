/**
 ****************************************************************************************
 *
 * @file txl_agg.h
 *
 * @brief LMAC TX aggregation related definitions.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _TXL_AGG_H_
#define _TXL_AGG_H_


/**
 *****************************************************************************************
 * @defgroup TX_AGG TX_AGG
 * @ingroup TX
 * @brief TX Aggregation specific functions.
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

#if NX_UMAC_PRESENT
#include "rc.h"
#endif

#if NX_AMPDU_TX
/*
 * DEFINES
 ****************************************************************************************
 */
/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
/// Aggregation descriptor, containing AMPDU THD, BAR descriptor, BAR payload and Policy Table
struct tx_agg_desc
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// PHY related flags field - NSS, rate, GI type, BW type used for the A-MPDU TX
    uint32_t phy_flags;
    /// Current status of the aggregation process
    uint16_t status;
    /// Amount of data currently downloaded for this A-MPDU
    uint16_t available_len;
    /// STA IDX for which AMPDU is being built
    uint8_t sta_idx;
    /// TID for which AMPDU is being built
    uint8_t tid;
    /// Number of "users" currently sharing this descriptor
    /// The descriptor will be freed only when no more SW modules are using it
    uint8_t user_cnt;
    /// AMPDU THD (no payload)
    struct tx_hd a_thd;
    /// BAR THD
    struct tx_hd bar_thd;
    /// BAR payload space (no FCS)
    struct bar_frame bar_payl;
    /// Policy table for explicit BAR frame
    struct tx_policy_tbl bar_pol_tbl;
    /// Pointer to the BA SSC + bitmap
    struct ba_ssc_bitmap *ssc_bitmap;
    /// Pointer to the BA SSC + bitmap
    struct ba_ssc_bitmap_256 ssc_bitmap_tab;
    /// Pointer to the lastly allocated buffer in the A-MPDU
    struct txl_buffer_tag *buf_last;
    /// Pointer to first TX descriptor of the A-MPDU
    struct txdesc *txdesc_first;
    #if NX_HE || NX_BW_LEN_ADAPT
    /// Policy table for A-MPDU
    struct tx_policy_tbl pol_tbl;
    /// Pointer to last TX descriptor of the A-MPDU
    struct txdesc *txdesc_last;
    #endif
    #if NX_BW_LEN_ADAPT
    /// Pointer to TX descriptors finishing a BW step
    struct txdesc *txdesc[NX_BW_LEN_STEPS - 1];
    #endif
    #if RW_MUMIMO_TX_EN
    /// List containing the descriptors to be confirmed once BA is received for this
    /// secondary user
    struct co_list cfm;
    /// Pointer to the primary TX descriptor
    struct tx_agg_desc *prim_agg_desc;
    /// Pointer to the last BAR THD of the MU-MIMO frame exchange
    struct tx_hd *last_bar_thd;
    /// Bitfield indicating which user indexes have still data to download before being
    /// able to be chained to HW
    uint8_t download;
    #endif
    /// List in which the A-MPDU descriptor has to be pushed back after transmission
    struct co_list *free_list;
    /// Rate control information
    uint8_t rc_control;
};

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */
/// Per-ac pools of free aggregation descriptors
extern struct co_list tx_agg_desc_pool[];

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/** @name External API
 @{ */

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being aggregatable or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is aggregatable and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_agg(struct txdesc * txdesc)
{
    return ((txdesc->umac.flags & AMPDU_BIT) == AMPDU_BIT);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being first in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is first in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_first(struct txdesc *txdesc)
{
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_FIRST);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being intermediate in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is intermediate in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_interm(struct txdesc * txdesc)
{
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_INT);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU being last in an AMPDU or not
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU is last in the AMPDU and false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_last(struct txdesc * txdesc)
{
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_LAST);
}

/**
 ****************************************************************************************
 * @brief Return boolean corresponding to MPDU having no position in an AMPDU yet
 * (inherent check for aggregatable too)
 * @param[in] txdesc  Txdesc pointer to access its flags elements.
 * @return Boolean True if MPDU has no position in AMPDU, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mpdu_unpos(struct txdesc * txdesc)
{
    //the definition of Which descriptor with 00 for position may be for the AMPDU THD
    //but we're using the 00 also until MPDU has a position, then for HW the right things
    //will be set in macctrlinfo2
    return ((txdesc->umac.flags & WHICHDESC_MSK) == WHICHDESC_AMPDU_EXTRA);
}

/**
 ****************************************************************************************
 * @brief Set position of MPDU inside an AMPDU
 * @param[in] txdesc  Txdesc pointer to modify its flags elements.
 * @param[in] pos     Which Descriptor value for the position in AMPDU - see hal_desc.h
 * @return Boolean True if MPDU is aggregatable and false otherwise
 ****************************************************************************************
 */
__INLINE void set_mpdu_pos(struct txdesc * txdesc, uint32_t pos)
{
    txdesc->umac.flags = ( txdesc->umac.flags         &
                                 (~(WHICHDESC_MSK & (~AMPDU_BIT))) ) | pos;
}

/**
 ****************************************************************************************
 * @brief Calculate AMPDU subframe length for an MPDU (without blank delimiters)
 * @param[in] thd   Pointer to the THD whose length +Delimiter(4B)+FCS(4)+PAD(0-3) is calculated.
 * @return AMPDU Subframe length.
 ****************************************************************************************
 */
__INLINE uint16_t txl_mpdu_subframe_len(struct tx_hd *thd)
{
    // Align packet length and add front delimiter length
    return (CO_ALIGN4_HI(thd->frmlen) + DELIMITER_LEN);
}

/**
 ****************************************************************************************
 * @brief Whether DCM modulation should be use for an A-MPDU
 * @param[in] agg_desc  A-MPDU descriptor
 * @return true is DCM should be used and false otherwise
 ****************************************************************************************
 */
__INLINE bool use_dcm(struct tx_agg_desc *agg_desc)
{
    #if NX_UMAC_PRESENT
    return (agg_desc->rc_control & RC_DCM_TX_MSK);
    #else
    return false;
    #endif
}

#if RW_MUMIMO_TX_EN
/**
 ****************************************************************************************
 * @brief Return a boolean indicating if the MPDU is part of a MU-MIMO PPDU
 * @param[in] txdesc  Pointer to the MPDU TX descriptor
 * @return true if MPDU is part of a MU-MIMO PPDU, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_in_mumimo_ppdu(struct txdesc * txdesc)
{
    struct tx_agg_desc *agg_desc = txdesc->lmac.agg_desc;

    return ((agg_desc != NULL) && (agg_desc->prim_agg_desc != NULL));
}

/**
 ****************************************************************************************
 * @brief Return a boolean indicating if the MPDU is part of the A-MPDU sent on the
 * primary HW queue.
 * @note This function assumes that the MPDU is sent as part of a MU-MIMO PPDU
 * @param[in] txdesc  Pointer to the MPDU TX descriptor
 * @return true if MPDU is part of the primary MU-MIMO PPDU, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_primary_user(struct txdesc * txdesc)
{
    struct tx_agg_desc *agg_desc = txdesc->lmac.agg_desc;

    return (agg_desc == agg_desc->prim_agg_desc);
}

/**
 ****************************************************************************************
 * @brief Return a boolean indicating if the GroupID passed as parameter is a MU-MIMO one.
 * @param[in] group_id  GroupID to be checked
 * @return true if the GroupId is a MU-MIMO one, false otherwise
 ****************************************************************************************
 */
__INLINE bool is_mumimo_group_id(uint8_t group_id)
{
    return (group_id > 0) && (group_id < 63);
}

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO GroupId from the TX descriptor
 *
 * @param[in] txdesc  Pointer to the TX descriptor
 *
 * @return The MU-MIMO GroupId included in the TX descriptor
 ****************************************************************************************
 */
__INLINE uint8_t get_group_id(struct txdesc * txdesc)
{
    return (txdesc->host.mumimo_info & 0x3F);
}
#endif

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO User Position from the TX descriptor
 *
 * @param[in] txdesc  Pointer to the TX descriptor
 *
 * @return The MU-MIMO GroupId included in the TX descriptor
 ****************************************************************************************
 */
__INLINE uint8_t get_user_pos(struct txdesc * txdesc)
{
    #if RW_MUMIMO_TX_EN
    return (txdesc->host.mumimo_info >> 6);
    #else
    return 0;
    #endif
}

/**
 ****************************************************************************************
 * @brief Get a free aggregation descriptor structure from the free pool.
 *
 * @param[in] access_category Access category
 *
 * @return Pointer to the retrieved free structure.
 ****************************************************************************************
 */
__INLINE struct tx_agg_desc * txl_agg_desc_alloc(uint8_t access_category)
{
    return ((struct tx_agg_desc *)co_list_pop_front(&tx_agg_desc_pool[access_category]));
}

/**
 ****************************************************************************************
 * @brief Free a used aggregation descriptor structure.
 *
 * @param[in] agg_desc  Pointer to the structure to be freed.
 ****************************************************************************************
 */
__INLINE void txl_agg_desc_free(struct tx_agg_desc *agg_desc)
{
    //chain back into free pool
    co_list_push_back(agg_desc->free_list, (struct co_list_hdr *)agg_desc);
}

/**
 ****************************************************************************************
 * @brief Initializes the TX aggregation module
 ****************************************************************************************
 */
void txl_agg_init(void);

/**
 ****************************************************************************************
 * @brief Reset the TX aggregation module.
 *
 * This function is called as part of the recovery procedure
 ****************************************************************************************
 */
void txl_agg_reset(void);

/**
 ****************************************************************************************
 * @brief Try and aggregate txdesc, several checks are made, when they fail the MPDU
 * goes on to be downloaded as a normal singleton MPDU
 *
 * The idea is to check if at least two txdescs in a row
 * - can be aggregated (UMAC set the flag AND txdesc not marked as part of AMPDU)
 * - have same (staid,tid)
 * - that (staid, tid) matches and existing BA agreement
 * - the SN fits within the current bitmap window of the BA agreement
 * - not both AMPDU header descriptors for this STA have been already prepared
 *
 * Then, the next txdescs are looped over to see if they can be aggregated too,
 * if so they are marked and the AMPDU desc length is increased.
 *
 * @param[in] txdesc    Descriptor of the first packet in pending list for an AC, analysed
 *                      for aggregation possibility
 * @param[in] ac        Access category for which creation of an AMPDU is being analysed
 *
 * @return The status of the packet just pushed (@ref SU_PACKET, @ref MU_PACKET or
 * @ref MU_PAUSED if the descriptor has been saved and the IPC needs to be paused).
 ****************************************************************************************
 */
int txl_agg_push_mpdu(struct txdesc *txdesc, uint8_t ac);

/**
 ****************************************************************************************
 * @brief Terminates the non MU-MIMO A-MPDU under construction
 * This function can be called in several cases:
 * - The new TX descriptor pushed is not compliant with the current A-MPDU
 * - The length of the current A-MPDU has reached the limit
 * - The number of MPDUs in the current A-MPDU has reached the limit
 * - The TX queue will become empty and new data shall be programmed for transmission
 * @param[in] ac  Access Category
 ****************************************************************************************
 */
void txl_agg_finish(uint8_t ac);

#if (NX_BW_LEN_ADAPT)
/**
 ****************************************************************************************
 * @brief Handler of the HW BW drop mechanism interrupt
 * Based on the actual BW of transmission, this function cuts the A-MPDU under
 * transmission and chains the following MPDUs as singleton.
 *
 * @param[in]  access_category  Access category of the ongoing transmission
 *
 ****************************************************************************************
 */
void txl_agg_bw_drop_handle(uint8_t access_category);
#endif

/**
 ****************************************************************************************
 * @brief Check if there is an A-MPDU under construction and if the TX queue is
 * empty. If both conditions are met, the A-MPDU is closed and programmed for TX.
 *
 * @param[in] access_category  Access category of the queue to be checked
 *
 ****************************************************************************************
 */
void txl_agg_check(uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Add Self-CTS protection to the A-MPDU.
 * This function first checks whether protection is already configured or not, and if not
 * it adds the Self-CTS. The rate of transmission of the CTS is decided depending on the
 * MCS used for the packet transmission (6 Mbps for MCS <= 2, 24 Mbps otherwise).
 *
 * @param[in,out] rc  The pointer to the RC step
 ****************************************************************************************
 */
void txl_agg_set_ampdu_protection(uint32_t *rc);

/**
 ****************************************************************************************
 * @brief Check if the current A-MPDU header descriptor indicates that the RTS/CTS retry
 * limit has been reached. In such case, reprogram the A-MPDU transmission with protection
 * changed to CTS-to-self to ensure that packet will go out.
 *
 * @param[in] a_thd              Pointer to the A-MPDU header descriptor
 * @param[in] access_category    Queue on which the A-MPDU is transmitted
 ****************************************************************************************
 */
void txl_agg_check_rtscts_retry_limit(struct tx_hd *a_thd, uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Check if an A-MPDU descriptor is currently saved and free it.
 *
 * This function is called each time a MPDU is marked as done by the MAC HW. Indeed a
 * A-MPDU descriptor might have been saved earlier because a new frame exchange was
 * "newTail'ed" to it, and therefore the HW could re-fetch the previous descriptor to get
 * the new one. When the new descriptor is marked as done, we are sure that we can free
 * the previous one.
 *
 * @param[in] access_category    Queue that needs to be checked
 ****************************************************************************************
 */
void txl_agg_check_saved_agg_desc(uint8_t access_category);

#if RW_MUMIMO_TX_EN
/**
 ****************************************************************************************
 * @brief Terminates the MU-MIMO PPDU. This function goes through the different user
 * positions, close the pending A-MPDUs, and finds which user position will be sent
 * on primary queue (i.e. the longest in duration). It then performs the chaining of
 * the different A-MPDU THDs to form the MU-MIMO PPDU to HW, and proceeds to the
 * allocation of the buffers whenever possible.
 *
 * @param[in] ac  Access Category
 ****************************************************************************************
 */
void txl_agg_mumimo_close(uint8_t ac);

/**
 ****************************************************************************************
 * @brief Process secondary user events of the MACTX IRQ
 *
 * This function releases packets transmitted for the second user in a MU transmission
 ****************************************************************************************
 */
void txl_agg_sec_transmit_trigger(void);
#endif // RW_MUMIMO_TX_EN

/**
 ****************************************************************************************
 * @brief Calculate number of blank delimiters to ensure MMSS after a THD
 * This number will be set in the THD of the NEXT txdesc in AMPDU
 *
 * @param[in] thd   Pointer to THD whose length number of blank delimiters is calculated
 * @param[in] mmss_bytes Minimal number of bytes in the A-MPDU subframe
 *
 * @return Number of blank delimiters to ensure Min MPDU Start Spacing
 ****************************************************************************************
 */
uint16_t txl_agg_mpdu_nb_delims(struct tx_hd *thd, uint16_t mmss_bytes);

/**
 ****************************************************************************************
 * @brief Release all AMPDU related space: BA, BAR, aggregate control and its AMPDU THD
 *
 * @param[in] agg_desc Pointer to A-MPDU descriptor
 ****************************************************************************************
 */
void txl_agg_release(struct tx_agg_desc *agg_desc);

/**
 ****************************************************************************************
 * @brief Transform a MPDU part of an A-MPDU into a singleton MPDU.
 *
 * This feature is used both for the BW drop and for the HE Trigger-Based transmissions.
 *
 * @param[in] txdesc Descriptor of the MPDU to be changed into singleton
 * @param[in] he_tb Flag indicating whether the MPDU will be sent as a HE TB PPDU or not
 *
 * @return The pointer to the THD of the MPDU, if the MPDU buffer is already allocated.
 * NULL otherwise.
 ****************************************************************************************
 */
struct tx_hd *txl_agg_change_to_singleton(struct txdesc *txdesc, bool he_tb);

#if NX_HE || NX_BW_LEN_ADAPT
/**
 ****************************************************************************************
 * @brief Reconfigure the A-MPDU following the PPDU sent as HE TB, or after a BW drop.
 *
 * This function has to be called when the next PPDU following a HE TB transmission is
 * a A-MPDU, or after a BW drop. It will reformat the A-MPDU in case it is the remainder
 * part of the  PPDU just sent, and check if the A-MPDU has enough data allocated to
 * be chained to the TX path.
 * The function returns the pointers to the THD that will form the next frame exchange.
 *
 * @param[in] txdesc First MPDU of the remaining part of a split A-MPDU
 * @param[out] last_thd Pointer to the last THD of the next frame exchange.
 *
 * @return The pointer to the first THD of the next frame exchange.
 ****************************************************************************************
 */
struct tx_hd *txl_agg_reconfig_ampdu(struct txdesc *txdesc, struct tx_hd **last_thd);
#endif

#if NX_HE
/**
 ****************************************************************************************
 * @brief Prepare the transmission of a HE TB PPDU.
 *
 * This function goes through the list of TX descriptors, starting with the one passed
 * as parameter, and builds the biggest possible A-MPDU fitting in the HE TB PPDU. For
 * that it will concatenate the packets until no concatenation is possible anymore.
 *
 * @param[in] txdesc  First MPDU of the A-MPDU to be checked
 * @param[out] txdesc_next Pointer to the TX descriptor following the last descriptor of
 * the HE TB PPDU.
 * @param[in] max_len Maximum length, in bytes, of the HE TB PPDU
 * @param[in] min_mpdu_len Minimum MPDU length inside the HE TB PPDU (for minimal MPDU
 * start spacing)
 * @param[in] ac Access category of the A-MPDU
 *
 * @return The pointer to the THD to be programmed in the HE TB PPDU (NULL if nothing can
 * be pushed).
 ****************************************************************************************
 */
struct tx_hd *txl_agg_he_tb_prep(struct txdesc *txdesc, struct txdesc **txdesc_next,
                                 uint32_t max_len, uint16_t min_mpdu_len, uint8_t ac);

#endif // NX_HE

#endif // NX_AMPDU_TX

/// @}
/// @}

#endif // _TXL_AGG_H_
