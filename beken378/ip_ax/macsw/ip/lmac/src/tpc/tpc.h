/**
 ******************************************************************************
 *
 * @file tpc.h
 *
 * @brief TPC (Transmit Power Control) module.
 *
 * Copyright (C) RivieraWaves 2016-2019
 *
 ******************************************************************************
 */

#ifndef _TPC_H_
#define _TPC_H_

#include "co_int.h"
#include "co_bool.h"
#include "vif_mgmt.h"
#include "txl_frame.h"

/**
 ******************************************************************************
 * @defgroup TPC TPC
 * @ingroup TX
 * @brief LMAC Transmit Power Control module.
 * @{
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @brief Set power level to be used for HW generated frame.
 *
 * Configure NXMAC_MAX_POWER_LEVEL register with the specified Tx power by
 * converting it to a radio idx first.
 *
 * @param[in] pwr TX power (in dBm)
 ******************************************************************************
 */
void tpc_update_tx_power(int8_t pwr);

/**
 ******************************************************************************
 * @brief Get TX power for a VIF
 *
 * Return the current TX power configured for the VIF.
 *
 * @param[in]  vif  VIF structure
 * @param[out] pwr  Updated with actual TX power in dBm (can be NULL)
 * @param[out] idx  Updated with the radio idx to use in policy table
 *                  (can be NULL)
 ******************************************************************************
 */
void tpc_get_vif_tx_power(struct vif_info_tag *vif, int8_t *pwr, uint8_t *idx);

/**
 ******************************************************************************
 * @brief Configure TX power for a specific vif
 *
 * Configure a TX power for a vif. The requested value must respect local
 * regulatory. The actual value set may be lower because of radio capabilities.
 *
 * @param[in,out] vif  VIF structure to configure
 * @param[in]     pwr  Tx power to set for the vif
 ******************************************************************************
 */
void tpc_set_vif_tx_power(struct vif_info_tag *vif, int8_t pwr);


#if NX_TX_FRAME
/**
 ******************************************************************************
 * @brief Set power level to be used for FW generated frame.
 *
 * Update tx power in policy table used by the frame.
 *
 * @param[in] vif    Pointer on vif that send the frame
 * @param[in] frame  Pointer on frame to be sent
 ******************************************************************************
 */
void tpc_update_frame_tx_power(struct vif_info_tag *vif,
                               struct txl_frame_desc_tag *frame);
#endif // NX_TX_FRAME

#if NX_UMAC_PRESENT
/**
 ******************************************************************************
 * @brief Configure TX power for a specific vif based on bss information
 *
 * Configure a TX power for a vif based on the bss information and user request.
 * The power configured is the minimum value among:
 * - The maximum allowed by the BSS regulatory
 * - The maximum supported by the phy driver
 * - The value requested by the upper layer
 *
 * @param[in,out] vif  VIF structure to update
 ******************************************************************************
 */
void tpc_update_vif_tx_power(struct vif_info_tag *vif);
#endif // NX_UMAC_PRESENT

/**
 * @}
 */

#endif /* _TPC_H_ */
