/**
 ****************************************************************************************
 * @file scan.h
 *
 * @brief Scanning module definitions.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _SCAN_H_
#define _SCAN_H_

/**
 *****************************************************************************************
 * @defgroup SCAN SCAN
 * @ingroup LMAC
 * @brief LMAC Scanning module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"
#include "co_bool.h"
#include "co_utils.h"

#include "mac.h"
#include "ke_timer.h"
#include "mm_task.h"
#include "hal_desc.h"
#include "hal_dma.h"

#include "dma.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Maximum number of SSIDs in a scan request
#define SCAN_SSID_MAX   2

/// Maximum number of channels in a scan request
#define SCAN_CHANNEL_MAX (MAC_DOMAINCHANNEL_24G_MAX + MAC_DOMAINCHANNEL_5G_MAX)

/// Maximum length of the ProbeReq IEs
#define SCAN_MAX_IE_LEN 300

/// Default duration on channel (in us) when actively scanning
#if BK_MAC
#define SCAN_ACTIVE_DURATION    90000   // 90 ms
#else
#define SCAN_ACTIVE_DURATION    30000   // 30 ms
#endif

/// Default duration on channel (in us) when passively scanning
#define SCAN_PASSIVE_DURATION   110000

/// Definition of a ProbeReq IE buffer
struct scan_probe_req_ie_tag
{
    /// DMA descriptor to download the IEs from host memory
    struct dma_desc dma_desc;
    /// TX payload buffer descriptor
    struct tx_pbd pbd;
    /// Buffer space for IEs
    uint32_t buf[SCAN_MAX_IE_LEN/4];
};

/// SCAN Context
struct scan_env_tag
{
    /// GP DMA descriptors
    struct hal_dma_desc_tag dma_desc;
    /// Pointer to the scanning parameters
    struct scan_start_req const *param;
    /// Address of the DS information element
    uint32_t ds_ie;
    /// Task identifier of the scanning requester
    ke_task_id_t req_id;
    /// Current channel index
    uint8_t chan_idx;
    /// Abort scan requested by host ?
    bool abort;
#if BK_MAC
    /// Count scan ?
    uint8_t count;
#endif
};

#if NX_HW_SCAN
/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct scan_env_tag scan_env;

/// Global variable to store probe request frame downloaded from host memory
extern struct scan_probe_req_ie_tag scan_probe_req_ie;


/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief SCAN Module main initialization function.
 * This function is called after reset and initializes all SCAN related env and data.
 ****************************************************************************************
 */
void scan_init(void);

/**
 ****************************************************************************************
 * @brief Download the information elements that will need to be added to the ProbeReq.
 *
 * @param[in] param Pointer to the scan parameters
 ****************************************************************************************
 */
void scan_ie_download(struct scan_start_req const *param);


/**
 ****************************************************************************************
 * @brief Send the appropriate channel request message.
 ****************************************************************************************
 */
void scan_set_channel_request(void);


/**
 ****************************************************************************************
 * @brief Prepare and program for TX the ProbeReq required for active scanning
 ****************************************************************************************
 */
void scan_probe_req_tx(void);

/**
 ****************************************************************************************
 * @brief Sends the SCAN_CANCEL_CFM message to the SCAN_CANCEL_REQ task source
 *
 * @param[in] status  Status to include in the message
 * @param[in] dest_id ID of the requested task
 ****************************************************************************************
 */
void scan_send_cancel_cfm(uint8_t status, ke_task_id_t dest_id);

/**
 ****************************************************************************************
 * @brief Returns scan channel information currently used
 *
 * @param[out] duration  Duration, in us, of the current scan
 * @return Information of the channel that is currently scanned
 ****************************************************************************************
 */
struct mac_chan_def const *scan_get_chan(uint32_t *duration);
#endif

/// @} end of group

#endif // _SCAN_H_
