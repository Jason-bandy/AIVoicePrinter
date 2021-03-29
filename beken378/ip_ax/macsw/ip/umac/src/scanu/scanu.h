/**
 ****************************************************************************************
 *
 * @file scanu.h
 *
 * @brief Declaration of the Template module environment.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */
#ifndef _SCANU_H_
#define _SCANU_H_

/** @defgroup SCANU SCANU
 * @ingroup UMAC
 * @brief Declaration of all structures and functions used by the SCAN module.
 * @{
 */
#include "co_int.h"
#include "co_bool.h"
#include "hal_desc.h"

// for ke_task_id_t
#include "ke_msg.h"

// for mac_scan_result and other structures
#include "mac.h"

#include "hal_dma.h"

/// Maximum length of the additional ProbeReq IEs
#define SCANU_MAX_IE_LEN  200
#if BK_MAC
#define DIS_HT_SCAN_RST_RSSI_THRED          (-80)

#define TYPE_ACTIVE_SCAN                     1
#define TYPE_PASSIVE_SCAN                    2
#endif

///Maximum number of scan results that can be stored.
#define SCANU_MAX_RESULTS 32

///Maximum number of nonTxed BSSID information we can store for one received beacon
#define SCANU_MAX_NONTXED_BSSID_PER_BEACON 16

struct rxu_mgt_ind;

#if BK_MAC
/** The SCAN environment.
 * This environment can be used by any element that requires the SCAN.
 */
struct scanu_setting
{
    /* active-scan or passive-scan*/
    uint16_t scan_method;

    /* 1 or 2*/
    uint16_t probe_cnt;

    /* delay before switching on a channel*/
    uint32_t chan_delay;
    uint32_t joining_chan_delay;

    /* scan duration on each channel*/
    uint32_t scan_time;
};
#endif

/// nonTransmitted BSSID information structure
struct scanu_mbssid_profile_tag
{
    /// Address of the SSID element in the Beacon/ProbeRsp frame
    uint32_t ssid_ie_addr;
    /// Capability information
    uint16_t capa;
    /// nonTransmitted BSSID index
    uint8_t bssid_index;
};

/// nonTransmitted BSSID database
struct scanu_mbssids_tag
{
    /// Information about each nonTransmitted BSSID
    struct scanu_mbssid_profile_tag bssids[SCANU_MAX_NONTXED_BSSID_PER_BEACON];
    /// Maximum BSSID indicator
    uint8_t max_bssid_ind;
    /// Number of nonTransmitted BSSIDs stored
    uint8_t mbssid_cnt;
};

/// Type of SCANU request
enum scanu_request_type
{
    /// Normal scan request
    SCANU_NO_JOIN,
    /// Internal Scan request prior to connection
    SCANU_JOIN,
};

/** The SCAN environment.
 * This environment can be used by any element that requires the SCAN.
 */
struct scanu_env_tag
{
    /// Parameters of the Scan request
    struct scanu_start_req const *param;
    /// GP DMA descriptor
    struct hal_dma_desc_tag dma_desc;
    /// The number of scan results obtained from LMAC
    uint16_t result_cnt;
    /// Array containing the results of the scan
    struct mac_scan_result scan_result[SCANU_MAX_RESULTS];
    /// Task ID of the sender of the scan request.
    ke_task_id_t src_id;
    /// Type of scan request
    enum scanu_request_type req_type;
    /// Band currently scanned
    uint8_t band;
#if BK_MAC
    /// is send_cancel
    uint8_t send_cancel;
#endif
    /// BSSID looked for.
    struct mac_addr bssid;
    /// SSID looked for.
    struct mac_ssid ssid;
    /// Reference BSSID looked for.
    struct mac_addr ref_bssid;
    #if (NX_P2P)
    /// P2P Scan (ssid we are looking for is "DIRECT-")
    bool p2p_scan;
    #endif //(NX_P2P)
    /// Join procedure status (1: success, 0: failure)
    bool join_status;
    /// nonTransmitted BSSID database
    struct scanu_mbssids_tag mbssids;
#if BK_MAC
    struct scanu_setting setting;
#endif
};

/// Definition of an additional IE buffer
struct scanu_add_ie_tag
{
    /// DMA descriptor to download the IEs from host memory
    struct dma_desc dma_desc;
    /// Buffer space for IEs
    uint32_t buf[SCANU_MAX_IE_LEN/4];
};

/// Scan time to enable or disable the scan (2sec).
#define SCAN_ENABLE_TIME 2000000

/// SCAN module environment declaration.
extern struct scanu_env_tag scanu_env;

/// Variable used to store the addition information elements downloaded.
extern struct scanu_add_ie_tag scanu_add_ie;
#if BK_MAC
extern struct scanu_env_tag scanu_env;
#endif

/**
 ****************************************************************************************
 * @brief Initialize the SCAN environment and task.
 *
 ****************************************************************************************
 */
void scanu_init(void);

/**
 ****************************************************************************************
 * @brief Handle the reception of a beacon or probe response frame.
 *
 * It extracts all required information from them and save them in scan result array.
 *
 * @param[in] frame Pointer to the received beacon/probe-response message.
 *
 * @return The message status to be returned to the kernel (@ref KE_MSG_CONSUMED or
 *         @ref KE_MSG_NO_FREE)
 ****************************************************************************************
 */
int scanu_frame_handler(struct rxu_mgt_ind const *frame);

/**
 ****************************************************************************************
 * @brief Look for a given BSS in the scan results and returns its parameters.
 *
 * @param[in] bssid_ptr Pointer to the BSSID looked for.
 * @param[in] allocate Set to true only a result location is required for allocation.
 *
 * @return If the allocation parameter was set, return the pointer to this BSS parameters
 * if the BSS exists or the pointer to the first free BSS entry.  In this case, if return
 * NULL, then no space was found.  If the allocation parameter was not set, return NULL
 * if the BSS was not found.
 ****************************************************************************************
 */
struct mac_scan_result* scanu_find_result(struct mac_addr const *bssid_ptr,
                                          bool allocate);

/**
 ****************************************************************************************
 * @brief Look for a given BSS from its BSSID.
 *
 * @param[in] bssid Pointer to the BSSID looked for.
 *
 * @return If found, return the pointer to the BSS parameters, and NULL otherwise
 ****************************************************************************************
 */
struct mac_scan_result *scanu_search_by_bssid(struct mac_addr const *bssid);

/**
 ****************************************************************************************
 * @brief Look for a given BSS from its SSID.
 * If several entries in the database have the same SSID, then the function returns the
 * one having the highest RSSI.
 *
 * @param[in] ssid Pointer to the SSID looked for.
 *
 * @return If found, return the pointer to the BSS parameters, and NULL otherwise
 ****************************************************************************************
 */
struct mac_scan_result *scanu_search_by_ssid(struct mac_ssid const *ssid);

#if BK_MAC
uint32_t scanu_is_active_scan(void);
uint32_t scanu_is_passive_scan(void);
bool scanu_is_joining(void);
uint32_t scanu_is_more_probe_request(void);
#endif

/**
 ****************************************************************************************
 * @brief Start scanning process.
 ****************************************************************************************
 */
void scanu_start(void);

/**
 ****************************************************************************************
 * @brief Send a scan request to LMAC for the next band to scan
 ****************************************************************************************
 */
void scanu_scan_next(void);

/**
 ****************************************************************************************
 * @brief Send the appropriate confirmation to the source task.
 *
 * @param[in] status  Status to be sent to the source task
 ****************************************************************************************
 */
void scanu_confirm(uint8_t status);


/// @} end of group

#endif // _SCANU_H_
