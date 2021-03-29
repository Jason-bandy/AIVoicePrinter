/**
****************************************************************************************
*
* @file rc.h
*
* Copyright (C) RivieraWaves 2015-2020
*
* @brief Declaration of the initialization functions used in UMAC RC.
*
****************************************************************************************
*/

#ifndef _RC_ENABLE_H_
#define _RC_ENABLE_H_

/**
 *****************************************************************************************
 * @defgroup RC RC
 * @ingroup UMAC
 * @brief Declaration of all functions used for rate control algorithm.
 * @{
 *****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"

// for number of rate control steps
#include "hal_desc.h"
// for struct sta_pol_tbl_cntl and struct sta_info_tag
#include "sta_mgmt.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Set 1 to always use maximum allowed BW
#define RC_USE_MAX_BW         0
/// Maximum number of retries of each step of the retry chain
#define RC_MAX_NUM_RETRY         (2)
/// Rate control algorithm execution period (us)
/// @warning This value shall not exceed 200ms in order not to overflow
/// internal RC variables during statistics computation
#define RC_PERIOD_TOUT           (100000)
#if RC_PERIOD_TOUT > 200000
#error "RC_PERIOD_TOUT shall be 200ms maximum"
#endif

/// Rate control algorithm trial TX interval (number of TX)
#define RC_TRIAL_PERIOD          (10)
/// Maximum number of samples to be maintained in the statistics structure
#define RC_MAX_N_SAMPLE          (10)
/// Index of the HE statistics element in the table
#define RC_HE_STATS_IDX          RC_MAX_N_SAMPLE

/// Scaling used for the fractional computation
#define RC_SCALE                 (16)
/// Default weight used in the Exponentially Weighted Moving Average computation
#define EWMA_LEVEL               (96)
/// Divisor used in the Exponentially Weighted Moving Average computation
#define EWMA_DIV                 (128)

/**
 * @brief Compute a fractional number
 * @param[in] val Value to be divided
 * @param[in] div Divisor
 * @return The value of the fraction
 */
#define RC_FRAC(val, div)        (((val) << RC_SCALE) / (div))

/**
 * @brief Get the integer part of a fractional number
 * @param[in] val Value to be checked
 * @return The integer part of the number
 */
#define RC_TRUNC(val)            ((val) >> RC_SCALE)

/**
 * @brief Divide and round to the upper integer
 * @param[in] n Number to be divided
 * @param[in] d Divisor
 * @return The up-rounded result of the division
 */
#define DIV_ROUND_UP(n, d)       (((n) + (d) - 1) / (d))

/// Disabled fixed rate configuration
#define RC_FIXED_RATE_NOT_SET    (0xFFFF)

/// @name RC information flags
/// Flag indicating whether next transmission could be part of a AMPDU
#define RC_AGG_TX_BIT           CO_BIT(1)
/// Flag indicating whether aggregation is allowed by the rate control
#define RC_AGG_ALLOWED_BIT      CO_BIT(2)
/// Offset of fixed rate status
#define RC_FIX_RATE_STATUS_OFT   (4)
/// Mask of the fixed rate status
#define RC_FIX_RATE_STATUS_MASK  (3 << RC_FIX_RATE_STATUS_OFT)
/// Fixed rate not enabled
#define RC_FIX_RATE_NOTEN_MASK   (0 << RC_FIX_RATE_STATUS_OFT)
/// Fixed rate requested
#define RC_FIX_RATE_REQ_MASK     (1 << RC_FIX_RATE_STATUS_OFT)
/// Fixed rate enabled
#define RC_FIX_RATE_EN_MASK      (2 << RC_FIX_RATE_STATUS_OFT)
/// Flag indicating whether the rate control station statistics table needs to be updated
#define RC_SS_UPD_REQ_BIT       CO_BIT(6)
/// Flag indicating whether the rates should be re-initialised after the fixed rate period
#define RC_FIX_RATE_UPD_SS_REQ_BIT   CO_BIT(7)
/// Format of the DCM modulation offset
#define RC_DCM_MOD_OFT           14
/// Type of DCM modulation mask
#define RC_DCM_MOD_MASK          CO_BIT(RC_DCM_MOD_OFT)

/// @name RC control information inside TX descriptor
/// SW retry step offset
#define RC_SW_RETRY_STEP_OFT     0
/// SW retry step mask
#define RC_SW_RETRY_STEP_MSK     (0x03 << RC_SW_RETRY_STEP_OFT)
/// Retry chain version offset
#define RC_RETRY_CHAIN_VER_OFT   2
/// Retry chain version mask
#define RC_RETRY_CHAIN_VER_MSK   (0x03 << RC_RETRY_CHAIN_VER_OFT)
/// DCM offset
#define RC_DCM_TX_OFT            4
/// DCM mask
#define RC_DCM_TX_MSK            (0x01 << RC_DCM_TX_OFT)
/// Macro used to retrieve a field from the RC control field
#define RC_CONTROL_GET(rc_control, field) (((rc_control) & RC_##field##_MSK) >> RC_##field##_OFT)

/// @name RC trial states
/// Wait for the next time to transmit a trial rate
#define RC_TRIAL_STATUS_WAIT       (0)
/// Wait the trial rate transmission confirmation
#define RC_TRIAL_STATUS_WAIT_CFM   (1)
/// @}


/*
 * STRUCTURES/TYPES DEFINITION
 ****************************************************************************************
 */

/// Statistics table
struct rc_rate_stats
{
    /// Number of attempts (per sampling interval)
    uint16_t attempts;
    /// Number of success (per sampling interval)
    uint16_t success;
    /// Estimated probability of success (EWMA)
    uint16_t probability;
    /// Rate configuration of the sample
    uint16_t rate_config;
    union
    {
        struct {
            /// Number of times the sample has been skipped (per sampling interval)
            uint8_t  sample_skipped;
            /// Whether the old probability is available
            bool  old_prob_available;
            /// Whether the rate can be used in the retry chain
            bool rate_allowed;
        };
        struct {
            /// RU size and UL length received in the latest HE trigger frame
            uint16_t ru_and_length;
        };
    };
};

/// Rate control structure
struct rc_sta_stats
{
    /// Last time the RC has run
    uint32_t last_rc_time;
    /// Statistics table, per sample
    struct rc_rate_stats rate_stats[RC_MAX_N_SAMPLE + NX_HE];
    /// Retry chain steps
    uint16_t retry_step_idx[RATE_CONTROL_STEPS];
    /// Number of MPDUs transmitted (per sampling interval)
    uint16_t ampdu_len;
    /// Number of AMPDUs transmitted (per sampling interval)
    uint16_t ampdu_packets;
    /// Average number of MPDUs in each AMPDU frame (EWMA)
    uint32_t avg_ampdu_len;
    /// Trial transmission period
    uint16_t sample_wait;
    /// Counter used to regulate the use of lower rates for trial transmission
    uint8_t sample_slow;
    /// Status of the trial transmission
    uint8_t trial_status;
    /// Trial rate configuration
    uint32_t trial_rate;
    /// Index of the trial sample in the rate_stats table
    uint8_t trial_idx;
    /// Boolean indicating whether DCM is used in the trial sample
    bool trial_dcm;
    /// Information field:
    /// bit   0 : position of the trial entry in the retry chain (0=step 0, 1=step 1)
    /// bit   1 : whether next transmission could be part of a AMPDU
    /// bit   2 : whether aggregation is allowed by the rate control
    /// bit   3 : whether the number of SW retry request has been exceeded, and it's
    ///           necessary to use the next step of the retry chain for the next transmission
    /// bit 4-5 : fixed rate status
    /// bit   6 : whether the sample table needs to be updated
    uint8_t info;
    /// Current version of the retry chain
    uint8_t retry_chain_ver;
    /// Current step 0 of the retry chain
    uint8_t sw_retry_step;
    /// Format and modulation of the station
    uint8_t format_mod;
    /// Allowed HT/VHT rates
    union
    {
        ///   Bit 0-31: MCS0-MCS31 bitmask
        ///             MCS0-7   : 1SS
        ///             MCS8-15  : 2SS
        ///             MCS16-23 : 3SS
        ///             MCS24-31 : 4SS
        uint8_t     ht[4];
        #if NX_VHT
        ///   Bit  0- 1: 1 SS (0=MCS 0-7, 1=MCS 0-8, 2=MCS 0-9, 3=SS not supported)
        ///   Bit  2- 3: 2 SS
        ///   Bit  4- 5: 3 SS
        ///   Bit  6- 7: 4 SS
        ///   Bit  8- 9: 5 SS
        ///   Bit 10-11: 6 SS
        ///   Bit 12-13: 7 SS
        ///   Bit 14-15: 8 SS
        uint16_t    vht;
        #endif
        #if NX_HE
        ///   Bit  0- 1: 1 SS (0=MCS 0-7, 1=MCS 0-9, 2=MCS 0-11, 3=SS not supported)
        ///   Bit  2- 3: 2 SS
        ///   Bit  4- 5: 3 SS
        ///   Bit  6- 7: 4 SS
        ///   Bit  8- 9: 5 SS
        ///   Bit 10-11: 6 SS
        ///   Bit 12-13: 7 SS
        ///   Bit 14-15: 8 SS
        uint16_t    he;
        #endif
    } rate_map;
    /// Allowed legacy rates
    /// Bit 0-3 : 1, 2, 5.5, 11 Mbps CCK
    /// Bit 4-11: 6-54 Mbps OFDM
    uint16_t rate_map_l;
    /// Maximum MCS supported (0xFF if HT rates not supported)
    uint8_t mcs_max;
    /// Minimum rate index supported
    uint8_t r_idx_min;
    /// Maximum rate index supported
    uint8_t r_idx_max;
    /// Maximum BW supported - 0: 20MHz, 1: 40MHz, 2: 80MHz, 3: 160MHz
    uint8_t bw_max;
    /// Number of spatial streams supported minus 1
    uint8_t no_ss;
    /// Short GI supported
    uint8_t short_gi;
    /// Preamble type supported - 0: short and long, 1: only long
    uint8_t type;
    /// Number of samples for statistics
    uint16_t no_samples;
    /// Maximum allowed AMSDU size
    uint16_t max_amsdu_len;
    /// Currently allowed AMSDU size
    uint16_t curr_amsdu_len;
    /// Fixed rate configuration - 0xFFFF if disabled
    uint16_t fixed_rate_cfg;
    /// Number of A-MSDU currently in the TX lists (or programmed as retry)
    uint16_t amsdu_cnt;
    /// DCM supported
    bool dcm;
    /// Maximum MCS supported in DCM
    uint8_t dcm_mcs_max;
    /// Maximum number of spatial streams supported in DCM
    uint8_t dcm_no_ss_max;
    /// Maximum BW supported in DCM - 0: 20MHz, 1: 40MHz, 2: 80MHz, 3: 160MHz
    uint8_t dcm_bw_max;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Gets the maximum AMSDU length we can transmit to a given station.
 *
 * @param[in] sta       Pointer to the station entry
 *
 * @return Maximum AMSDU length. Value 0 means that AMSDU are not allowed.
 ****************************************************************************************
 */
__INLINE uint16_t rc_get_max_amsdu_len(struct sta_info_tag *sta)
{
    struct sta_pol_tbl_cntl *pt = &sta->pol_tbl;
    struct rc_sta_stats *rc_ss = pt->sta_stats;

    return rc_ss->curr_amsdu_len;
}

/**
 ****************************************************************************************
 * @brief Gets the pointer to the rate control station statistics structure.
 *
 * @param[in] sta_idx Station index
 *
 * @return Pointer to the RC station statistics structure
 ****************************************************************************************
 */
__INLINE struct rc_sta_stats* rc_get_sta_stats(uint8_t sta_idx)
{
    struct sta_info_tag *sta = &sta_info_tab[sta_idx];
    struct sta_pol_tbl_cntl *rc = &sta->pol_tbl;
    struct rc_sta_stats *rc_ss = rc->sta_stats;
    ASSERT_ERR(rc_ss != NULL);

    return rc_ss;
}

/**
 ****************************************************************************************
 * @brief Gets the aggregation enabled flag for given station
 *
 * @param[in] sta       Pointer to the station entry
 *
 * @return Whether the aggregation is allowed or not.
 ****************************************************************************************
 */
__INLINE bool rc_check_aggregation(struct sta_info_tag *sta)
{
    struct sta_pol_tbl_cntl *pt = &sta->pol_tbl;
    struct rc_sta_stats *rc_ss = pt->sta_stats;

    return (rc_ss->info & RC_AGG_ALLOWED_BIT);
}

/**
 ****************************************************************************************
 * @brief Update the number of A-MSDU pending for a Station
 *
 * @param[in] sta_idx  Station Index
 * @param[in] inc      Number of A-MSDU increment
 ****************************************************************************************
 */
__INLINE void rc_update_amsdu_cnt(uint8_t sta_idx, int inc)
{
    #if NX_AMSDU_TX
    if (sta_idx < NX_REMOTE_STA_MAX)
    {
        rc_get_sta_stats(sta_idx)->amsdu_cnt += inc;
    }
    #endif
}
/**
 ****************************************************************************************
 * @brief RC algorithm initialization.
 * This function sets the pointer to the statics structure for the station, reads the
 * capability informations about the station (format, max bw, max spatial streams, max
 * MCS, guard interval, min and max rate index, preamble type) and sets them into the RC
 * statistics structure, initializes the rate control algorithm.
 *
 * @param[in] sta       pointer to STA Info Table
 ****************************************************************************************
 */
void rc_init(struct sta_info_tag *sta);

/**
 ****************************************************************************************
 * @brief Gets the TX duration of a packet of 1200 bytes.
 * @param[in] rate_stats Rate statistics element (including the rate configuration info)
 * @return Transmission duration in nsecs.
 ****************************************************************************************
 */
uint32_t rc_get_duration(struct rc_rate_stats *rate_stats);

/**
 ****************************************************************************************
 * @brief Updates Rate Control statistics after sending a singleton frame
 *
 * @param[in] txdesc TX descriptor of the singleton sent
 ****************************************************************************************
 */
void rc_cfm_singleton(struct txdesc *txdesc);

/**
 ****************************************************************************************
 * @brief Updates Rate Control statistics after sending an A-MPDU
 *
 * @param[in] txdesc TX descriptor of the first MPDU of the A-MPDU
 * @param[in] txed Number of frame transmitted in the A-MDPU
 * @param[in] txok Number of frame successfully transmitted in the A-MDPU
 ****************************************************************************************
 */
void rc_cfm_ampdu(struct txdesc *txdesc, uint32_t txed, uint32_t txok);

/**
 ****************************************************************************************
 * @brief RC algorithm check.
 *
 * This function shall be called upon queuing of every packets within the TX path.
 * It checks if:
 * - it's time to update statistics and select the new retry chain steps;
 * - it's time to select the next step of the retry chain because the max number of
 *     retries for the AMPDU has been reached;
 * - it's time to do a trial transmission.
 *
 * @param[in,out] txdesc  TX descriptor of the packet to be transmitted
 ****************************************************************************************
 */
void rc_check(struct txdesc *txdesc);

/**
 ****************************************************************************************
 * @brief Updates the maximum bandwidth and spatial streams allowed by the station
 *
 * @param[in] sta_idx index of the station
 * @param[in] bw_max maximum bandwith
 * @param[in] nss_max maximum number of spatial streams
 ****************************************************************************************
 */
void rc_update_bw_nss_max(uint8_t sta_idx, uint8_t bw_max, uint8_t nss_max);

/**
 ****************************************************************************************
 * @brief Updates the preamble type allowed by the station
 *
 * @param[in] sta_idx index of the station
 * @param[in] preamble_type preamble type to set
 ****************************************************************************************
 */
void rc_update_preamble_type(uint8_t sta_idx, uint8_t preamble_type);

/**
 ****************************************************************************************
 * @brief Initializes the RC algorithm for BC/MC transmissions
 *
 * @param[in] sta            Pointer to the station entry
 * @param[in] basic_rate_idx Rate index of the basic rate to be set
 ****************************************************************************************
 */
void rc_init_bcmc_rate(struct sta_info_tag *sta, uint8_t basic_rate_idx);

/**
 ****************************************************************************************
 * @brief Calculates the throughput of an entry of the sample table.
 * Calculates the throughput based on the average A-MPDU length, taking into account
 * the expected number of retransmissions and their expected length.
 *
 * @param[in] rc_ss      Pointer to rate control station statistics structure
 * @param[in] sample_idx Index of the entry of the sample table
 *
 * @return Calculated throughput
 ****************************************************************************************
 */
uint32_t rc_calc_tp(struct rc_sta_stats *rc_ss, uint8_t sample_idx);

/**
 ****************************************************************************************
 * @brief Checks whether the requested fixed rate is compliant with the peer STA
 * capabilities.
 *
 * @param[in] rc_ss             Pointer to rate control station statistics structure
 * @param[in] fixed_rate_config Fixed rate configuration
 *
 * @return true if configuration is OK, false otherwise
 ****************************************************************************************
 */
bool rc_check_fixed_rate_config(struct rc_sta_stats *rc_ss, uint16_t fixed_rate_config);

/**
 ****************************************************************************************
 * @brief Checks if STBC, beamforming and +HTC can be used for the trial transmission and
 * sets the tx_flags in the buffer control structure accordingly.
 *
 * @param[in] buf_ctrl      pointer to the buffer control structure to be used
 * @param[in] trial_rate    trial rate configuration
 * @param[in] can_use_bfm   '1' if beamforming can be used with the current policy table;
 *                          '0' otherwise
 * @param[in] use_stbc      '1' if STBC can be used with the current policy table;
 *                          '0' otherwise
 * @param[in] stbc_nss      number of streams to use with STBC
 * @param[in] nc            NC value received in the latest beamforming report
 * @param[in] ntx           number of antennas
 * @param[in] can_use_htc   '1' if +HTC can be used with the current policy table;
 *                          '0' otherwise
 *
 ****************************************************************************************
 */
void rc_trial_check_bfm_stbc_htc(struct txl_buffer_control *buf_ctrl, uint32_t trial_rate,
                                 bool can_use_bfm, bool use_stbc, uint8_t stbc_nss,
                                 uint8_t nc, uint8_t ntx, bool can_use_htc);

/**
 ****************************************************************************************
 * @brief Resets the sample table and fills it with new random samples.
 *
 * @param[in] sta_idx index of the station
 ****************************************************************************************
 */
void rc_update_sample_table(uint8_t sta_idx);

/// @}

#endif /* _RC_ENABLE_H_ */
