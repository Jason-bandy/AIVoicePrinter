/**
 ****************************************************************************************
 * @file td.h
 *
 * @brief Traffic Detection (TD) Module
 *
 * Copyright (C) RivieraWaves 2015-2020
 *
 ****************************************************************************************
 */

#ifndef _TD_H_
#define _TD_H_

/**
 *****************************************************************************************
 * @defgroup TD TD
 * @ingroup LMAC
 * @brief TD module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwnx_config.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Traffic Detection Interval default value (in TU)
#define TD_DEFAULT_INTV_TU          100

/// Traffic Detection Interval value (in microseconds)
#define TD_DEFAULT_INTV_US          (TD_DEFAULT_INTV_TU * TU_DURATION)

/// Default threshold (number of RX or TX packets needed to consider that
/// traffic is occurring, for one Traffic Detection interval)
#define TD_DEFAULT_PCK_NB_THRES     3

/// Factor used to compute the threshold for frames received in PS mode (because in this
/// case frames are received in burst at DTIM) - This value is actually the threshold
/// for one TU, multiplied by 65536
#define TD_FACT (((TD_DEFAULT_PCK_NB_THRES << 16) / (TD_DEFAULT_INTV_TU)) + 1)  // +1 just so that can skip rounding

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Bit index for Traffic Status
enum td_status_bit
{
    /// TX Traffic Detected (overall)
    TD_STATUS_TX    = 0,
    /// RX Traffic Detected (overall)
    TD_STATUS_RX,
    /// TX Traffic Detected (with PS filtering)
    TD_STATUS_TX_PS,
    /// RX Traffic Detected (with PS filtering)
    TD_STATUS_RX_PS,
};

#if (NX_TD)

#include "mm_timer.h"

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

/**
 * Structure containing all information about traffic detection for a given interface
 */
struct td_env_tag
{
    /// Timer used for Traffic Detection Interval
    struct mm_timer_tag td_timer;

    /// Number of packets transmitted during TD interval (overall)
    uint32_t pck_cnt_tx;
    /// Number of packets received during TD interval (overall)
    uint32_t pck_cnt_rx;

    #if (NX_DPSM)
    /// Number of packets transmitted during TD interval, PS filtering applied (UAPSD,...)
    uint32_t pck_cnt_tx_ps;
    /// Number of packets received during TD interval, PS filtering applied (UAPSD,...)
    uint32_t pck_cnt_rx_ps;
    /// Threshold above which PS RX packets can trigger the DPSM
    uint32_t pck_cnt_rx_ps_thres;
    #endif //(NX_DPSM)

    /// VIF Index
    uint8_t vif_index;
    /// Bit field indicating traffic status - see enum td_status_bit
    uint8_t status;

#if BK_MAC
    /// Bit field indicating traffic old status
    uint8_t old_status;
    /// Bit field indicating traffic now status
    uint8_t now_status;
#endif

    /// Indicate if the Traffic Detection is activated for a VIF (<=> td_timer running)
    bool is_on;

    #if (NX_CHNL_CTXT)
    /// Indicate if VIF has been present on its channel context during TD interval
    bool has_active_chan;
    #endif //(NX_CHNL_CTXT)
};


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/// List of TD Environment Entries, one for each VIF that can be created
extern struct td_env_tag td_env_tab[NX_VIRT_DEV_MAX];

/*
 * PUBLIC FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the traffic detection module.
 ****************************************************************************************
 */
void td_init(void);

/**
 ****************************************************************************************
 * @brief Reset Traffic Detection status for a given VIF.
 * @param[in] vif_index     Index of VIF on which traffic status has to be reseted
 ****************************************************************************************
 */
void td_reset(uint8_t vif_index);

/**
 ****************************************************************************************
 * @brief Start Traffic Detection on a given VIF.
 * @param[in] vif_index     Index of VIF on which traffic has to be monitored
 ****************************************************************************************
 */
void td_start(uint8_t vif_index);

/**
 ****************************************************************************************
 * @brief Handle reception or transmission of a packet on a given interface.
 *
 * @param[in] vif_index    Index of the VIF on which packet has been received/transmitted
 * @param[in] sta_index    Index of the STA on which packet has been received/transmitted
 * @param[in] rx           True if packet has been received, false if it has been transmitted
 ****************************************************************************************
 */
void td_pck_ind(uint8_t vif_index, uint8_t sta_index, bool rx);

#if NX_DPSM
/**
 ****************************************************************************************
 * @brief Indicate to the TD the beacon interval and DTIM period, so that it can compute
 * the actual threshold of received packets that should trigger the DPSM.
 *
 * @param[in] vif_index     Index of the VIF on which the threshold is computed
 * @param[in] bcn_int       Beacon interval, in TU
 * @param[in] wakeup_period DTIM period or listen interval, indicating every how many
 * beacons the SW wakes up to receive buffered traffic.
 ****************************************************************************************
 */
void td_set_rx_ps_thres(uint8_t vif_index, uint32_t bcn_int, uint8_t wakeup_period);

/**
 ****************************************************************************************
 * @brief Handle reception or transmission of a packet on a given interface in a PS context.
 *        This function should not be called for packet whose queue is UAPSD enabled.
 *
 * @param[in] vif_index    Index of the VIF on which packet has been received/transmitted
 * @param[in] rx           True if packet has been received, false if it has been transmitted
 ****************************************************************************************
 */
void td_pck_ps_ind(uint8_t vif_index, bool rx);
#endif //(NX_DPSM)

/**
 ****************************************************************************************
 * @brief Get traffic status for a VIF
 *
 * @param[in] vif_index  VIF index
 * @return Whether RX or TX traffic occured on the vif during last interval
 ****************************************************************************************
 */
__INLINE uint8_t td_get_status(uint8_t vif_index)
{
    return (td_env_tab[vif_index].status & (CO_BIT(TD_STATUS_TX) | CO_BIT(TD_STATUS_RX)));
}

/**
 ****************************************************************************************
 * @brief Get traffic status for a VIF after PS filtering
 *
 * @param[in] vif_index  VIF index
 * @return Whether enough RX or TX traffic occured on the vif during last interval to
 * leave PS mode
 ****************************************************************************************
 */
__INLINE uint8_t td_get_ps_status(uint8_t vif_index)
{
    return (td_env_tab[vif_index].status & (CO_BIT(TD_STATUS_TX_PS) | CO_BIT(TD_STATUS_RX_PS)));
}

#endif //(NX_TD)

#if (NX_TD_STA)
/**
 * Structure containing all information about traffic detection for a given link
 */
struct td_sta_env_tag
{
    /// Status (@see enum td_status_bit)
    uint8_t status;
    /// Counter allowing to monitor if number of packets transmitted during TD interval has reached the detection threshold
    uint8_t pck_cnt_tx;
    /// Counter allowing to monitor if number of packets received during TD interval has reached the detection threshold
    uint8_t pck_cnt_rx;
};

/// List of TD Environment Entries used for per-STA traffic detection
extern struct td_sta_env_tag td_sta_env[NX_REMOTE_STA_MAX];

/**
 ****************************************************************************************
 * @brief Reset Traffic Detection status for a given STA.
 *
 * @param[in] sta_idx  Index of STA on which traffic status has to be reset.
 ****************************************************************************************
 */
void td_sta_reset(uint8_t sta_idx);

/**
 ****************************************************************************************
 * @brief Check if there is TX traffic for a given STA.
 *
 * @param[in] sta_idx  Index of STA for which the TX status is checked.
 * @return Whether the STA has TX traffic
 ****************************************************************************************
 */
__INLINE bool td_sta_has_tx_traffic(uint8_t sta_idx)
{
    return (td_sta_env[sta_idx].status & CO_BIT(TD_STATUS_TX));
}

/**
 ****************************************************************************************
 * @brief Check if there is TX or RX traffic for a given STA.
 *
 * @param[in] sta_idx  Index of STA for which the TX and RX status is checked.
 * @return Whether the STA has TX or RX traffic
 ****************************************************************************************
 */
__INLINE bool td_sta_has_traffic(uint8_t sta_idx)
{
    return ((td_sta_env[sta_idx].status & (CO_BIT(TD_STATUS_TX) | CO_BIT(TD_STATUS_RX))) != 0);
}
#endif //(NX_TD_STA)

#if BK_MAC
/* * @brief Traffic Detection timer callback.
 *
 * This function checks the various TX and RX statistics accumulated over the last period
 * and proceeds to the required actions (wake up, channel scheduling, etc.).
 *
 * @param[in] env     Pointer to the TD environment structure
 ****************************************************************************************
 */
void td_timer_end(void *env);
void td_pck_ps_clr(uint8_t vif_index, bool rx);
uint32_t td_cal_stay_count(uint8_t index);
#endif

/// @} end of group

#endif // _TD_H_
