/**
 ****************************************************************************************
 * @file rxl_cntrl.h
 *
 * @brief API declarations for the Rx path.
 *
 * Copyright (C) RivieraWaves 2011-2020
 ****************************************************************************************
 */

#ifndef _RXL_CNTRL_H_
#define _RXL_CNTRL_H_

/**
 ****************************************************************************************
 * @defgroup RX_CNTRL RX_CNTRL
 * @ingroup RX
 * @brief Initialization and control of LMAC SW Rx path.
 * @{
 * It is responsible to initialize the data structures required for the RX path. These include:
 * -Tuple initialization
 * -TID Info table initialization
 * -LMAC SW Rx descriptor initialization
 * -Defragmentation table initialization
 *
 * It initializes the Rx context that includes the lists maintained at SW to host the received
 * frames and the HW descriptor details.
 * It configures the MAC HW registers for the Rx operation.
 *
 * Controls the operations to be performed on the received frame.
 *
 * It is responsible to validate the frame and control the sequence of operations to be
 * performed in the receive and the deferred context.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"

// for co_list
#include "co_list.h"

// for co_ring
#include "co_ring.h"

#include "dma.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// DMA channel used for data upload
#if NX_FULLY_HOSTED
#define RX_DATA_UPLOAD_CHAN IPC_DMA_CHANNEL_INTERNAL
#else
#define RX_DATA_UPLOAD_CHAN IPC_DMA_CHANNEL_DATA_RX
#endif

/// Pattern indicating to the host that the buffer is ready for him
#define DMA_HD_RXPATTERN 0xAAAAAA00

/// RX upload control flag bits
enum rx_upload_flags
{
    /// No upload was done on this buffer
    RX_NO_UPLOAD = CO_BIT(0),
};

/*
 * MACROS
 ****************************************************************************************
 */

#if (NX_UMAC_PRESENT)
/// Set bit in frame_info value (struct rxl_cntrl_rx_status)
#define RXL_CNTRL_FRAME_INFO_SET(bit)                           \
    (rxl_cntrl_env.rx_status.frame_info |= RXL_CNTRL_ ## bit)

/// Get if bit set to 1 in frame_info value (struct rxl_cntrl_rx_status)
#define RXL_CNTRL_FRAME_INFO_GET(bit)                           \
    (rxl_cntrl_env.rx_status.frame_info & RXL_CNTRL_ ## bit)
#endif //(NX_UMAC_PRESENT)

/// Configure the bridge DMA control field
#define RX_LLICTRL(irqenable)                                                            \
    ((irqenable)?(IPC_DMA_LLI_COUNTER_EN|(IPC_DMA_LLI_DATA_RX0<<IPC_DMA_LLI_COUNTER_POS) \
                | IPC_DMA_LLI_IRQ_EN|(IPC_DMA_LLI_DATA_RX0 << IPC_DMA_LLI_IRQ_POS)) : 0)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/*
 * TYPES DECLARATION
 ****************************************************************************************
 */
// forward declarations
struct rxdesc;

/// Pointer to callback function
typedef void (*cb_rx_dma_func_ptr)(void *);

/// RX control environment declaration.
struct rxl_cntrl_env_tag
{
    /// Frames that are being transfered to host memory.
    struct co_list upload_pending;
    /// The bridge DMA count
    uint16_t bridgedmacnt;
    #if !NX_FULLY_HOSTED
    /// The number of packets pending for signaling to host
    uint8_t packet_cnt;
    /// The number of packets above which the received packets are indicated to the host
    uint8_t packet_thd;
    #endif
};

/// Descriptor used to handle the actions performed when uploading the MPDU.
struct rx_upload_cntrl_tag
{
    /// Pointer used to form the list.
    struct co_list_hdr list_hdr;
    /// Function to be called upon frame upload
    cb_rx_dma_func_ptr cb;
    /// Pointer to pass to the upload callback
    void *env;
    /// Pointer to the associated RX SW Descriptor (used to know if a payload transfer
    /// is attached to this descriptor)
    struct rxdesc *rxdesc;
    /// Upload control flags (see @ref rx_upload_flags)
    uint8_t flags;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
/// RXL module environment variable
extern struct rxl_cntrl_env_tag rxl_cntrl_env;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Push an upload control descriptor to the pending queue.
 *
 * @param[in] upload_cntrl Pointer to the upload control descriptor
 ****************************************************************************************
 */
__INLINE void rxl_upload_cntrl_push_pending(struct rx_upload_cntrl_tag *upload_cntrl)
{
    // Insert the element in the pending list
    co_list_push_back(&rxl_cntrl_env.upload_pending, &upload_cntrl->list_hdr);
}

/**
 ****************************************************************************************
 * @brief Check if some packets are pending for upload.
 *
 * @return true if packets are pending, false otherwise
 ****************************************************************************************
 */
__INLINE bool rxl_upload_pending(void)
{
    return (!co_list_is_empty(&rxl_cntrl_env.upload_pending));
}

/**
 ****************************************************************************************
 * @brief This function initializes the RX path.
 ****************************************************************************************
 */
void rxl_init(void);

/**
 ****************************************************************************************
 * @brief RX path reset function.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It flushes all the packets currently in the RX path and exits when all of them
 * have been indicated to the driver
 ****************************************************************************************
 */
void rxl_reset(void);

/**
 ****************************************************************************************
 * @brief RX path background packet handler.
 *
 * This function is a kernel event handler triggered upon packet reception. It performs
 * the platform DMA programming for the upload of the packet to the host memory.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void rxl_cntrl_evt(int dummy);

/**
 ****************************************************************************************
 * @brief Update uploaded packet counter upon a frame upload and call the indication
 * function if the counter passes the threshold.
 *
 * @param[in] env Parameter not used but required to follow the upload confirmation
 * callback format
 ****************************************************************************************
 */
void rxl_host_irq_mitigation_update(void *env);

/**
 ****************************************************************************************
 * @brief Handling of the SW RX timeout.
 *
 * This function is called when the SW RX timeout expires, that indicates that a pending
 * A-MPDU reception is over. In this handler we warn the host about the packets that are
 * available.
 ****************************************************************************************
 */
void rxl_timeout_int_handler(void);

/**
 ****************************************************************************************
 * @brief RX IPC DMA interrupt handler.
 *
 * This function sets an event to schedule the processing of the DMA'd packets in
 * background.
 ****************************************************************************************
 */
void rxl_dma_int_handler(void);

/**
 ****************************************************************************************
 * @brief RX DMA event handler.
 *
 * This function is used as a deferred processing of the DMA interrupt from the DMA engine.
 * It processes the DMA'd packets.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void rxl_dma_evt(int dummy);
#if BK_MAC
int8_t rxl_get_beacon_rssi(void);

#endif


/// @}

#endif // _RXL_CNTRL_H_
