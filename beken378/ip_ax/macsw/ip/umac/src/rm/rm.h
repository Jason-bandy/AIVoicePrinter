/**
 ****************************************************************************************
 * @defgroup RM Radio Measurement
 * @ingroup UMAC
 * @brief UMAC's Radio Measurement module
 * @{
 *
 * @file rm.h
 * @brief The UMAC's Radio Measurement module interface
 *
 * Copyright (C) RivieraWaves 2020-2020
 ****************************************************************************************
 */
#ifndef _RM_H_
#define _RM_H_

#include "co_int.h"
#include "mac_types.h"
#include "mac_frame.h"
#include "rm_task.h"
#include "rxu_task.h"
#include "phy.h"

/// Maximum number of measures that can be saved
#define RM_MAX_MEASURES 12

/// Size allocated for each measure to save beacon/probe response payload
#define RM_MEASURE_PAYLOAD_SIZE 464

/// Maximum number of requests that can be processed at once
#define RM_MAX_REQUEST 1

/**
 ****************************************************************************************
 * @brief Initialize RM module
 ****************************************************************************************
 */
void rm_init(void);

/**
 ****************************************************************************************
 * @brief Sends an Measurement Report action frame to reject the request
 *
 * @param[in] vif_idx       Index of the VIF to send the report from
 * @param[in] sta_idx       Index of the STA to send the report to
 * @param[in] dialog_token  Dialog token of action frame
 * @param[in] mode          Reject reason to include in the report
 * @param[in] type          Type of the rejected request
 ****************************************************************************************
 */
void rm_reject_request(uint8_t vif_idx, uint8_t sta_idx, uint8_t dialog_token,
                       uint8_t mode, uint8_t type);

/**
 ****************************************************************************************
 * @brief Initializes RM request from Measurement request elements
 *
 * Initializes internal request structures from list of Measurement Request elements
 * extracted form Radio Measurement action frame.
 * If invalid data is found in one of the request then the action is rejected.
 *
 * @param[in] vif_idx       Index of the vif that received the request
 * @param[in] sta_idx       Index of the STA that send the request
 * @param[in] dialog_token  Dialog token of action frame that contains the requests
 * @param[in] req           Table of Measurement element address (HW address)
 * @param[in] req_cnt       Number of element in @p req
 ****************************************************************************************
 */
void rm_initialize_requests(uint8_t vif_idx, uint8_t sta_idx, uint8_t dialog_token,
                            uint32_t *req, int req_cnt);

/**
 ****************************************************************************************
 * @brief Schedules processing of next RM request.
 *
 * If this function is called when all request have been processed then it simply puts
 * the RM task in @ref RM_IDLE state and exit.
 * Otherwise it sends the message @ref RM_PROCESS_NEXT_REQUEST_IND to the RM task.
 * This allows to asynchronously start process of request from a function.
 ****************************************************************************************
 */
void rm_schedule_next_request(void);

/**
 ****************************************************************************************
 * @brief Starts processing the current active request.
 ****************************************************************************************
 */
void rm_start_active_request(void);

/**
 ****************************************************************************************
 * @brief Continues processing of the current active request.
 ****************************************************************************************
 */
void rm_continue_active_request(void);

/**
 ****************************************************************************************
 * @brief Saves new beacon/probe response info to include in beacon Radio Measurement
 * report
 *
 * @param[in] bcn          Beacon (or Probe resp) frame (CPU address)
 * @param[in] bcn_length   Length, in bytes, of the frame
 * @param[in] tsflo        Lower part of the TSF at frame reception
 * @param[in] freq         PHY primary frequency during reception
 * @param[in] band         PHY Band during reception
 * @param[in] rssi         RSSI of the frame
 * @param[in] antenna_set  Antenna set used to receive the frame
 ****************************************************************************************
 */
void rm_new_beacon_measure(struct bcn_frame const *bcn, uint32_t bcn_length,
                           uint32_t tsflo, uint16_t freq, enum mac_chan_band band,
                           int8_t rssi, uint8_t antenna_set);

/**
 * @}
 */
#endif // _RM_H_
