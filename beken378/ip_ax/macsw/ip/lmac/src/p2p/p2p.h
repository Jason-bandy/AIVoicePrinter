/**
 ****************************************************************************************
 * @file p2p.h
 *
 * @brief Wi-Fi Peer-to-Peer (P2P) Management
 *
 * Copyright (C) RivieraWaves 2011-2019
 *
 ****************************************************************************************
 */

#ifndef _P2P_H_
#define _P2P_H_

/**
 *****************************************************************************************
 * @defgroup P2P P2P
 * @ingroup LMAC
 * @brief P2P module.
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwnx_config.h"

#if (NX_P2P)

#include "mm_timer.h"
#include "hal_desc.h"
#include "compiler.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/**
 * @name P2P IE format
 * See wifi_p2p_technical_specification_v1.1.pdf:
 * - 4.1.1 - P2P IE Format Table 4
 */
#define P2P_IE_ID_OFT       (0)
#define P2P_IE_LEN_OFT      (1)
#define P2P_IE_OUI_OFT      (2)
#define P2P_IE_OUI_TYPE_OFT (5)
#define P2P_IE_ATT_OFT      (6)

/// P2P Element ID (Vendor Specific)
#define P2P_ELMT_ID                 (MAC_ELTID_OUI)
/// OUI Value - Wi-FiAll
#define P2P_OUI_WIFI_ALL_BYTE0      (0x50)
#define P2P_OUI_WIFI_ALL_BYTE1      (0x6F)
#define P2P_OUI_WIFI_ALL_BYTE2      (0x9A)
/// Vendor Specific OUI Type for P2P
#define P2P_OUI_TYPE_P2P            (9)

/**
 * @name P2P Attribute format
 * See wifi_p2p_technical_specification_v1.1.pdf.
 * - 4.1.1 - General format of P2P attribute Table 5
 */
#define P2P_ATT_ID_OFT              (0)
#define P2P_ATT_LEN_OFT             (1)
#define P2P_ATT_BODY_OFT            (3)

/**
 * @name Notice of Absence attribute format
 * See wifi_p2p_technical_specification_v1.1.pdf.
 * - 4.1.14 - Notice of Absence attribute Tables 26-27
 */
#define P2P_NOA_ATT_INDEX_OFT        P2P_ATT_BODY_OFT
#define P2P_NOA_ATT_CTW_OPPPS_OFT    (4)
#define P2P_NOA_ATT_NOA_DESC_OFT     (5)

/**
 * @name Notice of Absence descriptor format
 * See wifi_p2p_technical_specification_v1.1.pdf.
 * - 4.1.14 - Notice of Absence attribute Tables 28
 */
#define P2P_NOA_DESC_COUNT_OFT    (0)
#define P2P_NOA_DESC_DUR_OFT      (1)
#define P2P_NOA_DESC_INTV_OFT     (5)
#define P2P_NOA_DESC_START_OFT    (9)
#define P2P_NOA_DESC_LENGTH       (13)

#define P2P_IE_NOA_NO_NOA_DESC_LENGTH   (P2P_IE_ATT_OFT + P2P_NOA_ATT_NOA_DESC_OFT)
#define P2P_NOA_IE_BUFFER_LEN  ((P2P_IE_NOA_NO_NOA_DESC_LENGTH + (P2P_NOA_NB_MAX * P2P_NOA_DESC_LENGTH) + 1) / 2)
/** @} */

/**
 * If the counter field of NOA attribute is equal to 255, the absence cycles shall repeat
 * until cancel.
 */
#define P2P_NOA_CONTINUOUS_COUNTER  (255)
/// Invalid P2P Information Structure Index
#define P2P_INVALID_IDX             (0xFF)
/// Maximal number of concurrent NoA
#define P2P_NOA_NB_MAX              (2)

/// Margin used to avoid to program NOA timer in the past (in us)
#define P2P_NOA_TIMER_MARGIN        (5000)
/// Minimal absence duration we can consider (in us)
#define P2P_ABSENCE_DUR_MIN         (5000)
/// Beacon RX Timeout Duration (in us)
#define P2P_BCN_RX_TO_DUR           (5000)

/// Flag allowing to read OppPS subfield of CTWindow + OppPS field
#define P2P_OPPPS_MASK              (0x80)
/// Flag allowing to read CTWindow subfield of CTWindow + OppPS field
#define P2P_CTWINDOW_MASK           (0x7F)

#if (NX_UMAC_PRESENT)
/// SSID Wildcard for a P2P Device
#define P2P_SSID_WILDCARD           ("DIRECT-")
/// Length of P2P SSID Wildcard
#define P2P_SSID_WILDCARD_LEN       (7)
#endif //(NX_UMAC_PRESENT)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// NOA Timer (see struct p2p_info_tag) status
enum p2p_noa_timer_status
{
    /// Timer not started
    P2P_NOA_TIMER_NOT_STARTED = 0,
    /// Wait for next absence
    P2P_NOA_TIMER_WAIT_NEXT_ABS,
    /// Wait for end of absence
    P2P_NOA_TIMER_WAIT_END_ABS,
};

/// OppPS Timer (see struct p2p_info_tag) status
enum p2p_oppps_timer_status
{
    /// Timer not started
    P2P_OPPPS_TIMER_NOT_STARTED = 0,
    /// Wait for end of CTWindow
    P2P_OPPPS_TIMER_WAIT_END_CTW,
    /// Wait start of CTWindow
    P2P_OPPPS_TIMER_WAIT_START_CTW
};

/**
 * P2P Attribute ID definitions
 * See wifi_p2p_technical_specification_v1.1.pdf
 *     4.1.1 - P2P IE Format Table 6
 */
enum p2p_attribute_id
{
    P2P_ATT_ID_STATUS         = 0,
    P2P_ATT_ID_MINOR_REASON_CODE,
    P2P_ATT_ID_P2P_CAPABILITY,
    P2P_ATT_ID_P2P_DEVICE_ID,
    P2P_ATT_ID_GROUP_OWNER_INTENT,
    P2P_ATT_ID_CONFIG_TIMEOUT,
    P2P_ATT_ID_LISTEN_CHANNEL,
    P2P_ATT_ID_P2P_GROUP_BSSID,
    P2P_ATT_ID_EXT_LISTEN_TIMING,
    P2P_ATT_ID_INTENDED_P2P_IF_ADDR,
    P2P_ATT_ID_P2P_MANAGEABILITY,
    P2P_ATT_ID_CHANNEL_LIST,
    P2P_ATT_ID_NOTICE_OF_ABSENCE,
    P2P_ATT_ID_P2P_DEVICE_INFO,
    P2P_ATT_ID_P2P_GROUP_INFO,
    P2P_ATT_ID_P2P_GROUP_ID,
    P2P_ATT_ID_P2P_INTERFACE,
    P2P_ATT_ID_OPERATING_CHANNEL,
    P2P_ATT_ID_INVITATION_FLAGS,
    /// 19 - 220 -> Reserved
    P2P_ATT_ID_VENDOR_SPEC   = 221,
    /// 222 - 255 -> Reserved
    P2P_ATT_ID_MAX           = 255
};

/// P2P Role
enum p2p_role
{
    /// Client
    P2P_ROLE_CLIENT = 0,
    /// GO
    P2P_ROLE_GO,
};

/// Operation codes to be used in p2p_bcn_update_req structure
enum p2p_bcn_upd_op
{
    /// No Operation
    P2P_BCN_UPD_OP_NONE     = 0,

    /// Add P2P NOA IE
    P2P_BCN_UPD_OP_NOA_ADD,
    /// Remove P2P NOA IE
    P2P_BCN_UPD_OP_NOA_RMV,
    /// Update P2P NOA IE
    P2P_BCN_UPD_OP_NOA_UPD,
};

#if (NX_P2P_GO)
/// Type of NoA
enum p2p_noa_type
{
    /// Concurrent NoA - Triggered by connection as STA on a different channel
    P2P_NOA_TYPE_CONCURRENT,
    /// Normal NoA
    P2P_NOA_TYPE_NORMAL,
};
#endif //(NX_P2P_GO)

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

// Forward declaration
struct vif_info_tag;

/**
 * Structure containing all information linked with NoA
 */
struct p2p_noa_info_tag
{
    /// Timer used for P2P Notice of Absence management
    struct mm_timer_tag noa_timer;
    /// Status of the NOA timer
    uint8_t noa_status;

    /// Index of the linked P2P information structure
    uint8_t p2p_index;
    /// NOA Instance
    uint8_t noa_inst;
    /**
     * NOA Counter
     * Indicate the remaining number of absence (current one included) - 255 for continuous
     */
    uint8_t noa_counter;
    /// Initial NOA Counter value
    uint8_t noa_init_counter;
    /// NOA - Absence Duration (in us)
    uint32_t noa_dur_us;
    /// NOA - Absence Interval (in us)
    uint32_t noa_intv_us;

    #if (NX_P2P_GO)
    /// Type of NoA (due to Concurrent Mode or required by Host for GO) - relevant for GO only
    uint8_t noa_type;
    /// Indicate if NoA can be paused for traffic reasons
    bool dyn_noa;
    #endif //(NX_P2P_GO)

    /// NOA - Start Time - GO only
    uint32_t noa_start_time;
    /// Counter used to know when we can update the start time inside the beacon - GO only
    uint32_t noa_time_upd_cnt;

    /// Next NOA time (absence or end of absence) in peer TSF (valid only for CLI connection)
    uint32_t peer_next_noa_time;
};

/**
 * Information structure containing all information about a P2P link
 */
struct p2p_info_tag
{
    /// NOA - Up to 2 concurrent NoA
    struct p2p_noa_info_tag noa[P2P_NOA_NB_MAX];
    /// Timer used for P2P Opportunistic PS management
    struct mm_timer_tag oppps_timer;

    /// Role
    uint8_t role;
    /// Associated VIF index
    uint8_t vif_index;
    /// Status of OppPS timer
    uint8_t oppps_status;
    /// CTWindow, Opportunistic Power Save is enabled if CTW is different than 0
    uint8_t oppps_ctw;
    /// End of next CTW (only used for p2p-cli when delaying start of CTW)
    uint32_t oppps_ctw_end;
    /// Last received/sent NOA Index
    uint8_t noa_index;
    /// Number of NOAs currently used
    uint8_t noa_nb;
    /// Number of NOA triggered by Concurrent Mode
    uint8_t noa_cm_nb;
    /// timestamp when GO is next absent
    uint32_t next_absent;
    /// timestamp when GO is next present
    uint32_t next_present;

    /// Indicate if the NOA attribute is part of the beacon
    bool is_noa_bcn;
    /// Indicate if P2P Go is present
    bool is_go_present;
    /// Indicate if we are waiting for Beacon reception
    bool is_waiting_bcn;

    #if (NX_P2P_GO)
    /**
     * Indicate if P2P PS NoA is paused due to traffic
     *  - OppPS does not need to be paused: if we have traffic on a link, peer device won't enter in doze mode
     *  - NoA due to Concurrent mode cannot be paused
     */
    bool noa_paused;
    /**
     * Indicate the number of NoA indicated in the beacon.
     * This value can be different than noa_nb if noa_paused is true
     */
    uint8_t noa_in_bcn_nb;
    #endif //(NX_P2P_GO)
};

#if (NX_P2P_GO)
/// P2P Global Environment
struct p2p_env_tag
{
    /// Number of P2P GO VIF that have been created (up to 1)
    uint8_t nb_p2p_go;
};
#endif //(NX_P2P_GO)

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

#if (NX_P2P_GO)
/// Global p2p status
extern struct p2p_env_tag p2p_env;
#endif //(NX_P2P_GO)

/// Global table with all P2P entries
extern struct p2p_info_tag p2p_info_tab[NX_P2P_VIF_MAX];

/*
 * PUBLIC FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the p2p module. Set the vif_index value of each p2p structures to
 * INVALID_VIF_IDX in order to inform that the structure is available.
 ****************************************************************************************
 */
void p2p_init(void);


/**
 ****************************************************************************************
 * @brief Link a VIF entry with a P2P entry.
 *
 * @param[in] vif_index     Index of the P2P VIF
 * @param[in] role          P2P Interface Role
 *
 * @return P2P entry index if a P2P structure was available, else P2P_INVALID_IDX.
 ****************************************************************************************
 */
uint8_t p2p_create(uint8_t vif_index, uint8_t role);

/**
 ****************************************************************************************
 * @brief Release a P2P structure.
 *
 * @param[in] p2p_index     P2P entry index
 * @param[in] vif_del       Indicate if the VIF will be unregistered.
 ****************************************************************************************
 */
void p2p_cancel(uint8_t p2p_index, bool vif_del);

/**
 ****************************************************************************************
 * @brief Update the P2P entry parameters when VIF state is updated
 *
 * @param[in] vif           VIF Entry
 * @param[in] active        Boolean value indicating if VIF is active or not
 ****************************************************************************************
 */
void p2p_set_vif_state(struct vif_info_tag *vif, bool active);

/**
 ****************************************************************************************
 * @brief Check GO device presence
 *
 * @param[in]  p2p_index  Index of the P2P entry
 * @return true if the GO device is present, false otherwise
 ****************************************************************************************
 */
bool p2p_is_present(uint8_t p2p_index);

/**
 ****************************************************************************************
 * @brief Get next presence/absence date for a p2p GO interface
 *
 * This function returns next absence/presence date for a GO interface based on its
 * scheduled NOA.
 * The date returned takes into account potential overlapping NOA, and they are always in
 * the future (meaning that if this is currently an absence period then next_presence
 * date will happen before next_absence date).
 *
 * @note The return value indicates if this is currently a presence or absence period
 * from NOA point of view. It is not always the same status as returned by @ref
 * p2p_is_present (e.g. if TBTT happens during an absence period of a periodic NOA, then
 * @ref p2p_is_present will return true, but this function will return false).
 *
 * @param[in]  vif               Vif entry for the P2P GO interface.
 * @param[out] next_p2p_present  Date at witch next NOA ends
 * @param[out] next_p2p_absent   Date at witch next NOA starts
 *
 * @return true if this is currently a presence period and false otherwise.
 ****************************************************************************************
 */
bool p2p_go_get_next_NOA_date(struct vif_info_tag *vif,
                              uint32_t *next_p2p_present,
                              uint32_t *next_p2p_absent);

/**
 ****************************************************************************************
 * @brief Get end of the next NOA iteration.
 *
 * @param[in] vif       Vif entry for the P2P interface
 * @param[in] noa_inst  Index of the NOA
 *
 * @return timestamp, in us, of the end of the next absence period of the required NOA.
 ****************************************************************************************
 */
uint32_t p2p_go_get_next_noa_end_date(struct vif_info_tag *vif,
                                      uint8_t noa_inst);

/**
 ****************************************************************************************
 * @brief Look for P2P NOA Attribute inside a received beacon. Start/Update/Stop the NOA
 *        procedure accordingly with received information.
 *
 * @param[in]  vif            VIF Entry
 * @param[in]  pyld_desc      Beacon Payload Descriptor
 * @param[in]  dma_hdrdesc    Beacon Header Descriptor
 *
 * @return address on the NOA Attribute IE. (0 if not present)
 ****************************************************************************************
 */
uint32_t p2p_cli_bcn_check_noa(struct vif_info_tag *vif,
                               struct rx_pbd *pyld_desc, struct rx_dmadesc *dma_hdrdesc);

/**
 ****************************************************************************************
 * @brief Look for P2P NOA Attribute inside a received action frame. Start/Update/Stop the NOA
 *        procedure accordingly with received information.
 *
 * @param[in]  vif          VIF Entry
 * @param[in]  a_frame      Address of the frame (HW address)
 * @param[in]  length       Length of the frame (bytes)
 * @param[in]  rx_tsf       TSF when frame was received
 *
 ****************************************************************************************
 */
void p2p_cli_handle_action(struct vif_info_tag *vif, uint32_t a_frame,
                           uint16_t length, uint32_t rx_tsf);

/**
 ****************************************************************************************
 * @brief Handle P2P operation to be performed on STA_TBBT or AP_TBBT event.
 *
 * @param[in]  vif        VIF Entry
 * @param[in]  tbtt_time  Timestamp of TBTT. It doesn't include any margin.
  ****************************************************************************************
 */
void p2p_tbtt_handle(struct vif_info_tag *vif, uint32_t tbtt_time);

/**
 ****************************************************************************************
 * @brief Handle P2P operation to be performed on Beacon reception.
 *
 * @param[in]  vif            VIF Entry
 ****************************************************************************************
 */
void p2p_bcn_evt_handle(struct vif_info_tag *vif);

#if (NX_P2P_GO && NX_POWERSAVE)
/**
 ****************************************************************************************
 * @brief Check P2P PS mode
 *
 * @return True if only one VIF is active and this VIF is a P2P GO VIF with PS enabled
 * and currently in a absence period.
 ****************************************************************************************
 */
bool p2p_go_check_ps_mode(void);
#endif //(NX_P2P_GO && NX_POWERSAVE)

#if (NX_P2P_GO)
/**
 ****************************************************************************************
 * @brief Handle update of Traffic status generated by the Traffic Detection module.
 *        When VIF is used for a P2P GO, it is used to stop/start NOA in order to increase
 *        the throughput if needed.
 *
 * @param[in] vif_index   Index of the VIF entry for which the status has been updated
 * @param[in] new_status  New traffic status for the vif (@ref td_status_bit)
 ****************************************************************************************
 */
void p2p_go_td_evt(uint8_t vif_index, uint8_t new_status);

/**
 ****************************************************************************************
 * @brief Start Opportunistic Power Save mode on P2P GO side
 *
 * @param[in] vif            VIF Entry
 * @param[in] ctw            CTWindow
 ****************************************************************************************
 */
void p2p_go_oppps_start(struct vif_info_tag *vif, uint8_t ctw);

/**
 ****************************************************************************************
 * @brief Stop Opportunistic Power Save mode on P2P GO side
 *
 * @param[in] vif            VIF Entry
 ****************************************************************************************
 */
void p2p_go_oppps_stop(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Check if GO PS state has to be updated once it is known that a peer CLI device
 *        is entered in Doze mode (used if OppPS has been enabled)
 *
 * @param[in] vif           VIF Entry
 ****************************************************************************************
 */
void p2p_go_ps_state_update(struct vif_info_tag *vif);

/**
 ****************************************************************************************
 * @brief Start NOA procedure on P2P GO side.
 *
 * @param[in] vif            VIF Entry
 * @param[in] concurrent     Whether it is a concurent (@ref P2P_NOA_TYPE_CONCURRENT) NOA
 *                           or not (@ref P2P_NOA_TYPE_NORMAL)
 * @param[in] dyn_noa        Indicate if NoA can be paused for traffic reason
 * @param[in] counter        Number of absense intervals
 * @param[in] intv_us        Length of the Notice of Absence interval (in us)
 * @param[in] dur_us         Duration of the absence (in us)
 * @param[in] start_time     Start time (TSF timer) of the NOA
 *
 * @return Index of the NOA and @ref P2P_INVALID_IDX if no NOA has been created.
 ****************************************************************************************
 */
uint8_t p2p_go_noa_start(struct vif_info_tag *vif, bool concurrent, bool dyn_noa,
                         uint8_t counter, uint32_t intv_us, uint32_t dur_us,
                         uint32_t start_time);

/**
 ****************************************************************************************
 * @brief Stop NOA procedure on P2P GO side.
 *
 * @param[in] vif            VIF Entry
 * @param[in] noa_inst       NOA instance to be stopped
 * @param[in] host_req       Indicate if stop is request by host or not. Host is not
 *                           allowed to stop a NoA started for concurrent mode reasons.
 * @return CO_OK if NAO has been stopped, CO_FAIL otherwise
 ****************************************************************************************
 */
uint8_t p2p_go_noa_stop(struct vif_info_tag *vif, uint8_t noa_inst, bool host_req);

#if (NX_CHNL_CTXT)
/**
 ****************************************************************************************
 * @brief Shift concurrent NOA with a given offset.
 *
 * It will shift the next iteration of all concurrent NOA with a given offset.
 *
 * @param[in] vif     VIF Entry
 * @param[in] offset  Offset to apply, in us.
 ****************************************************************************************
 */
void p2p_go_noa_concurrent_move(struct vif_info_tag *vif, int32_t offset);
#endif //(NX_CHNL_CTXT)

/**
 ****************************************************************************************
 * @brief Return the additional length to consider in the beacon for the NOA attribute.
 * If the NOA is not part of the sent beacon, the function return 0.
 *
 * @param[in]  p2p_index    P2P entry index
 *
 * @return size, in bytes, for the NAO attribute
 ****************************************************************************************
 */
uint8_t p2p_go_bcn_get_noa_len(uint8_t p2p_index);

/**
 ****************************************************************************************
 * @brief Initialize an NOA attribute payload buffer (attribute ID, attribute length, ...)
 *
 * @param[in]  a_noa_ie_elt     Address of a NOA attribute payload buffer
 ****************************************************************************************
 */
void p2p_go_bcn_init_noa_pyld(uint32_t a_noa_ie_elt);

/**
 ****************************************************************************************
 * @brief Trigger an update of the provided NOA attribute payload buffer.
 * This function is called by the mm_bcn module once it is possible to update the data
 * contained in the beacon after mm_bcn_update_p2p_noa is called.
 *
 * @param[in]  p2p_index     P2P entry index
 * @param[in]  a_noa_ie_elt     Address of a NOA attribute payload buffer
 ****************************************************************************************
 */
void p2p_go_bcn_upd_noa_pyld(uint8_t p2p_index, uint32_t a_noa_ie_elt);

/**
 ****************************************************************************************
 * @brief Callback to be used by mm_bcn module once operation required by p2p module
 * has been done (ADD, REMOVE, UPDATE).
 *
 * @param[in]  p2p_index     P2P entry index
 * @param[in]  operation     Required operation
 ****************************************************************************************
 */
void p2p_go_bcn_op_done(uint8_t p2p_index, uint8_t operation);

#if (NX_POWERSAVE)
/**
 ****************************************************************************************
 * @brief When HW is in doze mode, it does not trigger the TBTT interrupt anymore. Hence we use a
 * timer in order to wake up slightly before AP_TBTT occurs. This function is used as a
 * callback for the TBTT timer
 *
 * @param[in] vif            VIF Entry
 ****************************************************************************************
 */
void p2p_go_pre_tbtt(struct vif_info_tag *vif);
#endif //(NX_POWERSAVE)
#endif //(NX_P2P_GO)

#endif //(NX_P2P)

/// @} end of group

#endif // _P2P_H_
