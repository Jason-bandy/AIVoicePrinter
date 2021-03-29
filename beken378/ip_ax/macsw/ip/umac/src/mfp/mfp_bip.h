/**
 ******************************************************************************
 * @file mfp_bip.h
 *
 * @brief Manangement Frames protection module definitions.
 *
 * Copyright (C) RivieraWaves 2015-2019
 *
 ******************************************************************************
 */

#ifndef _MFP_BIP_H_
#define _MFP_BIP_H_

#include "mac.h"

/**
 * @addtogroup UMACMFP
 * @{
 */

/**
 ******************************************************************************
 * @brief Compute MIC for BIP (Broadcast Integrity Protocol)
 *
 * The function takes a complete frame as parameter, with a MGMT MIC Element
 * whose MIC field has been set to zero.
 * If the MGMT MIC element is already located at the end of the frame then
 * the parameter @p mmic_addr can be set to 0.
 * It does not update MIC field in MGMT MIC Element.
 *
 * @param[in] key         Key for BIP (aka IGTK)
 * @param[in] frame_addr  Address of frame (HW address)
 * @param[in] frame_len   Size in bytes of the frame (excluding MGMT MIC elt)
 * @param[in] machdr_len  Size in bytes on MAC header in the frame
 * @param[in] mmic_addr   Optional address of the MGMT MIC element if not
 *                        included in the frame buffer (HW address)
 *
 * @return MIC value for this frame
 ******************************************************************************
 */
uint64_t mfp_compute_bip(struct key_info_tag *key, uint32_t frame_addr,
                         int frame_len, int machdr_len, uint32_t mmic_addr);


/**
 * @}
 */
#endif
