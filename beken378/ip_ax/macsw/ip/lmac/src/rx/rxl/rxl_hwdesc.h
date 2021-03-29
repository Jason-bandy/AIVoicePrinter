/**
 ****************************************************************************************
 *
 * @file rxl_hwdesc.h
 *
 * @brief File contains the declaration of pool's elements.
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _RXL_HWDESC_H_
#define _RXL_HWDESC_H_

/**
 ****************************************************************************************
 * @defgroup RX_HWDESC RX_HWDESC
 * @ingroup RX
 * @brief Rx buffer/descriptor management
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#if BK_MAC
#include "rwnx_config.h"
#endif
// for ASSERT
#include "dbg_assert.h"
// for __INLINE
#include "compiler.h"
// For co_status_t
#include "co_status.h"
// For rx_payload_desc
#include "hal_desc.h"
// For rx_upload_cntrl_tag
#include "rxl_cntrl.h"

/*
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */
struct rxu_cntrl_defrag;
struct rx_dmadesc;

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
/// SW descriptor to manage the MAC SW receive operations.
struct rxdesc
{
    /// Upload control element. Shall be the first element of the RX descriptor structure
    struct rx_upload_cntrl_tag upload_cntrl;
    #if NX_RX_RING
    /// HW descriptors
    struct rx_dmadesc dma_hdrdesc;
    /// Address of the expected HW descriptor following the present in the RX buffer one,
    /// and that should be used to set to the read pointer to free the buffer
    uint32_t new_read;
    /// Id of the buffer (0 or 1)
    uint8_t buf_id;
    #else
    /// Holds the MAC HW header descriptors list head pointer
    struct rx_dmadesc *dma_hdrdesc;
    /// Pointer to the last payload buffer descriptor
    struct rx_pbd *last_pbd;
    /// Pointer to the spare payload buffer descriptor
    struct rx_pbd *spare_pbd;
    #endif
};

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */
/// Length of the pattern for final DMA transfer
#define RXL_LAST_DMA_PATTERN_LEN    4

#if NX_UMAC_PRESENT
#if NX_AMSDU_DEAGG
/// Length of de-aggregated A-MSDUs (MSDUs are uploaded using separated RX buffers)
#define NX_AMSDU_DEAGG_LEN  ((NX_MAX_MSDU_PER_RX_AMSDU - 1) * 4)
#else
/// Length of de-aggregated A-MSDUs (not available)
#define NX_AMSDU_DEAGG_LEN  0
#endif /* NX_AMSDU_DEAGG */

#if NX_MON_DATA
/// Length of MAC Header descriptor (used only for MSDU in case of data+monitor interface)
#define RXU_MACHDRDESC_LEN  sizeof_b(struct rxu_machdrdesc)
#else
/// Length of MAC Header descriptor (not available)
#define RXU_MACHDRDESC_LEN  0
#endif /* NX_MON_DATA */
#endif /* NX_UMAC_PRESENT */

#if NX_UMAC_PRESENT
/// Length of the RX SW information
#define RXL_ADD_INFO_LEN    (sizeof_b(struct phy_channel_info) + 4 + RXU_MACHDRDESC_LEN + NX_AMSDU_DEAGG_LEN)
#else
/// Length of the RX SW information
#define RXL_ADD_INFO_LEN    sizeof_b(struct phy_channel_info)
#endif /* NX_UMAC_PRESENT */

/// Length of the receive vectors
#define RXL_HEADER_INFO_LEN   (RXL_HWDESC_RXV_LEN + RXL_LAST_DMA_PATTERN_LEN +           \
                               RXL_ADD_INFO_LEN)

/// Offset of the payload in the RX buffer
#if NX_FULLY_HOSTED
#define RXL_PAYLOAD_OFFSET     RXL_HEADER_INFO_LEN
#else
#define RXL_PAYLOAD_OFFSET    (RXL_HEADER_INFO_LEN + 2)
#endif

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Get the DMA descriptor attached to a received MPDU.
 *
 * @param[in] rxdesc   Pointer to the RX descriptor attached to the MPDU
 *
 * @return The pointer to the DMA descriptor
 ****************************************************************************************
 */
__INLINE struct rx_dmadesc *rxl_dmadesc_get(struct rxdesc *rxdesc)
{
    #if NX_RX_RING
    return &rxdesc->dma_hdrdesc;
    #else
    return rxdesc->dma_hdrdesc;
    #endif
}

/**
 ****************************************************************************************
 * @brief Initialize the pools of element
 * Function to initialize the Rx buffer and descriptor pools
 ****************************************************************************************
 */
void rxl_hwdesc_init(void);

/**
 ****************************************************************************************
 * @brief Do a SW copy of a received MPDU.
 *
 * @param[in] pbd         Pointer to the first payload descriptor of the MPDU
 * @param[in] length      Length to be copied
 * @param[in] offset      Offset inside the MPDU where to start the copy
 * @param[in] dst         Pointer to the destination buffer
 ****************************************************************************************
 */
void rxl_mpdu_copy(struct rx_pbd *pbd, uint16_t length, uint16_t offset, uint32_t *dst);

/**
 ****************************************************************************************
 * @brief Do a partial upload of a MPDU to a host buffer.
 *
 * This function performs the upload of a part of the MPDU. Once the upload is
 * done, the callback function passed as parameter is executed and the HW descriptors
 * linked to the MPDU are freed. This function is used by the BFR module to upload the
 * calibration reports, and the UMAC to upload the received fragments of a MSDU.
 *
 * @param[in] rxdesc      Pointer to the RX SW descriptor linked to the MPDU
 * @param[in] upload_len  Length to be uploaded
 * @param[in] hostbuf     Address in host memory where to upload the MPDU
 * @param[in] payl_offset Offset inside the MPDU where to start the upload
 * @param[in] cb          Function to be called upon upload completion
 * @param[in] env         Variable to be passed to the callback function
 ****************************************************************************************
 */
void rxl_mpdu_partial_transfer(struct rxdesc *rxdesc, uint16_t upload_len, uint32_t hostbuf,
                               uint16_t payl_offset, cb_rx_dma_func_ptr cb, void *env);

/**
 ****************************************************************************************
 * @brief This function programs the DMA transfer of the received payload to the host
 * memory.
 *
 * @param[in] rxdesc Pointer to SW descriptor attached to the received payload
 *
 * return true if need to upload, else drop.
 *
 ****************************************************************************************
 */
#if BK_MAC && BK_MIN_RX_BUFSZ
bool rxl_mpdu_transfer(struct rxdesc *rxdesc);
#else
void rxl_mpdu_transfer(struct rxdesc *rxdesc);
#endif

/**
 ****************************************************************************************
 * @brief Free the HW descriptors attached to the SW descriptor passed as parameter.
 *
 *This function releases both the RHD and RBD and requests chaining them to the HW.
 *
 * @param[in] rxdesc   Pointer to the rx SW descriptor for which the MPDU descriptors are freed
 ****************************************************************************************
 */
void rxl_mpdu_free(struct rxdesc *rxdesc);

/**
 ****************************************************************************************
 * @brief This function cleans up the complete frame data structures.
 *
 * @param[in] rxdesc Pointer to the structure that is holding the frame details.
 ****************************************************************************************
 */
void rxl_frame_release(struct rxdesc *rxdesc);

/**
 ****************************************************************************************
 * @brief Get the RX descriptor pointing to the first MPDU ready for processing.
 *
 * This function is called from the RXL control event prior to check if buffers are
 * available for upload. Once these checks are done and before any additional processing
 * is performed, the function @ref rxl_rxdesc_ready_for_processing shall be called.
 *
 * @return The RX descriptor pointing to the first MPDU ready for processing
 ****************************************************************************************
 */
struct rxdesc *rxl_rxdesc_get(void);

/**
 ****************************************************************************************
 * @brief Indicate that the MPDU associated to the descriptor is ready for processing.
 *
 * This function is called from the RXL control event after checking that the MPDU
 * associated to the descriptor is ready for processing, e.g. if host resources are
 * available for upload.
 *
 * @param[in] rxdesc The RX descriptor pointing to the MPDU
 ****************************************************************************************
 */
void rxl_rxdesc_ready_for_processing(struct rxdesc *rxdesc);

/**
 ****************************************************************************************
 * @brief Service routine for the MAC HW RX interrupt.
 *
 * This function clears the source of the interrupt and invokes the RX receive path
 * operations by setting an event that is scheduled by the kernel.
 ****************************************************************************************
 */
void rxl_mpdu_isr(void);

/**
 ****************************************************************************************
 * @brief Function used to get the immediate frame that was indicated from its dedicated
 * interrupt.
 *
 * This function shall be called from the interrupt handlers indicating the reception of
 * a BlockAck or a HE trigger frame.
 ****************************************************************************************
 */
void rxl_immediate_frame_get(void);

/**
 ****************************************************************************************
 * @brief This function returns the pointer to the first RX Header and Buffer
 * descriptors chained to the MAC HW.
 * It is used for error logging when an issue is detected in the LMAC.
 *
 * @param[out] rhd Pointer to the first RX Header descriptor chained to the MAC HW
 * @param[out] rbd Pointer to the first RX Buffer descriptor chained to the MAC HW
 *
 ****************************************************************************************
 */
void rxl_current_desc_get(struct rx_hd **rhd, struct rx_pbd **rbd);

/**
 ****************************************************************************************
 * @brief This function is called when an interface is changing its type, in order to
 * configure correctly the RX path for either monitor or active mode.
 *
 * In case monitor mode is enabled, the BA and other control frames are not handled
 * internally anymore, but forwarded to the upper layers. This mode shall therefore not
 * be used when monitoring is enabled in parallel of an active STA or AP interface.
 *
 * @param[in] enable Flag indicating if monitor mode is enabled or not
 ****************************************************************************************
 */
void rxl_hwdesc_monitor(bool enable);

/// @} // end of group RXHWDESC

#endif // _RXL_HWDESC_H_

