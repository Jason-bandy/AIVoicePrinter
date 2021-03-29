/**
 ****************************************************************************************
 *
 * @file txl_buffer.h
 *
 * @brief Management of Tx payload buffers
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _TXL_BUFFER_H_
#define _TXL_BUFFER_H_

/**
 ****************************************************************************************
 * @defgroup TX_BUFFER TX_BUFFER
 * @ingroup TX
 * @brief Definition and management of Tx payload buffers
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
#include "co_list.h"
#include "txl_cntrl.h"
#include "tx_swdesc.h"
#include "txl_agg.h"
#include "txl_he.h"
#include "co_utils.h"
#if NX_UMAC_PRESENT
#include "me_mic.h"
#include "txu_cntrl.h"
#include "rc.h"
#endif


/*
 * CONSTANTS
 ****************************************************************************************
 */
/// MTU of the MAC SW
#define TX_BUFFER_MTU 1536

/// Additional buffering (for headers)
#define TX_ADD_BUFFER 64

/// Maximum MPDU size
#define TX_MPDU_LEN_MAX  (TX_BUFFER_MTU + TX_ADD_BUFFER)

/// Buffer tag structure length
#define TX_BUFFER_TAG_LENGTH 252

/// Max buffer size
#define TX_BUFFER_MAX CO_ALIGN4_HI(TX_BUFFER_TAG_LENGTH + TX_MPDU_LEN_MAX)

/// Number of TX buffer pools
#define TX_BUFFER_POOL_MAX (AC_MAX * RW_USER_MAX + NX_BEACONING)

/// Size of the memory area reserved for one buffer pool (in number of 32-bit words)
#if NX_AMPDU_TX
#if defined(CFG_VIRTEX7) && !defined(CFG_FHOST)
/// Number of 1500-byte MSDUs that should be downloadable in a TX buffer
#define TX_MIN_DOWNLOAD_CNT   10
#else
#if NX_AMSDU_TX
/// Number of 1500-byte MSDUs that should be downloadable in a TX buffer
#define TX_MIN_DOWNLOAD_CNT   4
#else
/// Number of 1500-byte MSDUs that should be downloadable in a TX buffer
#define TX_MIN_DOWNLOAD_CNT   3
#endif // NX_AMSDU_TX
#endif // defined(CFG_VIRTEX7)
#else
/// Number of 1500-byte MSDUs that should be downloadable in a TX buffer
#define TX_MIN_DOWNLOAD_CNT   2
#endif // NX_AMPDU_TX

/// Theoretical size of the buffer pool (in number of 32-bit words) per queue
#define TX_BUFFER_POOL_SIZE_BASE   ((TX_MIN_DOWNLOAD_CNT * TX_BUFFER_MAX) / 4)

/// Target of minimal allocation percentage of a TX buffer to consider it as full
#define TX_BUFFER_FULL_THRES_PERCENT  90

/// Maximum percentage of free space in a TX buffer when it is considered as full
#define TX_BUFFER_FREE_THRES_PERCENT  (100 - TX_BUFFER_FULL_THRES_PERCENT)

/// Theoretical size of the free space when the buffer is considered as full
/// (in number of 32-bit words)
#define THD_FREE_SIZE CO_ALIGN4_LO((TX_BUFFER_POOL_SIZE_BASE * TX_BUFFER_FREE_THRES_PERCENT) / 100)

/// Margin to take to ensure that a max size MSDU fits into the free space of a TX buffer
/// when it is considered as full
#if THD_FREE_SIZE < TX_BUFFER_MAX
#define TX_BUFFER_POOL_MARGIN ((TX_BUFFER_MAX - THD_FREE_SIZE) / 4)
#else
#define TX_BUFFER_POOL_MARGIN 0
#endif

/// Actual size of the buffer pool (in number of 32-bit words) per queue
#define TX_BUFFER_POOL_SIZE     (TX_BUFFER_POOL_SIZE_BASE + TX_BUFFER_POOL_MARGIN)

/// Threshold (in number of 32-bit words) above which the buffer is considered as full
#define TX_BUFFER_FULL_THRES  (TX_BUFFER_POOL_SIZE_BASE * TX_BUFFER_FULL_THRES_PERCENT / 100)

/// Specific buffer address value indicating that buffer pool is empty
#define TX_BUFFER_NULL          0xFFFFFFFF

#if NX_UMAC_PRESENT
/// Maximum size of the padding between the Buffer Control structure and the payload
#define TX_BUFFER_PADDING_MAX   0
#else
/// Maximum size of the padding between the Buffer Control structure and the payload
#define TX_BUFFER_PADDING_MAX   3
#endif

/// Mask of the padding bits in the SW control information field
#define TX_BUFFER_PADDING_MASK  0x00000003

/// Offset of the padding bits in the SW control information field
#define TX_BUFFER_PADDING_OFT   0

/// Minimum amount of 32-bit words that form a super buffer
#define TX_BUFFER_MIN_SIZE      256

/// Minimum amount of data that has to be downloaded to chain an AMPDU
#define TX_BUFFER_MIN_AMPDU_DWNLD  (TX_BUFFER_POOL_SIZE_BASE * 80 / 100)

/// Number of DMA descriptors present in the TX buffer header structure
#if NX_AMSDU_TX
#define TX_DMA_DESC_MAX  NX_TX_PAYLOAD_MAX
#elif NX_MFP
#define TX_DMA_DESC_MAX  2
#else
#define TX_DMA_DESC_MAX  1
#endif

#if NX_FULLY_HOSTED
/** Number of Payload Buffer Descriptors attached to a packet.
    A packet passed by the TCP/IP stack may be split across TX_PBD_CNT buffers.         */
#define TX_PBD_CNT            3
#endif

/// Buffer flags
enum
{
    /// Flag indicating that the buffer is a frontier
    BUF_FRONTIER = CO_BIT(0),
    /// Flag indicating that the buffer is split
    BUF_SPLIT = CO_BIT(1),
};

/*
 * TYPE and STRUCT DEFINITIONS
 ****************************************************************************************
 */
/// Structure containing the HW descriptors needed to handle non-contiguous buffers
struct txl_buffer_hw_desc_tag
{
    /// IPC DMA descriptor
    struct dma_desc dma_desc;
    /// Payload buffer descriptor
    struct tx_pbd   pbd;
};

/// Buffer list structure
struct txl_buffer_list_tag
{
    /// First element of the list
    struct txl_buffer_tag *first;
    /// Last element of the list
    struct txl_buffer_tag *last;
};

/// Tx rate structure
struct txl_buffer_idx_tag
{
    /// Amount of used space in the pool
    uint32_t used_area;
    /// Index of the beginning of the free part of the pool
    uint32_t free;
    /// Size of the free part of the pool
    uint32_t free_size;
    /// Index of the lastly allocated buffer
    uint32_t last;
    /// Amount of data needed for the next allocation (different from 0 only when an
    /// allocation fails)
    uint32_t next_needed;
    /// Current size allocated in the pending super buffer
    uint32_t buf_size;
    /// Pointer to the buffer pool
    uint32_t *pool;
    /// Pointer to the additional HW descriptor of the pool
    struct txl_buffer_hw_desc_tag *desc;
    /// Number of super buffers currently allocated
    uint8_t count;
};


/// Context of the Tx buffer management block
struct txl_buffer_env_tag
{
    /// Indexes of the buffer pools
    struct txl_buffer_idx_tag buf_idx[NX_TXQ_CNT][RW_USER_MAX];
    /// List elements
    struct txl_buffer_list_tag list[NX_TXQ_CNT];
};

/// Buffer header structure
struct txl_buffer_tag
{
    #if !NX_FULLY_HOSTED
    /// Total length of the buffer
    uint32_t length;
    /// Linked list element allowing to chain buffers together
    struct txl_buffer_tag *next;
    /// Pointer to the TX descriptor linked to this buffer
    struct txdesc *txdesc;
    /// Pointer to the main TX descriptor of the first exchange (i.e. the first one in
    /// case of a A-MPDU, or the TX descriptor linked to this buffer otherwise)
    struct txdesc *txdesc_main;
    /// IPC DMA descriptor for the payload
    struct dma_desc dma_desc[TX_DMA_DESC_MAX];
    /// IPC DMA descriptor for the pattern
    struct dma_desc dma_desc_pat;
    /// IPC DMA descriptor for the late push
    struct dma_desc dma_desc_late;
    #if (RW_BFMER_EN)
    /// IPC DMA descriptor for the Beamforming Report
    struct dma_desc dma_desc_bfr;
    #endif //(RW_BFMER_EN)
    /// TX buffer descriptor
    struct tx_pbd tbd;
    /// Index of the user in the access category (0 if no MU-MIMO)
    uint8_t user_idx;
    #endif //!NX_FULLY_HOSTED
    /// Bit field containing some information flags about the buffer
    uint32_t flags;
    /// Buffer control (filled by the upper MAC)
    struct txl_buffer_control buffer_control;
    #if NX_FULLY_HOSTED
    /// TX buffer descriptor structure
    struct tx_pbd pbd[TX_PBD_CNT];
    /// TX buffer descriptor structure for the security tail
    struct tx_pbd pbd_tail;
    /// Security tail space
    uint8_t sec_tail[SEC_MAX_TAIL_LEN];
    /// MAC+SEC+LLC header space - Has to be placed just before the payload space
    uint8_t headers[MAC_LONG_QOS_MAC_HDR_LEN + SEC_MAX_HDR_LEN + LLC_802_2_HDR_LEN - LLC_ETHER_HDR_LEN];
    #endif
    /// Payload area (force 4-byte alignment)
    uint32_t payload[];
};


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// TX buffer space
extern uint32_t txl_buffer_pool[TX_BUFFER_POOL_MAX][TX_BUFFER_POOL_SIZE];

/// Additional HW descriptors used for non-contiguous buffers
extern struct txl_buffer_hw_desc_tag txl_buffer_hw_desc[TX_BUFFER_POOL_MAX];

/// HW location for late DMA transfers
extern uint32_t txl_buffer_dma_late_loc;

/// Environment of the Tx buffer management module
extern struct txl_buffer_env_tag txl_buffer_env;

#if NX_UMAC_PRESENT
/// Per-STA buffer control descriptor pairs
extern struct txl_buffer_control txl_buffer_control_desc[NX_REMOTE_STA_MAX];
/// Per-VIF BC/MC buffer control descriptors
extern struct txl_buffer_control txl_buffer_control_desc_bcmc[NX_VIRT_DEV_MAX];
#endif

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/// @name External API
/// @{

#if !NX_FULLY_HOSTED
/**
 ****************************************************************************************
 * @brief Push a TX buffer in the buffer list.
 *
 * This function is called when a specific handling needs to be performed upon completion
 * of the payload DMA transfer (e.g. for singleton MPDUs or when enough data has been
 * downloaded for A-MPDUs).
 *
 * @param[in] access_category The index of the TX queue for which the buffer is pushed
 * @param[in] buf The pointer to the TX buffer to be pushed
 ****************************************************************************************
 */
__INLINE void txl_buffer_push(uint8_t access_category, struct txl_buffer_tag *buf)
{
    struct txl_buffer_list_tag *list = &txl_buffer_env.list[access_category];

    // check if list is empty
    if (list->first == NULL)
    {
        // list empty => pushed element is also head
        list->first = buf;
    }
    else
    {
        // list not empty => update next of last
        list->last->next = buf;
    }

    // add element at the end of the list
    list->last = buf;
    buf->next = NULL;
}

/**
 ****************************************************************************************
 * @brief Pop a TX buffer from the buffer list.
 * This function is called upon the completion of the payload DMA transfer of a pushed
 * buffer.
 *
 * @param[in] access_category The index of the TX queue for which the buffer was pushed
 *
 * @return The pointer to the TX buffer pushed
 ****************************************************************************************
 */
__INLINE struct txl_buffer_tag *txl_buffer_pop(uint8_t access_category)
{
    struct txl_buffer_list_tag *list = &txl_buffer_env.list[access_category];
    struct txl_buffer_tag *buf;

    // check if list is empty
    buf = list->first;
    if (buf != NULL)
    {
        // The list isn't empty : extract the first element
        list->first = list->first->next;
    }

    return buf;
}

/**
 ****************************************************************************************
 * @brief Pick a TX buffer from the buffer list.
 * This function returns the first TX buffer in the list, without popping it.
 *
 * @param[in] access_category The index of the TX queue
 *
 * @return The pointer to the first TX buffer in the list
 ****************************************************************************************
 */
__INLINE struct txl_buffer_tag *txl_buffer_pick(uint8_t access_category)
{
    struct txl_buffer_list_tag *list = &txl_buffer_env.list[access_category];

    return list->first;
}

/**
 ****************************************************************************************
 * @brief Check if the buffer list is empty.
 *
 * @param[in] access_category The index of the TX queue
 *
 * @return true if the list is empty, false otherwise
 ****************************************************************************************
 */
__INLINE bool txl_buffer_list_empty(uint8_t access_category)
{
    struct txl_buffer_list_tag *list = &txl_buffer_env.list[access_category];

    return (list->first == NULL);
}
#endif //!NX_FULLY_HOSTED


#if NX_AMSDU_TX
/**
 ****************************************************************************************
 * @brief Check whether a MPDU is an A-MSDU stored in multiple TX buffers.
 *
 * @param[in] txdesc The pointer to the TX descriptor to be checked.
 *
 * @return    true if the MPDU is an A-MSDU stored in multiple TX buffers, false
 *            otherwise.
 ****************************************************************************************
 */
__INLINE bool txl_buffer_is_amsdu_multi_buf(struct txdesc *txdesc)
{
    return (is_mpdu_split(txdesc) && (tx_desc_thd(txdesc)->frmlen > TX_MPDU_LEN_MAX));
}

/**
 ****************************************************************************************
 * @brief Check whether a MPDU is an A-MSDU stored in a single TX buffer.
 *
 * @param[in] txdesc The pointer to the TX descriptor to be checked.
 *
 * @return    true if the MPDU is an A-MSDU stored in a single TX buffer, false
 *            otherwise.
 ****************************************************************************************
 */
__INLINE bool txl_buffer_is_amsdu_single_buf(struct txdesc *txdesc)
{
    return (is_mpdu_split(txdesc) && (tx_desc_thd(txdesc)->frmlen <= TX_MPDU_LEN_MAX));
}
#endif

/**
 ****************************************************************************************
 * @brief Get the pointer to the buffer structure
 *
 * @param[in] txdesc The pointer to the TX descriptor
 *
 * @return The pointer to the buffer structure
 *
 ****************************************************************************************
 */
__INLINE struct txl_buffer_tag *txl_buffer_get(struct txdesc *txdesc)
{
    #if NX_AMSDU_TX
    struct txl_buffer_tag *buffer = txdesc->lmac.buffer[0];
    #else
    struct txl_buffer_tag *buffer = txdesc->lmac.buffer;
    #endif
    return (buffer);
}


/**
 ****************************************************************************************
 * @brief Get the pointer to the buffer allocated for the last fragment of a MPDU
 *
 * In case of a A-MSDU not allocated in a single TX buffer, this function returns the
 * pointer to the last MSDU buffer. Otherwise it returns the pointer to the unique buffer
 * that might be allocated for the frame.
 *
 * @param[in] txdesc The pointer to the TX descriptor
 *
 * @return The pointer to the buffer structure
 *
 ****************************************************************************************
 */
__INLINE struct txl_buffer_tag *txl_buffer_last_get(struct txdesc *txdesc)
{
    struct txl_buffer_tag *buffer;
    #if NX_AMSDU_TX
    if (txl_buffer_is_amsdu_multi_buf(txdesc))
        buffer = txdesc->lmac.buffer[txdesc->host.packet_cnt - 1];
    else
        buffer = txdesc->lmac.buffer[0];
    #else
    buffer = txdesc->lmac.buffer;
    #endif
    return (buffer);
}


/**
 ****************************************************************************************
 * @brief Get the pointer to the buffer control structure
 *
 * @param[in] txdesc The pointer to the TX descriptor
 *
 * @return The pointer to the buffer control structure
 *
 ****************************************************************************************
 */
__INLINE struct txl_buffer_control *txl_buffer_control_get(struct txdesc *txdesc)
{
    struct txl_buffer_tag *buffer = txl_buffer_get(txdesc);
    return (&buffer->buffer_control);
}

/**
 ****************************************************************************************
 * @brief Get the pointer to the buffer payload
 *
 * @param[in] txdesc The pointer to the TX descriptor
 *
 * @return The pointer to the buffer payload
 *
 ****************************************************************************************
 */
__INLINE void *txl_buffer_payload_get(struct txdesc *txdesc)
{
    struct txl_buffer_tag *buffer = txl_buffer_get(txdesc);
    return ((void *)buffer->payload);
}

/**
 ****************************************************************************************
 * @brief Get the address of the MAC header
 * This function is supposed to be called when the payload is present in the shared memory.
 *
 * @param[in] txdesc The pointer to the TX descriptor
 *
 * @return HW address of the MAC header
 *
 ****************************************************************************************
 */
__INLINE uint32_t txl_buffer_machdr_get(struct txdesc *txdesc)
{
    struct tx_hd *thd = tx_desc_thd(txdesc);
    struct tx_pbd *pbd;

    // A buffer shall be available
    ASSERT_ERR(txl_buffer_get(txdesc) != NULL);

    // Check if the frame buffer is at least partly pointed by the THD
    if (thd->datastartptr)
        return (thd->datastartptr);

    // Sanity check - If the MAC header is not pointed by the THD, a PBD shall be present
    ASSERT_ERR(thd->first_pbd_ptr);
    pbd = HW2CPU(thd->first_pbd_ptr);

    return (pbd->datastartptr);
}

#if NX_UMAC_PRESENT
/**
 ****************************************************************************************
 * @brief If a trial transmission is requested, copy the trial rate configuration into
 *        the buffer and update the trial status.
 *
 * @param[in]   txdesc The pointer to the TX descriptor
 * @param[in]   buf    The pointer to the buffer allocated for the frame transmission
 *
 ****************************************************************************************
 */
__INLINE void txl_buffer_control_trial(struct txdesc *txdesc, struct txl_buffer_tag *buf)
{
    // Set ratecntrlinfo for trial rate transmission
    if (txdesc->host.flags & TXU_CNTRL_RC_TRIAL)
    {
        struct txl_buffer_control *buf_ctrl_src = txdesc->umac.buf_control;
        struct txl_buffer_control *buf_ctrl_dest = &buf->buffer_control;
        // get RC station statistics
        struct rc_sta_stats *rc_ss = rc_get_sta_stats(txdesc->host.staid);

        PROF_RC_SET_TRIAL_BUFFER_SET();

        // Move step 0 to step 1
        buf_ctrl_dest->policy_tbl.ratecntrlinfo[1] = buf_ctrl_src->policy_tbl.ratecntrlinfo[0];
        #if NX_HE
        buf_ctrl_dest->policy_tbl.powercntrlinfo[1] = buf_ctrl_src->policy_tbl.powercntrlinfo[0];
        #endif

        // Replace step 0 by trial rate
        buf_ctrl_dest->policy_tbl.ratecntrlinfo[0] = rc_ss->trial_rate;
        #if NX_HE
        if (txl_he_is_he_su(rc_ss->trial_rate))
        {
            txl_he_pwr_info_set(rc_ss->trial_rate,
                                rc_ss->trial_dcm,
                                &buf_ctrl_dest->policy_tbl.powercntrlinfo[0]);
        }
        #endif

        // Disable the STBC
        if ((buf_ctrl_src->tx_flags & TX_SWDESC_UMAC_TRIAL_STBC_BIT) == 0)
        {
            buf_ctrl_dest->policy_tbl.phycntrlinfo1 &= ~STBC_PT_MASK;
        }

        PROF_RC_SET_TRIAL_BUFFER_CLR();
    }
}

/**
 ****************************************************************************************
 * @brief Copy the buffer control structure into the buffer
 *
 * @param[in]   txdesc The pointer to the TX descriptor
 * @param[in]   buf    The pointer to the buffer allocated for the frame transmission
 *
 ****************************************************************************************
 */
__INLINE void txl_buffer_control_copy(struct txdesc *txdesc,
                                      struct txl_buffer_tag *buf)
{
    uint32_t *dst = (uint32_t *)&buf->buffer_control;
    uint32_t *src = (uint32_t *)txdesc->umac.buf_control;
    unsigned int i;

    // Copy the policy table
    for (i = 0; i < (sizeof_b(struct txl_buffer_control)/sizeof_b(uint32_t)); i++)
    {
        *dst++ = *src++;
    }

    // Set the trial rate if a trial tx is requested
    txl_buffer_control_trial(txdesc, buf);

}
#endif


/**
 ****************************************************************************************
 * @brief Get the padding value from the SW control information field
 *
 * @param[in]   txdesc   The pointer to the TX descriptor for which the padding value has
 * to be retrieved
 *
 * @return    The padding value
 *
 ****************************************************************************************
 */
__INLINE int txl_buffer_padding(struct txdesc *txdesc)
{
    #if NX_UMAC_PRESENT
    return (0);
    #else
    return (txdesc->host.padding);
    #endif
}

#if NX_HE
/**
 ****************************************************************************************
 * @brief Remove the +HTC field present in the MAC Header pointed by the TX header
 * descriptor passed as parameter.
 *
 * This function is supposed to be called when a frame initially formatted with a +HTC
 * field (e.g. a frame transmitted by a non-AP HE STA) is finally transmitted as a
 * singleton MPDU, with a policy table that may cause the transmission to be done using
 * a DSSS/CCK modulation.
 * The function clears the ORDER bit in the frame control field, and adjusts the packet
 * length and data end pointers to remove the +HTC from the transmitted packet.
 *
 * @param[in] thd Pointer to the TX header descriptor.
 ****************************************************************************************
 */
__INLINE void txl_buffer_remove_htc(struct tx_hd *thd)
{
    #if NX_UMAC_PRESENT
    uint32_t mac_hdr_addr = thd->datastartptr;
    uint16_t framectl = co_read16p(mac_hdr_addr);

    // Check if the order bit is set in the MAC Header, indicating if the +HTC is present
    // or not
    if ((framectl & MAC_FCTRL_ORDER) == 0)
        return;

    framectl &= ~MAC_FCTRL_ORDER;
    co_write16p(mac_hdr_addr, framectl);

    thd->frmlen -= MAC_HTC_LEN;
    thd->dataendptr -= MAC_HTC_LEN;
    #endif
}

/**
 ****************************************************************************************
 * @brief Re-add the +HTC field that was initially present in the MAC Header pointed by
 * the TX header descriptor passed as parameter.
 *
 * This function is supposed to be called when a frame initially formatted with a +HTC
 * field saw this field removed using @ref txl_buffer_remove_htc before being chained to
 * the HW EDCA queue, but finally requires the field to be put back because the frame is
 * re-programmed for transmission in a HE TB PPDU.
 * The function sets back the ORDER bit in the frame control field, and adjusts the packet
 * length and data end pointer to add the +HTC back in the transmitted packet.
 * If the +HTC field is already present in the packet (order bit set), then the function
 * does nothing.
 *
 * @param[in] thd Pointer to the TX header descriptor.
 ****************************************************************************************
 */
__INLINE void txl_buffer_add_htc(struct tx_hd *thd)
{
    #if NX_UMAC_PRESENT
    uint32_t mac_hdr_addr = thd->datastartptr;
    uint16_t framectl;

    if (!mac_hdr_addr || ((framectl = co_read16p(mac_hdr_addr)) & MAC_FCTRL_ORDER))
        return;

    framectl |= MAC_FCTRL_ORDER;
    co_write16p(mac_hdr_addr, framectl);

    thd->frmlen += MAC_HTC_LEN;
    thd->dataendptr += MAC_HTC_LEN;
    #endif
}

/**
 ****************************************************************************************
 * @brief Update the value of the +HTC field.
 *
 * This function assumes that there is room in the packet buffer for the +HTC and it shall
 * be called only in this case.
 *
 * @param[in] thd Pointer to the TX header descriptor.
 * @param[in] htc Value of the +HTC field
 *
 * @return true if the +HTC field has been updated, false if no buffer is available
 *         currently
 ****************************************************************************************
 */
__INLINE bool txl_buffer_update_htc(struct tx_hd *thd, uint32_t htc)
{
    #if NX_UMAC_PRESENT
    // Ensure we have a buffer already allocated
    if (!thd->datastartptr)
        return false;

    co_write32p(thd->dataendptr - MAC_HTC_LEN + 1, htc);
    return true;
    #else
    return false;
    #endif
}


#endif

#if !NX_FULLY_HOSTED
/**
 ****************************************************************************************
 * @brief Update the THD and TBD after allocation of the TX buffer.
 * This function initializes various pointers of the HW descriptors to memory areas that
 * are inside the allocated TX buffer, and that can therefore not be set prior to the
 * allocation.
 *
 * @param[in] txdesc The pointer to the TX descriptor for which the THD and TBD have
 *                   to be updated
 * @param[in] access_category The corresponding queue index
 *
 * ****************************************************************************************
 */
__INLINE void txl_buffer_update_thd(struct txdesc *txdesc,
                                    uint8_t access_category)
{
    struct tx_hd *thd = tx_desc_thd(txdesc);
    #if NX_AMSDU_TX
    struct txl_list *txlist = &txl_cntrl_env.txlist[access_category];
    #endif
    struct txl_buffer_tag *buffer = txl_buffer_get(txdesc);
    struct txl_buffer_idx_tag *idx = &txl_buffer_env.buf_idx[access_category][buffer->user_idx];
    uint32_t pool_size = TX_BUFFER_POOL_SIZE * sizeof_b(uint32_t);
    uint32_t pool_start = CPU2HW(idx->pool);
    uint32_t pool_end = pool_start + pool_size - 1;
    uint32_t start = CPU2HW(buffer->payload) + txl_buffer_padding(txdesc);
    #if NX_UMAC_PRESENT
    uint16_t add_len;
    uint32_t payl = start + txdesc->umac.machead_len;
    #else
    uint32_t payl = start;
    #endif
    #if NX_AMSDU_TX
    uint32_t pkt_len = txdesc->host.packet_len[0];
    #else
    uint32_t pkt_len = txdesc->host.packet_len;
    #endif
    uint32_t end;
    struct tx_pbd *pbd = &buffer->tbd;
    struct tx_pbd *add_pbd = NULL;
    struct tx_pbd *last_pbd = pbd;

    #if NX_UMAC_PRESENT
    #if NX_AMSDU_TX
    if (txl_buffer_is_amsdu_single_buf(txdesc))
        pkt_len = tx_desc_thd(txdesc)->frmlen - MAC_FCS_LEN;
    else
    #endif
    {
        add_len = txdesc->umac.head_len;
        #if NX_AMSDU_TX
        if (!is_mpdu_split(txdesc))
        #endif
            add_len += txdesc->umac.tail_len;
        pkt_len += add_len;
    }
    #elif NX_AMSDU_TX
    if (txl_buffer_is_amsdu_single_buf(txdesc))
        pkt_len = tx_desc_thd(txdesc)->frmlen - MAC_FCS_LEN;
    #endif
    end = start + pkt_len - 1;

    // By default we consider that the buffer is not split
    buffer->flags &= ~BUF_SPLIT;

    // Check if buffer padding has caused a buffer wrap
    ASSERT_ERR(start < pool_end);

    // Check if buffer is split
    if (end > pool_end)
    {
        // Buffer is split
        end = pool_end;

        // We use the PBD of the pool to handle the split
        add_pbd = &idx->desc->pbd;
        add_pbd->datastartptr = pool_start;
        add_pbd->dataendptr = pool_start + pkt_len - (end - start) - 2;

        last_pbd = add_pbd;

        // Buffer is split
        buffer->flags |= BUF_SPLIT;
    }

    // Set start and end pointers to the beginning and end of the frame
    #if NX_UMAC_PRESENT
    if (txdesc->umac.machead_len)
    {
        thd->datastartptr = start;
        thd->dataendptr = start + txdesc->umac.machead_len - 1;
    }
    #endif
    pbd->datastartptr = payl;
    pbd->dataendptr = end;
    pbd->next = CPU2HW(add_pbd);
    pbd->bufctrlinfo = 0;

    thd->first_pbd_ptr = CPU2HW(pbd);
    last_pbd->bufctrlinfo = 0;
    #if NX_AMPDU_TX
    if ((buffer->flags & BUF_FRONTIER) || is_mpdu_last(txdesc) || !is_mpdu_agg(txdesc))
    {
        if (!(buffer->flags & BUF_FRONTIER))
        {
            #if NX_AMSDU_TX
            if (!is_mpdu_split(txdesc))
            {
            #endif
                buffer->flags |= BUF_FRONTIER;
                idx->count++;
            #if NX_AMSDU_TX
            }
            #endif
            // Set WhichDescriptor field
            thd->macctrlinfo2 |= INTERRUPT_EN_TX;
        }
        else
        {
            #if NX_AMSDU_TX
            if (txl_buffer_is_amsdu_multi_buf(txdesc))
            {
                // Enable interrupt on the last PBD
                last_pbd->bufctrlinfo = TBD_INTERRUPT_EN;

                // For singleton we need to enable the IRQ on packet completion
                if (!is_mpdu_agg(txdesc))
                    thd->macctrlinfo2 |= INTERRUPT_EN_TX;
            }
            else
            #endif
            {
                // Set WhichDescriptor field
                thd->macctrlinfo2 |= INTERRUPT_EN_TX;
            }
        }
    }
    #else
    // Set WhichDescriptor field
    thd->macctrlinfo2 = WHICHDESC_UNFRAGMENTED_MSDU | INTERRUPT_EN_TX;
    #endif
    last_pbd->next = 0;

    #if NX_AMSDU_TX
    // Store the new value of last PBD
    txlist->last_pbd[buffer->user_idx] = last_pbd;
    #endif
}

#if NX_AMSDU_TX
/**
 ****************************************************************************************
 * @brief Update the TBD of A-MSDU sub-frames after the first one.
 * This function initializes the TBD linked to A-MSDU sub-frames \#2 - \#n (Sub-frame \#1
 * THD/TBD is updated in @ref txl_buffer_update_thd).
 *
 * @param[in] txdesc The pointer to the TX descriptor for which the TBD has to be updated
 * @param[in] access_category The corresponding queue index
 * @param[in] pkt_idx Index of the MSDU inside the A-MSDU
 *
 * @return    The padding value
 *
 ****************************************************************************************
 */
__INLINE void txl_buffer_update_tbd(struct txdesc *txdesc,
                                    uint8_t access_category,
                                    uint8_t pkt_idx)
{
    struct txl_list *txlist = &txl_cntrl_env.txlist[access_category];
    struct txl_buffer_tag *buffer = txdesc->lmac.buffer[pkt_idx];
    struct tx_pbd *tbd = &buffer->tbd;
    uint32_t pkt_len = txdesc->host.packet_len[pkt_idx];
    struct txl_buffer_idx_tag *idx = &txl_buffer_env.buf_idx[access_category][buffer->user_idx];
    uint32_t pool_size = TX_BUFFER_POOL_SIZE * sizeof_b(uint32_t);
    uint32_t pool_start = CPU2HW(idx->pool);
    uint32_t pool_end = pool_start + pool_size - 1;
    uint32_t start = CPU2HW(buffer->payload);
    uint32_t end = start + pkt_len - 1;
    struct tx_pbd *pbd = NULL;
    struct tx_pbd *last = tbd;

    #if NX_UMAC_PRESENT
    if ((pkt_idx + 1) == txdesc->host.packet_cnt) {
       pkt_len += txdesc->umac.tail_len;
       end += txdesc->umac.tail_len;
    }
    #endif

    // By default we consider that the buffer is not split
    buffer->flags &= ~BUF_SPLIT;

    // Check if buffer padding has caused a buffer wrap
    if (start > pool_end)
    {
        start -= pool_size;
        end -= pool_size;
    }
    else
    {
        // Check if buffer is split
        if (end > pool_end)
        {
            // Buffer is split
            end = pool_end;

            // We use the PBD of the pool to handle the split
            pbd = &idx->desc->pbd;
            pbd->datastartptr = pool_start;
            pbd->dataendptr = pool_start + pkt_len - (end - start) - 2;

            last = pbd;

            // Buffer is split
            buffer->flags |= BUF_SPLIT;
        }
    }

    // Set start and end pointers to the beginning and end of the frame
    tbd->datastartptr = start;
    tbd->dataendptr = end;
    tbd->next = CPU2HW(pbd);
    tbd->bufctrlinfo = 0;

    if ((buffer->flags & BUF_FRONTIER) ||
        (is_mpdu_last(txdesc) && ((pkt_idx + 1) == txdesc->host.packet_cnt)))
        // Enable interrupt on the last PBD
        last->bufctrlinfo = TBD_INTERRUPT_EN;
    else
        last->bufctrlinfo = 0;

    // Chain the descriptor(s) to the previous one(s)
    ASSERT_ERR(txlist->last_pbd[buffer->user_idx] != NULL);
    txlist->last_pbd[buffer->user_idx]->next = CPU2HW(tbd);

    // Store the new value of last PBD
    last->next = 0;
    txlist->last_pbd[buffer->user_idx] = last;
}
#endif


#if NX_UMAC_PRESENT
/**
 ****************************************************************************************
 * @brief Compute the TKIP MIC over the TX buffer.
 * This function handles split buffers (i.e. buffers split accross the end and the start
 * of the buffer pool. The resulting MIC is copied at the end of the buffer.
 *
 * @param[in] txdesc   The pointer to the TX descriptor linked to the buffer
 * @param[in] mic_key  The pointer to the MIC key to be used
 * @param[in] start    Start address of the payload on which the MIC is computed
 * @param[in] len      Length of the payload on which the MIC is computed
 * @param[in] access_category The corresponding queue index
 ****************************************************************************************
 */
__INLINE void txl_buffer_mic_compute(struct txdesc *txdesc,
                                     uint32_t *mic_key,
                                     uint32_t start,
                                     uint32_t len,
                                     uint8_t access_category)
{
    struct hostdesc *host = &txdesc->host;
    struct mic_calc mic;
    struct txl_buffer_tag *buffer = txl_buffer_get(txdesc);
    struct txl_buffer_idx_tag *idx = &txl_buffer_env.buf_idx[access_category][buffer->user_idx];
    uint32_t pool_size = TX_BUFFER_POOL_SIZE * sizeof_b(uint32_t);
    uint32_t pool_start = CPU2HW(idx->pool);
    uint32_t pool_end = pool_start + pool_size - 1;
    uint32_t end = start + len - 1;
    uint32_t clen = len;
    uint32_t mic_start = end + 1, mic_end;
    uint32_t mic_addr = CPU2HW(&mic.mic_key_least);

    // Initialize MIC computation
    me_mic_init(&mic, mic_key, &host->eth_dest_addr, &host->eth_src_addr, host->tid);

    // Check if buffer is split
    if (end > pool_end)
    {
        clen = pool_end - start + 1;

        // Buffer is split
        me_mic_calc(&mic, start, clen);

        // Compute the parameters of the next computation
        clen = len - clen;
        start = pool_start;
    }

    me_mic_calc(&mic, start, clen);

    // End the MIC computation
    me_mic_end(&mic);

    // Copy the MIC in the packet
    if (mic_start > pool_end)
        mic_start -= pool_size;
    mic_end = mic_start + 7;
    clen = 8;
    if (mic_end > pool_end)
    {
        clen = pool_end - mic_start + 1;

        // Copy the first part of the MIC in the TX buffer
        co_copy8p(mic_start, mic_addr, clen);

        // Compute the parameters of the next computation
        mic_addr += clen;
        clen = 8 - clen;
        mic_start = pool_start;
    }

    // Copy the remaining part of the MIC in the TX buffer
    co_copy8p(mic_start, mic_addr, clen);
}
#endif //NX_UMAC_PRESENT

/**
 ****************************************************************************************
 * @brief Re-initializes the Tx buffer area (upon recovery procedure)
 *
 ****************************************************************************************
 */
void txl_buffer_reinit(void);

/**
 ****************************************************************************************
 * @brief Reset a Tx pool.
 *
 * This primitive frees all the allocated buffers of a Tx pool.
 *
 * @param[in] access_category   Access category for which the pool is reset
 *
 ****************************************************************************************
 */
void txl_buffer_reset(uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Allocates a Tx buffer
 *
 * This primitive allocates a Tx buffer from the free pool according to the
 * priority (one pool per access category).
 *
 * @param[in] txdesc          TX Descriptor for which we need to allocate the buffer
 * @param[in] access_category Access category for which the buffer is allocated
 * @param[in] user_idx        User index (for MU-MIMO TX only, 0 otherwise)
 * @param[in]  pkt_idx        Indicates which payload is transfered when several payloads
 *                            are present in the TX descriptors (i.e. in case of A-MSDU).
 *                            0xFF is a specific value indicating that the txdesc describes
 *                            a small A-MSDU that shall be fully allocated in a single
 *                            buffer.
 *
 * @return A pointer to the allocated buffer, NULL if allocation failed.
 *
 ****************************************************************************************
 */
#if NX_AMSDU_TX
struct txl_buffer_tag *txl_buffer_alloc(struct txdesc *txdesc, uint8_t access_category,
                                        uint8_t user_idx, uint8_t pkt_idx);
#else
struct txl_buffer_tag *txl_buffer_alloc(struct txdesc *txdesc, uint8_t access_category,
                                        uint8_t user_idx);
#endif

/**
 ****************************************************************************************
 * @brief Releases a Tx buffer
 *
 * @param[in] buf               Pointer to the buffer to be freed
 * @param[in] access_category   Access category for which the buffer is freed
 *
 * @return true if a new buffer allocation attempt can occur, false otherwise
 *
 ****************************************************************************************
 */
bool txl_buffer_free(struct txl_buffer_tag *buf, uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Free the Tx buffer(s) attached to a TX descriptor.
 *
 * @param[in] txdesc TX Descriptor for which we need to free the buffer(s)
 * @param[in] access_category Access category on which the buffer(s) are freed
 ****************************************************************************************
 */
void txl_buffer_free_all(struct txdesc *txdesc, uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Returns the amount of data currently allocated in the TX buffer
 *
 * @param[in] access_category   Access category for which the buffer data is checked
 * @param[in] user_idx          User index (for MU-MIMO TX only, 0 otherwise)
 *
 * @return The allocated data size, in 32-bit words
 *
 ****************************************************************************************
 */
__INLINE uint32_t txl_buffer_used(uint8_t access_category, uint8_t user_idx)
{
    return txl_buffer_env.buf_idx[access_category][user_idx].used_area;
}

/**
 ****************************************************************************************
 * @brief Indicates if the TX buffer pool for this queue is full or not
 *
 * @param[in] access_category   Access category for which the buffer data is checked
 * @param[in] user_idx          User index (for MU-MIMO TX only, 0 otherwise)
 *
 * @return true if the buffer pool is full, false otherwise
 *
 ****************************************************************************************
 */
__INLINE uint8_t txl_buffer_count(uint8_t access_category, uint8_t user_idx)
{
    struct txl_buffer_idx_tag *idx = &txl_buffer_env.buf_idx[access_category][user_idx];

    return(idx->count);
}

/**
 ****************************************************************************************
 * @brief Push a TX buffer already downloaded in the buffer list.
 *
 * This function is called when a A-MPDU that did not reach the chaining threshold (i.e.
 * @ref TX_BUFFER_MIN_AMPDU_DWNLD) is closed using the @ref txl_agg_finish.
 * It pushes the buffer in the downloading queue, and trig a fake DMA transfer in order
 * to:
 *  - Generate a DMA interrupt
 *  - Ensure that the A-MPDU last buffer is downloaded when the DMA interrupt is handled
 *
 * @param[in] access_category The index of the TX queue for which the buffer is pushed
 * @param[in] buf The pointer to the TX buffer to be pushed
 ****************************************************************************************
 */
void txl_buffer_push_late(uint8_t access_category, struct txl_buffer_tag *buf);

#endif //!NX_FULLY_HOSTED

/**
 ****************************************************************************************
 * @brief Initializes the Tx buffer area
 *
 ****************************************************************************************
 */
void txl_buffer_init(void);

/// @}
/// @}

#endif /// TX_BUFFER_H_
