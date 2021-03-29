/**
 ****************************************************************************************
 *
 * @file me_utils.h
 *
 * @brief All utility functions manipulating rates, etc.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _ME_UTILS_H_
#define _ME_UTILS_H_

/** @defgroup MEUTILS UTILS
* @ingroup ME
* @brief All utility functions manipulating rates, etc.
* @{
*/

// standard includes
#include "co_int.h"
#include "co_bool.h"

#include "tx_swdesc.h"

// forward declarations
struct mac_mcsset;
struct mac_rateset;
struct mac_rates;
struct sta_info_tag;
struct txdesc;
struct me_bss_info;

/**
 ****************************************************************************************
 * @brief Convert a PHY channel type to a MAC HW BW definition
 *
 * @param[in]  phy_bw  PHY channel type
 *
 * @return The corresponding MAC HW BW value
 *
 ****************************************************************************************
 */
__INLINE uint8_t me_phy2mac_bw(uint8_t phy_bw)
{
    return((phy_bw == PHY_CHNL_BW_80P80) ? BW_160MHZ : phy_bw);
}

/**
 ****************************************************************************************
 * @brief Return the maximum PHY BW than can be used with a STA depending on the BSS PHY.
 * This function generally takes the minimum of the 2 BW passed as parameter, except
 * if the BSS is a 80P80. In such case the maximum common BW might be 80MHz instead
 * of 160MHz if the STA supports 160MHz but not 80P80
 *
 * @param[in]  sta_phy_bw  Max PHY BW of the STA
 * @param[in]  bss_phy_bw  PHY BW of the BSS
 *
 * @return The maximum PHY common BW
 *
 ****************************************************************************************
 */
__INLINE uint8_t me_get_sta_bw(uint8_t sta_phy_bw, uint8_t bss_phy_bw)
{
    // Handle specific case where BSS is 80+80 whereas we don't support
    if ((bss_phy_bw == PHY_CHNL_BW_80P80) && (sta_phy_bw < PHY_CHNL_BW_80P80))
    {
        bss_phy_bw = PHY_CHNL_BW_80;
    }
    return (co_min(bss_phy_bw, sta_phy_bw));
}

/**
 ****************************************************************************************
 * @brief Compute some HT and VHT parameters for the given STA.
 * This function computes the maximum PHY BW we can use with the peer STA
 *
 * @param[in]  sta Pointer to the STA element
 * @param[in]  bss Pointer to the BSS information element
 *
 * @return Whether the STA supports SMPS or not
 *
 ****************************************************************************************
 */
bool me_set_sta_ht_vht_param(struct sta_info_tag *sta, struct me_bss_info *bss);

/**
 ****************************************************************************************
 * @brief Build the legacy rate bitfield according to the rate set passed as parameter.
 * Include only basic rates or not depending on the corresponding parameter.
 *
 * @param[in]  rateset     The rate set containing the legacy rates
 * @param[in]  basic_only  If this parameter is true, then only basic rates are extracted
 *                         from the rate set
 *
 * @return The legacy rate bitfield
 *
 ****************************************************************************************
 */
uint16_t me_legacy_rate_bitfield_build(struct mac_rateset const *rateset, bool basic_only);

/**
 ****************************************************************************************
 * @brief Build a VHT-MCS map according to the MCS maps passed as parameters
 *
 * @param[in]  mcs_map_1  The first VHT-MCS Map
 * @param[in]  mcs_map_2  The second VHT-MCS Map
 * @return The VHT-MCS map of the station
 ****************************************************************************************
 */
uint16_t me_rate_bitfield_vht_build(uint16_t mcs_map_1, uint16_t mcs_map_2);

/**
 ****************************************************************************************
 * @brief Build the capability information
 *
 * @param[in]  vap_idx     instance of the device
 * @return     capability information
 *
 ****************************************************************************************
 */
uint16_t me_build_capability(uint8_t vap_idx);

/**
 ****************************************************************************************
 * @brief Build the BSS color as per MAC HW register format from the BSS operation element
 *
 * @param[in]  bss  BSS information structure
 *
 * @return The BSS color formatted as per MAC HW register format
 ****************************************************************************************
 */
uint32_t me_build_bss_color_reg(struct me_bss_info *bss);

/**
 ****************************************************************************************
 * @brief Initializes Rate control for a STA
 *
 * @param[in] sta       STA to initialize
 ****************************************************************************************
 */
void me_init_rate(struct sta_info_tag *sta);

/**
 ****************************************************************************************
 * @brief Initializes Rate control for a Brodcast/Multicast  STA
 *
 * @param[in] sta       STA to initialize
 ****************************************************************************************
 */
void me_init_bcmc_rate(struct sta_info_tag *sta);

/**
 ****************************************************************************************
 * @brief Update the field contained in the Policy Table
 *
 * @param[in]  sta      STA Interface Information
 *
 * @return The buffer control structure to be used for the transmission
 *
 ****************************************************************************************
 */
struct txl_buffer_control *me_update_buffer_control(struct sta_info_tag *sta);

/**
 ****************************************************************************************
 * @brief Check content of the HT and VHT operation element passed as parameters and
 * update the bandwidth and channel information of the BSS accordingly.
 *
 * @param[in] ht_op_addr Address of the HT operation element to check (0 if not present)
 * @param[in] vht_op_addr Address of the VHT operation element to check (0 if not present)
 * @param[in] bss Pointer to the BSS information structure
 *
 ****************************************************************************************
 */
void me_bw_check(uint32_t ht_op_addr, uint32_t vht_op_addr, struct me_bss_info *bss);

/**
 ****************************************************************************************
 * @brief Check in the received (in case of STA mode) or just modified (in case of AP mode)
 * beacon if the protection and/or bandwidth of operation have been modified.
 * If this is the case, then all the STA associated to the VIF are updated accordingly.
 *
 * @param[in] vif_idx Index of the VIF for which the beacon is checked
 * @param[in] length Length of the beacon
 * @param[in] bcn_addr Payload address of the beacon
 *
 ****************************************************************************************
 */
void me_beacon_check(uint8_t vif_idx, uint16_t length, uint32_t bcn_addr);

/**
 ****************************************************************************************
 * @brief Update the maximum bandwidth supported by a peer STA. This function is called
 * upon the reception of a HT Notify Channel Width or VHT Operation Mode Notification
 * action frame.
 *
 * @param[in] sta_idx  Index of the STA for which the BW is updated
 * @param[in] bw       Maximum bandwidth to use with this STA
 * @param[in] nss      Maximum number of SS we can use with this STA
 *
 ****************************************************************************************
 */
void me_sta_bw_nss_max_upd(uint8_t sta_idx, uint8_t bw, uint8_t nss);

/**
 ****************************************************************************************
 * @brief Return the A-MSDU size allowed for a TID of one STA. This is sent to host for each
 * tx confirmation.
 *
 * @param[in] txdesc Tx descriptor (Containing STA and TID information)
 * @return The amsdu size allowed for the TID of this STA
 *
 ****************************************************************************************
 */
uint16_t me_tx_cfm_amsdu(struct txdesc *txdesc);

/**
 ****************************************************************************************
 * @brief Gets the maximum MCS supported (11ac station)
 * @param[in]  mcs_map Bitmap of supported MCSs
 * @return The maximum MCS
 ****************************************************************************************
 */
uint8_t me_11ac_mcs_max(uint16_t mcs_map);

/**
 ****************************************************************************************
 * @brief Gets the maximum MCS supported (11ax station)
 * @param[in]  mcs_map Bitmap of supported MCSs
 * @return The maximum MCS
 ****************************************************************************************
 */
uint8_t me_11ax_mcs_max(uint16_t mcs_map);

/**
 ****************************************************************************************
 * @brief Gets the maximum number of spatial streams supported (11ac station)
 * @param[in]  mcs_map Bitmap of supported MCSs
 * @return The maximum number of spatial streams
 ****************************************************************************************
 */
uint8_t me_11ac_nss_max(uint16_t mcs_map);

/**
 ****************************************************************************************
 * @brief Gets the maximum number of spatial streams supported (11n station)
 * @param[in]  mcs_set Pointer to the Supported MCS Set array
 * @return The RX maximum number of spatial streams
 ****************************************************************************************
 */
uint8_t me_11n_nss_max(uint8_t *mcs_set);


/**
 ****************************************************************************************
 * @brief Gets the minimum legacy rate index supported
 * @param[in]  rate_map Bitmap of legacy rate indexes
 * @return The minimum rate index
 ****************************************************************************************
 */
uint8_t me_legacy_ridx_min(uint16_t rate_map);

/**
 ****************************************************************************************
 * @brief Gets the maximum legacy rate index supported
 * @param[in]  rate_map Bitmap of legacy rate indexes
 * @return The maximum rate index
 ****************************************************************************************
 */
uint8_t me_legacy_ridx_max(uint16_t rate_map);

/**
 ****************************************************************************************
 * @brief Compute the A-MPDU parameters of a peer STA (maximum A-MPDU length, minimum
 * spacing), based on the HT, VHT and HE capabilities.
 * @param[in] ht_cap Pointer to the HT capabilities (NULL if not supported)
 * @param[in] vht_cap Pointer to the VHT capabilities (NULL if not supported)
 * @param[in] he_cap Pointer to the HE capabilities (NULL if not supported)
 * @param[out] ampdu_size_max_ht Maximum A-MPDU size in HT mode
 * @param[out] ampdu_size_max_vht Maximum A-MPDU size in VHT mode
 * @param[out] ampdu_size_max_he Maximum A-MPDU size in HE mode
 * @param[out] ampdu_spacing_min Minimum MPDU start spacing
 ****************************************************************************************
 */
void me_get_ampdu_params(struct mac_htcapability const *ht_cap,
                         struct mac_vhtcapability const *vht_cap,
                         struct mac_hecapability const *he_cap,
                         uint16_t *ampdu_size_max_ht,
                         uint32_t *ampdu_size_max_vht,
                         uint32_t *ampdu_size_max_he,
                         uint8_t *ampdu_spacing_min);

/**
 ****************************************************************************************
 * @brief Update chan bandwidth and center freqs from channel width, channel center
 * frequency segment0 and channel center frequency segment1
 *
 * This is the information found on VHT operation and Wide Bandwidth Channel Switch
 * elements.
 * If the new bandwidth is not supported by the PHY (e.g. 160Mhz or 80+80MHz) then the
 * bandwidth is limited to the 'primary' 80MHZ channel.
 *
 * @param[in]     width    Channel Width
 * @param[in]     center0  Channel Center Frequency Segment 0
 * @param[in]     center1  Channel Center Frequency Segment 1
 * @param[in,out] chan     Channel description to update. 'band' and 'prim20_freq' fields
 *                         must be valid when calling this function
 ****************************************************************************************
 */
void me_vht_bandwidth_parse(uint8_t width, uint8_t center0, uint8_t center1,
                            struct mac_chan_op *chan);
#if BK_MAC
uint8_t me_get_rate(uint32_t rate_info);
bool me_11n_mcs_set_valid(uint8_t *mcs_set);
#endif

/// @}

#endif  // _ME_UTILS_H_
