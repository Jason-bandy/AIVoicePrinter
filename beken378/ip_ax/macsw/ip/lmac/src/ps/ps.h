/**
 ****************************************************************************************
 *
 * @file ps.h
 *
 * @brief Power-Save definitions.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _PS_H_
#define _PS_H_

/**
 ****************************************************************************************
 * @defgroup PS PS
 * @ingroup LMAC
 * @brief Power-Save mode implementation.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

// for software configuration
#include "rwnx_config.h"
// for linked list definitions
#include "co_list.h"
// for mac_addr and other structure definitions
#include "mac.h"
// for VIF definitions
#include "vif_mgmt.h"
#include "reg_mac_pl.h"
#include "co_math.h"
#if (NX_P2P)
#include "p2p.h"
#endif //(NX_P2P)
#if (RW_UMESH_EN)
#include "mesh_ps.h"
#endif //(RW_UMESH_EN)
#if (NX_TWT)
#include "twt.h"
#endif //(NX_TWT)

#if NX_POWERSAVE

/*
 * DEFINES
 ****************************************************************************************
 */

// Definition of bits preventing from going to sleep (per VIF)
/// Station is waiting for beacon reception
#define PS_VIF_WAITING_BCN         CO_BIT(0)
/// Station is waiting for broadcast/multicast traffic from AP
#define PS_VIF_WAITING_BCMC        CO_BIT(1)
/// Station is waiting for unicast traffic from AP
#define PS_VIF_WAITING_UC          CO_BIT(2)
/// Station is waiting for WMM-PS end of service period
#define PS_VIF_WAITING_EOSP        CO_BIT(3)
/// Station is waiting for the end of the association procedure
#define PS_VIF_ASSOCIATING         CO_BIT(4)
/// P2P GO is supposed to be present
#define PS_VIF_P2P_GO_PRESENT      CO_BIT(5)
/// P2P GO is waiting for TBTT interrupt
#define PS_VIF_P2P_WAIT_TBTT       CO_BIT(6)
/// VIF is configured for monitoring
#define PS_VIF_MONITOR             CO_BIT(7)
/// Waiting for TWT SP to end
#define PS_VIF_TWT_SP_ACTIVE       CO_BIT(8)

// Definition of bits preventing from going to sleep (global)
/// Upload of TX confirmations is ongoing
#define PS_TX_CFM_UPLOADING        CO_BIT(0)
/// A scanning process is ongoing
#define PS_SCAN_ONGOING            CO_BIT(1)
/// A request for going to IDLE is pending
#define PS_IDLE_REQ_PENDING        CO_BIT(2)
/// PSM is paused in order to allow data traffic
#define PS_PSM_PAUSED              CO_BIT(3)
/// A CAC period is active
#define PS_CAC_STARTED             CO_BIT(4)
#if BK_MAC
/// Waiting for MAC RESET
#define PC_WAITING_MAC_RESET       CO_BIT(5)
//  Station is waiting for data
#define PS_WAITING_ADD_KEY         CO_BIT(6)
//  RF registers are being operated
#define PS_WAITING_RF_OPERATION    CO_BIT(7)
//  Station is waiting for temp detect
#define PS_WAITING_TEMP            CO_BIT(8)
#endif /* BK_MAC */
/// Mask showing that all ACs UAPSD enabled
#define PS_ALL_UAPSD_ACS           0x0F

/// This value is sent to host, as the number of packet of a service period, to inicate
/// that current service period has been interrrupted.
#define PS_SP_INTERRUPTED          0xff

/*
 * MACROS
 ****************************************************************************************
 */

#if (NX_DPSM)
/// Indicate if specified bit is set to 1 in dpsm_state
#define PS_DPSM_STATE_GET(bit_pos) \
        (ps_env.dpsm_state & (1 << PS_DPSM_STATE_ ## bit_pos))
/// Set specified bit to 1 in dpsm_state
#define PS_DPSM_STATE_SET(bit_pos) \
        (ps_env.dpsm_state |= (1 << PS_DPSM_STATE_ ## bit_pos))
/// Set specified bit to 0 in dpsm_state
#define PS_DPSM_STATE_CLEAR(bit_pos) \
        (ps_env.dpsm_state &= ~(1 << PS_DPSM_STATE_ ## bit_pos))
#endif //(NX_DPSM)

/**
 ****************************************************************************************
 * Check the specified PS prevent sleep bit
 * @param[in] bit_pos  Position of PS prevent sleep bit to check
 ****************************************************************************************
 */
#define PS_PREVENT_SLEEP(bit_pos) (ps_env.prevent_sleep & PS_##bit_pos)

/**
 ****************************************************************************************
 * Set the specified PS prevent sleep bit
 * @param[in] bit_pos  Position of PS prevent sleep bit to enable
 ****************************************************************************************
 */
#define PS_PREVENT_SLEEP_SET(bit_pos) (ps_env.prevent_sleep |= PS_##bit_pos)
/**
 ****************************************************************************************
 * Clear the specified PS prevent sleep bit
 * @param[in] bit_pos  Position of PS prevent sleep bit to disable
 ****************************************************************************************
 */
#define PS_PREVENT_SLEEP_CLR(bit_pos) (ps_env.prevent_sleep &= ~PS_##bit_pos)
#else
#define PS_PREVENT_SLEEP(bit_pos)
#define PS_PREVENT_SLEEP_SET(bit_pos)
#define PS_PREVENT_SLEEP_CLR(bit_pos)
#endif //(NX_POWERSAVE)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Power Save mode setting
enum
{
    /// Power-save off
    PS_MODE_OFF,
    /// Power-save on - Normal mode
    PS_MODE_ON,
    /// Power-save on - Dynamic mode
    PS_MODE_ON_DYN,
};

#if (NX_POWERSAVE)

#if NX_UAPSD
/// Possible actions for UAPSD Timer
enum
{
    /// Start UAPSD Timer
    PS_UAPSD_TMR_START,
    /// Stop UAPSD Timer
    PS_UAPSD_TMR_STOP,
};
#endif

#if (NX_DPSM)
/// Bit position description for dpsm_state variable
enum ps_dpsm_state_bit_pos
{
    /// Indicate if DPSM is ON (Required by application)
    PS_DPSM_STATE_ON                = 0,
    /// Indicate if it has been required to pause PS Mode
    PS_DPSM_STATE_PAUSING,
    /// Indicate if it has been required to resume PS Mode
    PS_DPSM_STATE_RESUMING,
    /// Indicate if the PS Mode is currently paused
    PS_DPSM_STATE_PAUSE,
    /*
     * Indicate if MM_SET_PS_MODE_REQ has been received from UMAC while pausing/resuming
     * Mode required by UMAC will be stored in ps_env.dpsm_new_mode
     */
    PS_DPSM_STATE_SET_MODE_REQ,
};
#endif //(NX_DPSM)

#if BK_MAC
enum
{
    PS_NULL_SUC,
    PS_NULL_DIS_FAIL,
    PS_NULL_EN_FAIL,
    PS_NULL_BUSY_FAIL,
};
#endif

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// PS module environment
struct ps_env_tag
{
    /// Current Power Save mode state
    bool ps_on;
    /// TaskID of the task that requested the power save change
    ke_task_id_t taskid;
    /// Bitfield indicating which ongoing procedure prevent from going to sleep
    uint32_t prevent_sleep;
    /// Expected number of confirmations for NULL frame transmission
    uint8_t cfm_cnt;

    #if NX_UAPSD
    /// Timer used for Traffic Detection Interval
    struct mm_timer_tag uapsd_timer;
    /// Flag indicating if UAPSD timer is currently active
    bool uapsd_tmr_on;
    /// Flag indicating if UAPSD is currently in use
    bool uapsd_on;
    /// UAPSD Timer timeout value, in microseconds
    uint32_t uapsd_timeout;
    #endif

    #if (NX_DPSM)
    /// Bit field containing different information used for DPSM
    uint8_t dpsm_state;
    /// Next PS Mode required by the upper layers
    uint8_t next_mode;
    #endif //(NX_DPSM)
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern struct ps_env_tag ps_env;

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
#if NX_UAPSD
/**
 ****************************************************************************************
 * @brief Checks UAPSD status
 *
 * @return whether uapsd is enabled or not
 ****************************************************************************************
 */
__INLINE bool ps_uapsd_enabled(void)
{
    return (ps_env.uapsd_timeout != 0);
}
#endif

#if BK_MAC
void ps_run_td_timer(UINT32);
#endif

/**
 ****************************************************************************************
 * @brief Initialize all the entries of the station table.
 ****************************************************************************************
 */
void ps_init(void);


/**
 ****************************************************************************************
 * @brief Set the Power-save mode as requested by the upper layers.
 * This function sends the NULL frame to the AP to indicate them the mode change
 *
 * @param[in] mode       @ref PS_MODE_OFF or @ref PS_MODE_ON
 * @param[in] taskid     ID of the task that requested the power save, and to
 *                       which the confirmation will be sent
 ****************************************************************************************
 */
void ps_set_mode(uint8_t mode, ke_task_id_t taskid);

#if (RW_MESH_EN)
bool ps_check_tim(uint32_t a_tim, uint16_t aid);
#endif //(!RW_MESH_EN)

/**
 ****************************************************************************************
 * @brief Checks the TIM IE in the beacon to know if the station has to wait for
 * individually or group addressed traffic following this beacon.
 * If individually addressed traffic is buffered by AP, this function transmits the
 * PS poll frame.
 *
 * @param[in]  tim        Pointer to the TIM element in the beacon
 * @param[in]  len        Length of the beacon
 * @param[in]  vif        Pointer to the VIF element associated to the beacon
 ****************************************************************************************
 */
void ps_check_beacon(uint32_t tim, uint16_t len, struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Checks the more data bit in the data or management frame passed as parameter
 * and behave accordingly
 *
 * @param[in]  frame      Pointer to the received data or management packet
 * @param[in]  statinfo   MAC HW status of the reception
 * @param[in]  vif        Pointer to the VIF element associated to the frame
 ****************************************************************************************
 */
void ps_check_frame(uint8_t *frame, uint32_t statinfo, struct vif_info_tag *vif);

#if (NX_UAPSD)
/**
 ****************************************************************************************
 * @brief Enable/disable the UAPSD for the HW queue and VIF passed as parameters
 *
 * @param[in]  vif        Pointer to the VIF for UAPSD is enabled/disabled
 * @param[in]  hw_queue   HW queue index
 * @param[in]  uapsd      Flag indicating if U-APSD has to enabled or disabled
 ****************************************************************************************
 */
void ps_uapsd_set(struct vif_info_tag *vif, uint8_t hw_queue, bool uapsd);
#endif //(NX_UAPSD)

#if (NX_UAPSD || NX_DPSM)
/**
 ****************************************************************************************
 * @brief Check if the frame is a UAPSD trigger frame. In such case, if all the
 * queues are UAPSD enabled, do not increase PS traffic statistics in order not to enable
 * DPSM for the UAPSD traffic.
 *
 * @param[in]  txdesc     TX Descriptor containing information about the frame to transmit
 ****************************************************************************************
 */
void ps_check_tx_frame(struct txdesc *txdesc);
#endif //(NX_UAPSD || NX_DPSM)

#if NX_UAPSD
/**
 ****************************************************************************************
 * @brief Re-enable WAIT EOSP status after UAPSD trigger frame transmission
 *
 * When an UAPSD trigger frame has been transmitted (i.e. PS_UAPSD_TRIGGER_SET has been
 * set by @ref ps_check_tx_frame), this function will set the PS_VIF_WAITING_EOSP status
 * if the transmission is successful.
 *
 * @param[in] txdesc    TX Descriptor containing information about the frame to transmit
 * @param[in] tx_status Transmission status of the frame
 ****************************************************************************************
 */
void ps_check_tx_trigger_sent(struct txdesc *txdesc, uint32_t tx_status);
#endif //NX_UAPSD

#if (NX_DPSM)
/**
 ****************************************************************************************
 * @brief Handle update of Traffic status generated by the Traffic Detection module.
 *        It is used in order to pause/restart the legacy power save mode if DPSM feature
 *        has been enabled.
 *
 * @param[in] vif_index     Index of the VIF entry for which the status has been updated
 * @param[in] new_status    Updated status
 ****************************************************************************************
 */
void ps_traffic_status_update(uint8_t vif_index, uint8_t new_status);
#endif //(NX_DPSM)

#if (NX_P2P && NX_UAPSD)
/**
 ****************************************************************************************
 * @brief Update P2P GO presence status update in order to send a trigger frame when
 *        an absence period is over after an interrupted Service Period.
 *
 * @param[in] vif           VIF Entry
 * @param[in] absent        P2P GO presence status
 ****************************************************************************************
 */
void ps_p2p_absence_update(struct vif_info_tag *vif, bool absent);
#endif //(NX_P2P && NX_UAPSD)

#endif // NX_POWERSAVE


#if BK_MAC
void ps_set_rf_prevent(void);
void ps_clear_rf_prevent(void);
void ps_set_mac_reset_prevent(void);
#endif

/// @}

#endif // _PS_H_
