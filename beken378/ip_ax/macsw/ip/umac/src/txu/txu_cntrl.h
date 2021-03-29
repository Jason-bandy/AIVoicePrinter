/**
 ****************************************************************************************
 *
 * @file txu_cntrl.h
 *
 * @brief UMAC TX Path definitions.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _TXU_CNTRL_H_
#define _TXU_CNTRL_H_

#include "co_int.h"
#include "co_bool.h"
#include "rwnx_config.h"

/** @defgroup UMACTX TXU
 * @ingroup UMAC
 * @brief UMAC TX Path.
 * @{
 */

struct txdesc;

/*
 * DEFINES
 ****************************************************************************************
 */

/// @name TX Status bits passed to the host upon frame confirmation

/// Frame transmission done
#define TX_STATUS_DONE                 CO_BIT(0)
/// Frame retry required
#define TX_STATUS_RETRY_REQUIRED       CO_BIT(1)
/// Frame sw retry required
#define TX_STATUS_SW_RETRY_REQUIRED    CO_BIT(2)
/// Frame acknoledged
#define TX_STATUS_ACKNOWLEDGED         CO_BIT(3)

/// @name TX Flags passed by the host or UMAC

/// The frame is a retry
#define TXU_CNTRL_RETRY         CO_BIT(0)
/// The frame is sent under a BlockAck agreement
#define TXU_CNTRL_UNDER_BA      CO_BIT(1)
/// More data are buffered on host side for this STA after this packet
#define TXU_CNTRL_MORE_DATA     CO_BIT(2)
/**
 * TX Frame is a management frame:
 *      - WLAN header is already provided, no need to transform an ethernet header
 *      - Frame shall be sent as a singleton
 */
#define TXU_CNTRL_MGMT          CO_BIT(3)
/// No CCK rate can be used (valid only if TXU_CNTRL_MGMT is set)
#define TXU_CNTRL_MGMT_NO_CCK   CO_BIT(4)
/// Internal flag indicating that the PM monitoring has been started for this frame
#define TXU_CNTRL_MGMT_PM_MON   CO_BIT(5)
/// The frame is an A-MSDU
#define TXU_CNTRL_AMSDU         CO_BIT(6)
/// The frame is a robust management frame
#define TXU_CNTRL_MGMT_ROBUST   CO_BIT(7)
/// The frame shall be transmitted using a 4 address MAC Header
#define TXU_CNTRL_USE_4ADDR     CO_BIT(8)
/// The frame is the last of the UAPSD service period
#define TXU_CNTRL_EOSP          CO_BIT(9)
/// This frame is forwarded
#define TXU_CNTRL_MESH_FWD      CO_BIT(10)
/// This frame is sent directly to a TDLS peer
#define TXU_CNTRL_TDLS          CO_BIT(11)
/// This frame is postponed internally because of PS. (only for AP)
#define TXU_CNTRL_POSTPONE_PS   CO_BIT(12)
/// Internal flag indicating that this packet should use the trial rate as first or second rate
#define TXU_CNTRL_RC_TRIAL      CO_BIT(13)
/// Internal flag indicating that this is a UAPSD trigger frame
#define TXU_CNTRL_UAPSD_TRIGGER CO_BIT(14)
/// @}

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief This is the entry point in UMAC TX path for sending host and send back
 * data frames.
 *
 * @param[in]  txdesc           Pointer to the structure that has the info of the data
 *                              packet to be transmitted.
 * @param[in]  access_category  Index of the queue in which the packet is passed
 *
 * @return Whether the IPC queue shall be stopped after this call or not
 ****************************************************************************************
 */
bool txu_cntrl_push(struct txdesc *txdesc, uint8_t access_category);

/**
 ****************************************************************************************
 * @brief Function called by the LMAC once the data buffer has been allocated. It is
 * responsible for the different header formatting (MAC Header, IV/EIV, LLC/SNAP).
 *
 * @param[in]  txdesc           Pointer to the structure that has the info of the data
 *                              packet to be transmitted.
 * @param[in]  buf              Address of the header in shared RAM
 ****************************************************************************************
 */
void txu_cntrl_frame_build(struct txdesc *txdesc, uint32_t buf);

/**
 ****************************************************************************************
 * @brief Function called by the LMAC once the data buffer has been downloaded. It is
 * responsible for the TKIP MIC computing if required.
 *
 * @param[in]  txdesc           Pointer to the structure that has the info of the data
 *                              packet to be transmitted.
 * @param[in]  ac               Access category on which the packet is sent
 ****************************************************************************************
 */
void txu_cntrl_tkip_mic_append(struct txdesc *txdesc, uint8_t ac);

/**
****************************************************************************************
* @brief This function handles status of a transmission. It returns the number of credits
* allocated to the host for the next transmissions.
*
* @param[in] txdesc  Pointer to the packet descriptor
*
****************************************************************************************
*/
void txu_cntrl_cfm(struct txdesc *txdesc);

/**
****************************************************************************************
* @brief This function Add security header to a management frame. If security
* lengths (head and tail) are not set in txdesc->umac, it will first compute them
* and update txdesc. Then it adds security header. No check on frame type/subtype
* is done and it assumes that it has already been checked before.
*
* @param[in,out] txdesc    Pointer to the packet descriptor
* @param[in]     mac_hdr   Pointer on MAC header of the frame
* @param[in]     hdr_len   Size of MAC header
****************************************************************************************
*/
void txu_cntrl_protect_mgmt_frame(struct txdesc *txdesc, struct mac_hdr *mac_hdr,
                                  uint16_t hdr_len);

#if NX_FULLY_HOSTED
/**
****************************************************************************************
* @brief Links PDB for MGMT MIC in MGMT frame pushed by upper layer.
*
* In FHOST, for MGMT frames pushed by upper layer there is no space for security header
* /tail (as we reuse the buffer allocate in fhost). Since in this case the frame is always
* made of one single PBD, this function will append a new PDB to store the MGMT MIC
* element (needed for broadcast MGMT protected frame).
* This PBD will point to the 'headers' buffer in the txl_buffer_tag which is not used
* for this kind of frame (as they are pushed with a 802.11 MAC header already formatted).
*
* @param[in,out] txdesc    Pointer to the packet descriptor
*
* @return HW address of the buffer reserved for the MGMT MIC element
****************************************************************************************
*/
uint32_t txu_cntrl_mgmt_mic_pbd_append(struct txdesc *txdesc);
#endif
/// @}

#endif // _TXU_CNTRL_H_
