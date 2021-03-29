/**
 ****************************************************************************************
 *
 * @file hal_desc.h
 *
 * @brief File containing the definition of HW descriptors.
 *
 * File containing the definition of the structure and API function used to initialise the pool.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

#ifndef _HAL_DESC_H_
#define _HAL_DESC_H_

/**
 *****************************************************************************************
 * @defgroup HWDESC HWDESC
 * @ingroup HAL
 * @brief HW descriptor definitions
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"
// for mac_addr
#include "mac.h"
// for mac frame definitions
#include "mac_frame.h"
// for dma_desc
#include "dma.h"
// for CO_BIT
#include "co_math.h"
// for co_list_hdr
#include "co_list.h"
// for phy_channel_info
#include "phy.h"
// for DBG info format
#include "_dbg.h"
#if (NX_RADAR_DETECT || NX_UF_EN)
// for hal_dma_desc
#include "hal_dma.h"
#endif //(NX_RADAR_DETECT)

/*
 * DEFINES
 ****************************************************************************************
 */
/// Number of pulses in a radar event structure
#define RADAR_PULSE_MAX   4
/// Number of radar event structures
#define RADAR_EVENT_MAX   10
///Number of rx vectors of unsupported frames
#define UNSUP_RX_VECT_MAX   8

/// uPattern for TX header descriptor.
#define TX_HEADER_DESC_PATTERN 0xCAFEBABE
/// uPattern for TX buffer descriptor
#define TX_PAYLOAD_DESC_PATTERN 0xCAFEFADE
/// uPattern for RX header descriptor.
#define RX_HEADER_DESC_PATTERN 0xBAADF00D
/// uPattern for RX payload descriptor.
#define RX_PAYLOAD_DESC_PATTERN 0xC0DEDBAD

#if NX_MON_DATA
#define RX_MACHDR_BACKUP_LEN    64
#endif

//----------------------------------------------------------------------------------------
// TBD: BUF CONTROL INFO definitions
//----------------------------------------------------------------------------------------
/// Flag indicating if HW handled the buffer
#define TBD_DONE_HW             CO_BIT(31)
/// Bit allowing to request HW to generate an interrupt upon a payload buffer transmission
#define TBD_INTERRUPT_EN        CO_BIT(0)

//----------------------------------------------------------------------------------------
// THD: PHY CONTROL INFO definitions
//----------------------------------------------------------------------------------------
#if !NX_MAC_HE
/// Sounding of PPDU Frame Transmission (Bit 0)
#define SOUNDING_TX_BIT         CO_BIT(0)
/// Smoothing for PPDU frames (Bit 1)
#define SMOOTHING_TX_BIT        CO_BIT(1)
/// Smoothing for Control frames (Bit 2)
#define SMOOTHING_PROT_TX_BIT   CO_BIT(2)
#endif
/// Use BW signaling bit
#define USE_BW_SIG_TX_BIT       CO_BIT(3)
/// Dynamic BW
#define DYN_BW_TX_BIT           CO_BIT(4)
/// Doze allowed by AP during TXOP
#define DOZE_ALLOWED_TX_BIT     CO_BIT(5)
/// Continuous transmit
#define CONT_TX_BIT             CO_BIT(6)
/// User Position field offset
#define USER_POS_OFT            12
/// User Position field mask
#define USER_POS_MASK           (0x3 << USER_POS_OFT)
/// Use RIFS for Transmission (Bit 14)
#define USE_RIFS_TX_BIT         CO_BIT(14)
/// Use MU-MIMO for Transmission (Bit 15)
#define USE_MUMIMO_TX_BIT       CO_BIT(15)
/// GroupId field offset
#define GID_TX_OFT              16
/// GroupId field mask
#define GID_TX_MASK             (0x3F << GID_TX_OFT)
/// Partial AID field offset
#define PAID_TX_OFT             22
/// Partial AID field mask
#define PAID_TX_MASK            (0x1FF << PAID_TX_OFT)

//----------------------------------------------------------------------------------------
// THD: MAC CONTROL INFO 1 definitions
//----------------------------------------------------------------------------------------
/// Protection Frame Duration offset
#define PROT_FRM_DURATION_OFT             16
/// Protection Frame Duration mask
#define PROT_FRM_DURATION_MSK             (0xFFFF << PROT_FRM_DURATION_OFT)

/// Set if ACK has to be passed to SW
#define WRITE_ACK                         CO_BIT(14)
/// Set if lower rates have to be used for retries
#define LOW_RATE_RETRY                    CO_BIT(13)
/// L-SIG TXOP Protection for protection frame
#define LSTP_PROT                         CO_BIT(12)
/// L-SIG TXOP Protection
#define LSTP                              CO_BIT(11)

// Expected Acknowledgment
/// Expected Acknowledgment offset
#define EXPECTED_ACK_OFT                  9
/// Expected Acknowledgment mask
#define EXPECTED_ACK_MSK                  (0x3 << EXPECTED_ACK_OFT)
/// No acknowledgment
#define EXPECTED_ACK_NO_ACK               (0x0 << EXPECTED_ACK_OFT)
/// Normal acknowledgment
#define EXPECTED_ACK_NORMAL_ACK           (0x1 << EXPECTED_ACK_OFT)
/// Uncompressed block acknowledgment
#define EXPECTED_ACK_BLOCK_ACK            (0x2 << EXPECTED_ACK_OFT)
/// Compressed block acknowledgment
#define EXPECTED_ACK_COMPRESSED_BLOCK_ACK (0x3 << EXPECTED_ACK_OFT)

//----------------------------------------------------------------------------------------
// THD MACCTRLINFO2 fields
//----------------------------------------------------------------------------------------
/// Type and Subtype fields Valid bit
#define TS_VALID_BIT                      CO_BIT(0)

/// Type field offset
#define FRM_TYPE_OFT                      1
/// Type field mask
#define FRM_TYPE_MSK                      (0x3<<FRM_TYPE_OFT)

/// Management type
#define FRM_TYPE_MNG                      (0<<FRM_TYPE_OFT)
/// Control type
#define FRM_TYPE_CNTL                     (1<<FRM_TYPE_OFT)
/// Data type
#define FRM_TYPE_DATA                     (2<<FRM_TYPE_OFT)

/// Subtype field offset
#define FRM_SUBTYPE_OFT                   3
/// Subtype field mask
#define FRM_SUBTYPE_MSK                   (0xF<<FRM_SUBTYPE_OFT)

/// BAR subtype
#define FRM_CNTL_SUBTYPE_BAR              (8<<FRM_SUBTYPE_OFT)
/// BA subtype
#define FRM_CNTL_SUBTYPE_BA               (9<<FRM_SUBTYPE_OFT)

/// Bit indicating if an interrupt has to be set or not once packet is transmitted
#define INTERRUPT_EN_TX                   CO_BIT(8)

/// Offset of Number of Blank delimiters
#define NB_BLANK_DELIM_OFT                9
/// Mask of Number of Blank delimiters
#define NB_BLANK_DELIM_MSK                (0x3FF << NB_BLANK_DELIM_OFT)

/// WhichDescriptor definition - contains aMPDU bit and position value
/// Offset of WhichDescriptor field in the MAC CONTROL INFO 2 word
#define WHICHDESC_OFT                     19
/// Mask of the WhichDescriptor field
#define WHICHDESC_MSK                     (0x07 << WHICHDESC_OFT)
/// Only 1 THD possible, describing an unfragmented MSDU
#define WHICHDESC_UNFRAGMENTED_MSDU       (0x00 << WHICHDESC_OFT)
/// THD describing the first MPDU of a fragmented MSDU
#define WHICHDESC_FRAGMENTED_MSDU_FIRST   (0x01 << WHICHDESC_OFT)
/// THD describing intermediate MPDUs of a fragmented MSDU
#define WHICHDESC_FRAGMENTED_MSDU_INT     (0x02 << WHICHDESC_OFT)
/// THD describing the last MPDU of a fragmented MSDU
#define WHICHDESC_FRAGMENTED_MSDU_LAST    (0x03 << WHICHDESC_OFT)
/// THD for extra descriptor starting an AMPDU
#define WHICHDESC_AMPDU_EXTRA             (0x04 << WHICHDESC_OFT)
/// THD describing the first MPDU of an A-MPDU
#define WHICHDESC_AMPDU_FIRST             (0x05 << WHICHDESC_OFT)
/// THD describing intermediate MPDUs of an A-MPDU
#define WHICHDESC_AMPDU_INT               (0x06 << WHICHDESC_OFT)
/// THD describing the last MPDU of an A-MPDU
#define WHICHDESC_AMPDU_LAST              (0x07 << WHICHDESC_OFT)

/// aMPDU bit offset
#define AMPDU_OFT                         21
/// aMPDU bit
#define AMPDU_BIT                         CO_BIT(AMPDU_OFT)

/// Under BA setup bit
#define UNDER_BA_SETUP_BIT                CO_BIT(22)

/// Don't touch duration bit
#define DONT_TOUCH_DUR                    CO_BIT(28)


//----------------------------------------------------------------------------------------
//THD STATINFO fields
//----------------------------------------------------------------------------------------
/// Number of RTS frame retries offset
#define NUM_RTS_RETRIES_OFT                0
/// Number of RTS frame retries mask
#define NUM_RTS_RETRIES_MSK               (0xFF << NUM_RTS_RETRIES_OFT)
/// Number of MPDU frame retries offset
#define NUM_MPDU_RETRIES_OFT               8
/// Number of MPDU frame retries mask
#define NUM_MPDU_RETRIES_MSK              (0xFF << NUM_MPDU_RETRIES_OFT)
/// Retry limit reached: frame unsuccessful
#define RETRY_LIMIT_REACHED_BIT            CO_BIT(16)
/// Frame lifetime expired: frame unsuccessful
#define LIFETIME_EXPIRED_BIT               CO_BIT(17)
/// BA frame not received - valid only for MPDUs part of AMPDU
#define BA_FRAME_RECEIVED_BIT              CO_BIT(18)
/// Frame was transmitted in a HE TB PPDU - Set by SW
#define HE_TB_TX_BIT                       CO_BIT(22)
/// Frame successful by TX DMA: Ack received successfully
#define FRAME_SUCCESSFUL_TX_BIT            CO_BIT(23)
/// Last MPDU of an A-MPDU
#define A_MPDU_LAST                        (0x0F << 26)
/// Transmission bandwidth offset
#define BW_TX_OFT                          24
/// Transmission bandwidth mask
#define BW_TX_MSK                          (0x3 << BW_TX_OFT)
/// Transmission bandwidth - 20MHz
#define BW_20MHZ_TX                        (0x0 << BW_TX_OFT)
/// Transmission bandwidth - 40MHz
#define BW_40MHZ_TX                        (0x1 << BW_TX_OFT)
/// Transmission bandwidth - 80MHz
#define BW_80MHZ_TX                        (0x2 << BW_TX_OFT)
/// Transmission bandwidth - 160MHz
#define BW_160MHZ_TX                       (0x3 << BW_TX_OFT)
/// Descriptor done bit: Set by HW for TX DMA
#define DESC_DONE_TX_BIT                   CO_BIT(31)
/// Descriptor done bit: Set by SW for TX DMA
#define DESC_DONE_SW_TX_BIT                CO_BIT(30)

/// @name Policy table definitions
/// @{

/// Number of rate control steps in the policy table
#define RATE_CONTROL_STEPS      4

/// uPattern for Policy Table
#define POLICY_TABLE_PATTERN    0xBADCAB1E

// Policy Table: PHY Control 1 Information field
#if NX_MAC_HE
/// Sounding PPDU offset
#define SOUNDING_TX_OFT         0
/// Sounding PPDU bit
#define SOUNDING_TX_BIT         CO_BIT(SOUNDING_TX_OFT)
/// Smoothing recommended for PPDU offset
#define SMOOTHING_TX_OFT        1
/// Smoothing recommended for PPDU bit
#define SMOOTHING_TX_BIT        CO_BIT(SMOOTHING_TX_OFT)
/// Smoothing recommended for protection offset
#define SMOOTHING_PROT_TX_OFT   2
/// Smoothing recommended for protection bit
#define SMOOTHING_PROT_TX_BIT   CO_BIT(SMOOTHING_PROT_TX_OFT)
#endif
/// Beam Forming Frame Exchange offset
#define BF_FORM_EXT_PT_OFT      3
/// Beam Forming Frame Exchange mask
#define BF_FORM_EXT_PT_MASK     (0x1 << BF_FORM_EXT_PT_OFT)
/// Number of Extension Spatial Streams offset
#define NO_EXTN_SS_PT_OFT       4
/// Number of Extension Spatial Streams mask
#define NO_EXTN_SS_PT_MASK      (0x3 << NO_EXTN_SS_PT_OFT)
/// FEC Coding offset
#define FEC_CODING_PT_OFT       6
/// FEC Coding bit
#define FEC_CODING_PT_BIT       CO_BIT(FEC_CODING_PT_OFT)
/// Space Time Block Coding offset
#define STBC_PT_OFT             7
/// Space Time Block Coding mask
#define STBC_PT_MASK            (0x3 << STBC_PT_OFT)
/// Number of Transmit Chains for PPDU offset
#define NX_TX_PT_OFT            14
/// Number of Transmit Chains for PPDU mask
#define NX_TX_PT_MASK           (0x7 << NX_TX_PT_OFT)
/// Number of Transmit Chains for Protection Frame offset
#define NX_TX_PROT_PT_OFT       17
/// Number of Transmit Chains for Protection Frame mask
#define NX_TX_PROT_PT_MASK      (0x7 << NX_TX_PROT_PT_OFT)
/// Doppler offset
#define DOPPLER_OFT             20
/// Doppler bit
#define DOPPLER_BIT             CO_BIT(DOPPLER_OFT)
/// Spatial reuse offset
#define SPATIAL_REUSE_OFT       24
/// Spatial reuse mask
#define SPATIAL_REUSE_MASK      (0xF << SPATIAL_REUSE_OFT)

// Policy Table: PHY Control 2 Information field
/// Antenna Set offset
#define ANTENNA_SET_PT_OFT      0
/// Antenna Set mask
#define ANTENNA_SET_PT_MASK     0XFF
/// Spatial Map Matrix Index offset
#define SMM_INDEX_PT_OFT        8
/// Spatial Map Matrix Index mask
#define SMM_INDEX_PT_MASK       (0XFF << SMM_INDEX_PT_OFT)
/// Beamformed Offset
#define BMFED_OFT               16
/// Beamformed Bit
#define BMFED_BIT               CO_BIT(BMFED_OFT)
/// Beam Change Offset
#define BMCHANGE_OFT            17
/// Beam Change Bit
#define BMCHANGE_BIT            CO_BIT(BMCHANGE_OFT)
/// Uplink Flag Offset
#define UPLINK_FLAG_OFT         18
/// Uplink Flag Bit
#define UPLINK_FLAG_BIT         CO_BIT(UPLINK_FLAG_OFT)
/// BSS Color offset
#define BSS_COLOR_OFT           20
/// BSS Color mask
#define BSS_COLOR_MASK         (0X3F << BSS_COLOR_OFT)
/// Packet Extension offset
#define PKT_EXTENSION_OFT       26
/// Packet Extension mask
#define PKT_EXTENSION_MASK     (0X07 << PKT_EXTENSION_OFT)

// Policy Table: MAC Control 1 Information field
/// Key Storage RAM Index offset
#define KEYSRAM_INDEX_OFT       0
/// Key Storage RAM Index mask
#define KEYSRAM_INDEX_MASK      (0X3FF << KEYSRAM_INDEX_OFT)
/// Key Storage RAM Index for Receiver Address offset
#define KEYSRAM_INDEX_RA_OFT    10
/// Key Storage RAM Index for Receiver Address mask
#define KEYSRAM_INDEX_RA_MASK   (0X3FF << KEYSRAM_INDEX_RA_OFT)

// Policy Table: MAC Control 2 Information field.
/// dot11LongRetryLimit MIB Value offset
#define LONG_RETRY_LIMIT_OFT    0
/// dot11LongRetryLimit MIB Value mask
#define LONG_RETRY_LIMIT_MASK   (0XFF << LONG_RETRY_LIMIT_OFT)
/// dot11ShortRetryLimit MIB Value offset
#define SHORT_RETRY_LIMIT_OFT   8
/// dot11ShortRetryLimit MIB Value mask
#define SHORT_RETRY_LIMIT_MASK  (0XFF << SHORT_RETRY_LIMIT_OFT)
/// dot11RtsThrshold MIB Value offset
#define RTS_THRSHOLD_OFT        16 // Bits 16-23
/// dot11RtsThrshold MIB Value mask
#define RTS_THRSHOLD_MASK       (0XFF << RTS_THRSHOLD_OFT)

// Policy Table: Rate Control Information field
/// MCS Index offset
#define MCS_INDEX_TX_RCX_OFT    0
/// MCS Index mask
#define MCS_INDEX_TX_RCX_MASK   (0X7F << MCS_INDEX_TX_RCX_OFT)
/// Bandwidth for transmission offset
#define BW_TX_RCX_OFT           7
/// Bandwidth for transmission mask
#define BW_TX_RCX_MASK          (0X3 << BW_TX_RCX_OFT)
/// Short Guard Interval for Transmission offset
#define SHORT_GI_TX_RCX_OFT     9
/// Short Guard Interval for Transmission mask
#define SHORT_GI_TX_RCX_MASK    (0x1 << SHORT_GI_TX_RCX_OFT)
/// Preamble type for 11b Transmission offset
#define PRE_TYPE_TX_RCX_OFT     10
/// Preamble type for 11b Transmission mask
#define PRE_TYPE_TX_RCX_MASK    (0x1 << PRE_TYPE_TX_RCX_OFT)
/// Guard Interval/Preamble type for Transmission offset
#define HE_GI_TYPE_TX_RCX_OFT   9
/// Guard Interval/Preamble for Transmission mask
#define HE_GI_TYPE_TX_RCX_MASK  (0x3 << HE_GI_TYPE_TX_RCX_OFT)
/// HE 0.8us GI
#define GI_TYPE_0_8             (0x0 << HE_GI_TYPE_TX_RCX_OFT)
/// HE 1.6us GI
#define GI_TYPE_1_6             (0x1 << HE_GI_TYPE_TX_RCX_OFT)
/// HE 3.2us GI
#define GI_TYPE_3_2             (0x2 << HE_GI_TYPE_TX_RCX_OFT)
/// Format of the modulation offset
#define FORMAT_MOD_TX_RCX_OFT   11
/// Format of the modulation mask
#define FORMAT_MOD_TX_RCX_MASK  (0X7 << FORMAT_MOD_TX_RCX_OFT)
/// Type of NAV protection frame exchange offset
#define PROT_FRM_EX_RCX_OFT     14
/// Type of NAV protection frame exchange mask
#define PROT_FRM_EX_RCX_MASK    (0X7 << PROT_FRM_EX_RCX_OFT)
/// No protection
#define PROT_NO_PROT            (0x0 << PROT_FRM_EX_RCX_OFT)
/// Self-CTS
#define PROT_SELF_CTS           (0x1 << PROT_FRM_EX_RCX_OFT)
/// RTS-CTS with intended receiver
#define PROT_RTS_CTS            (0x2 << PROT_FRM_EX_RCX_OFT)
/// RTS-CTS with QAP
#define PROT_RTS_CTS_WITH_QAP   (0x3 << PROT_FRM_EX_RCX_OFT)
/// STBC protection
#define PROT_STBC               (0x4 << PROT_FRM_EX_RCX_OFT)

/// MCS Index for protection frame offset
#define MCS_INDEX_PROT_TX_RCX_OFT  17
/// MCS Index for protection frame mask
#define MCS_INDEX_PROT_TX_RCX_MASK (0x7F << MCS_INDEX_PROT_TX_RCX_OFT)
/// Bandwidth for protection frame transmission offset
#define BW_PROT_TX_RCX_OFT      24
/// Bandwidth for protection frame transmission mask
#define BW_PROT_TX_RCX_MASK     (0x3 << BW_PROT_TX_RCX_OFT)
/// Format of the modulation for the protection frame offset
#define FORMAT_MOD_PROT_TX_RCX_OFT  26
/// Format of the modulation for the protection frame mask
#define FORMAT_MOD_PROT_TX_RCX_MASK (0x7 << FORMAT_MOD_PROT_TX_RCX_OFT)
/// Number of retries at this rate offset
#define N_RETRY_RCX_OFT         29
/// Number of retries at this rate mask
#define N_RETRY_RCX_MASK        (0x7 << N_RETRY_RCX_OFT)

// Values of the FORMAT_MOD fields
/// Non-HT format
#define FORMATMOD_NON_HT          0
/// Non-HT duplicate OFDM format
#define FORMATMOD_NON_HT_DUP_OFDM 1
/// HT mixed mode format
#define FORMATMOD_HT_MF           2
/// HT greenfield format
#define FORMATMOD_HT_GF           3
/// VHT format
#define FORMATMOD_VHT             4
/// HE-SU format
#define FORMATMOD_HE_SU           5
/// HE-MU format
/// @note This format cannot be used in the policy table but is used when reporting HE
/// TB transmissions/MU receptions to upper layers
#define FORMATMOD_HE_MU           6
/// HE-ER format
#define FORMATMOD_HE_ER           7

/// 20 MHz bandwidth
#define BW_20MHZ                  0
/// 40 MHz bandwidth
#define BW_40MHZ                  1
/// 80 MHz bandwidth
#define BW_80MHZ                  2
/// 160 or 80+80 MHz bandwidth
#define BW_160MHZ                 3

// VHT specific NSS and MCS values
/// VHT Nss offset
#define VHT_NSS_OFT               4
/// VHT Nss mask
#define VHT_NSS_MASK              (0x7 << VHT_NSS_OFT)
/// VHT MCS offset
#define VHT_MCS_OFT               0
/// VHT MCS mask
#define VHT_MCS_MASK              (0xF << VHT_MCS_OFT)

// HT specific NSS and MCS values (valid only for MCS <= 31)
/// Offset of the NSS in a HT MCS value (valid for MCS <= 31)
#define HT_NSS_OFT               3
/// Mask of the NSS in a HT MCS value (valid for MCS <= 31)
#define HT_NSS_MASK              (0x3 << HT_NSS_OFT)
/// Offset of the MCS in a HT MCS value (valid for MCS <= 31)
#define HT_MCS_OFT               0
/// Mask of the MCS in a HT MCS value (valid for MCS <= 31)
#define HT_MCS_MASK              (0x7 << HT_MCS_OFT)

// Policy Table: Power Control Information field
/// Transmit Power Level for RCX offset
#define TX_PWR_LEVEL_PT_RCX_OFT         0
/// Transmit Power Level for RCX mask
#define TX_PWR_LEVEL_PT_RCX_MASK        (0xff << TX_PWR_LEVEL_PT_RCX_OFT)
/// Transmit Power Level of Protection for RCX offset
#define TX_PWR_LEVEL_PROT_PT_RCX_OFT    8
/// Transmit Power Level of Protection for RCX mask
#define TX_PWR_LEVEL_PROT_PT_RCX_MASK   (0xff << TX_PWR_LEVEL_PROT_PT_RCX_OFT)
/// Transmit Power Level mask
#define TX_PWR_LEVEL_MASK               (TX_PWR_LEVEL_PT_RCX_MASK | TX_PWR_LEVEL_PROT_PT_RCX_MASK)
/// Macro used to compute both the Protection and Data power indexes
#define TX_PWR_LEVEL_SET(pwr)           (((pwr) << TX_PWR_LEVEL_PROT_PT_RCX_OFT) |       \
                                         ((pwr) << TX_PWR_LEVEL_PT_RCX_OFT))
/// HE LTF type for RCX offset
#define TX_HE_LTF_TYPE_PT_RCX_OFT       16
/// HE LTF type for RCX mask
#define TX_HE_LTF_TYPE_PT_RCX_MASK      (0x03 << TX_HE_LTF_TYPE_PT_RCX_OFT)
/// 1x HE-LTF for 3.2 µs
#define TX_1x_HE_LTF_FOR_3_2_US         (0x00 << TX_HE_LTF_TYPE_PT_RCX_OFT)
/// 2x HE-LTF for 6.4 µs
#define TX_2x_HE_LTF_FOR_6_4_US         (0x01 << TX_HE_LTF_TYPE_PT_RCX_OFT)
/// 4x HE-LTF for 12.8 µs
#define TX_4x_HE_LTF_FOR_12_8_US        (0x02 << TX_HE_LTF_TYPE_PT_RCX_OFT)
/// DCM bit offset
#define TX_HE_DCM_OFT                   18
/// DCM Bit
#define TX_HE_DCM_BIT                   CO_BIT(TX_HE_DCM_OFT)
/// @}

/// @name Compressed Policy table definitions
/// @{

// Compressed Policy Table: Secondary User Information field
/// MCS index offset
#define MCS_IDX_TX_CPT_OFT      0
/// MCS index mask
#define MCS_IDX_TX_CPT_MASK     (0x7F << MCS_IDX_TX_CPT_OFT)
/// FEC Coding offset
#define FEC_CODING_CPT_OFT      7
/// FEC Coding bit
#define FEC_CODING_CPT_BIT      CO_BIT(FEC_CODING_CPT_OFT)
/// Spatial Map Matrix Index offset
#define SMM_INDEX_CPT_OFT       8
/// Spatial Map Matrix Index mask
#define SMM_INDEX_CPT_MASK      (0XFF << SMM_INDEX_CPT_OFT)
/// Key Storage RAM Index offset
#define KEYSRAM_INDEX_CPT_OFT   16
/// Key Storage RAM Index mask
#define KEYSRAM_INDEX_CPT_MASK  (0X3FF << KEYSRAM_INDEX_OFT)

/// @}

/// @name RHD STATINFO
/// @{

/// Key index offset
#define KEY_IDX_OFT                       15
/// Key index mask
#define KEY_IDX_MSK                       (0x3FF << KEY_IDX_OFT)
/// Key index valid bit
#define KEY_IDX_VALID_BIT                 CO_BIT(25)
/// Immediate response access category offset
#define IMM_RSP_AC_OFT                    11
/// Immediate response access category mask
#define IMM_RSP_AC_MSK                    (0x03 << IMM_RSP_AC_OFT)

/// Last buffer of the MPDU
#define RX_PD_LASTBUF 0x0001
/// Descriptor Done in HW
#define RX_PD_DONE 0x0002

/// Storage RAM key index valid bit.
#define RX_HD_KEYIDV 0x02000000
/// Storage RAM key index.
#define RX_HD_KEYID 0x01FF8000
/// Lowest significant bit of the storage RAM key index.
#define RX_HD_KEYID_LSB 15
/// Done bit.
#define RX_HD_DONE 0x00004000
/// Frame successfully received bit.
#define RX_HD_SUCCESS 0x00002000
/// Group Addressed frame bit.
#define RX_HD_GA_FRAME 0x00000400
/// Address mismatch bit.
#define RX_HD_ADDRMIS 0x00000200
/// FCS error bit.
#define RX_HD_FCSERR 0x0100
#if NX_RX_RING
/// Undefined error bit.
#define RX_HD_UNDEF_ERR 0x00000080
/// Decryption error bit.
#define RX_HD_DECR_ERR 0x00000040
/// Decryption type mask
#define RX_HD_DECRTYPE_MSK 0x0000003C
/// Decryption type offset
#define RX_HD_DECRTYPE_OFT 2
/// Frame unencrypted.
#define RX_HD_DECR_UNENC (0x00 << RX_HD_DECRTYPE_OFT)
/// Frame decrypted using WEP.
#define RX_HD_DECR_WEP (0x01 << RX_HD_DECRTYPE_OFT)
/// Frame decrypted using TKIP.
#define RX_HD_DECR_TKIP (0x02 << RX_HD_DECRTYPE_OFT)
/// Frame decrypted using CCMP 128bits.
#define RX_HD_DECR_CCMP128 (0x03 << RX_HD_DECRTYPE_OFT)
/// Frame decrypted using CCMP 256 bits.
#define RX_HD_DECR_CCMP256 (0x04 << RX_HD_DECRTYPE_OFT)
/// Frame decrypted using GCMP 128 bits.
#define RX_HD_DECR_GCMP128 (0x05 << RX_HD_DECRTYPE_OFT)
/// Frame decrypted using GCMP 256 bits.
#define RX_HD_DECR_GCMP256 (0x06 << RX_HD_DECRTYPE_OFT)
/// Frame decrypted using WAPI.
#define RX_HD_DECR_WAPI (0x07 << RX_HD_DECRTYPE_OFT)
/// Frame not decrypted because cipher is unknown.
#define RX_HD_DECR_UNK (0x0F << RX_HD_DECRTYPE_OFT)
/// Decryption status mask (includes type + error bit).
#define RX_HD_DECRSTATUS (RX_HD_DECR_ERR | RX_HD_DECRTYPE_MSK)
/// MAC security type WEP.
#define RX_HD_DECR_WEP_SUCCESS (RX_HD_DECR_WEP)
/// MAC security type TKIP.
#define RX_HD_DECR_TKIP_SUCCESS (RX_HD_DECR_TKIP)
/// MAC security type CCMP 128 bits.
#define RX_HD_DECR_CCMP128_SUCCESS (RX_HD_DECR_CCMP128)
/// MAC security type CCMP 256 bits.
#define RX_HD_DECR_CCMP256_SUCCESS (RX_HD_DECR_CCMP256)
/// MAC security type GCMP 128 bits.
#define RX_HD_DECR_GCMP128_SUCCESS (RX_HD_DECR_GCMP128)
/// MAC security type GCMP 256 bits.
#define RX_HD_DECR_GCMP256_SUCCESS (RX_HD_DECR_GCMP256)
/// MAC security type WAPI.
#define RX_HD_DECR_WAPI_SUCCESS (RX_HD_DECR_WAPI)
#else
/// PHY error bit.
#define RX_HD_PHY_ERR 0x00000080
/// Undefined error bit.
#define RX_HD_UNDEF_ERR 0x00000040
/// Decryption status mask.
#define RX_HD_DECRSTATUS 0x0000001C
/// Frame unencrypted.
#define RX_HD_DECR_UNENC 0x0000
/// MAC security type WEP.
#define RX_HD_DECR_WEP_SUCCESS 0x0014
/// MAC security type TKIP.
#define RX_HD_DECR_TKIP_SUCCESS 0x0018
/// MAC security type CCMP.
#define RX_HD_DECR_CCMP128_SUCCESS 0x001C
#endif
/// Is response frame bit.
#define RX_HD_RSP_FRM 0x00000002
/// Vector 2 valid bit.
#define RX_HD_RXVEC2V 0x00000001
/// Macro to retrieve the storage RAM key index for the received frame.
/// @param[in] __s MPDU status information from the RX header descriptor.
#define RX_HD_KEYID_GET(__s) (((__s) & RX_HD_KEYID) >> RX_HD_KEYID_LSB)
/// Macro to retrieve the done bit of the received frame.
/// @param[in] __s MPDU status information from the RX header descriptor.
#define RX_HD_DONE_GET(__s) ((__s) & RX_HD_DONE)
/// Macro to retrieve the success bit of the received frame.
/// @param[in] __s MPDU status information from the RX header descriptor.
#define RX_HD_SUCCESS_GET(__s) ((__s) & (RX_HD_SUCCESS | RX_HD_FCSERR))

/// @}

/// Length of the receive vectors
#define RXL_HWDESC_RXV_LEN    40



/// @name RX VECTOR related definitions.
/// @{

///Number of 32 bits registers forming the RX vector
#define RXVEC1_NUM_REG      4

#if (NX_MAC_VER >= 20)
/// Macro retrieving the modulation format of the RX vector
/// @param[in] __x bytes 4 - 1 of Receive Vector 1.
#define RXVEC_FORMATMOD(__x)   (((__x) & 0x0000000F))
/// Macro retrieving the preamble type of the RX vector
/// @param[in] __x bytes 4 - 1 of Receive Vector 1.
#define RXVEC_PRETYPE(__x)     (((__x) & 0x00000080) >> 7)
/// Macro retrieving the legacy rate of the RX vector
/// @param[in] __x bytes 8 - 5 of Receive Vector 1.
#define RXVEC_LEGRATE(__x)     (((__x) & 0x000000F0) >> 4)
/// Macro retrieving the RSSI of the RX vector
/// @param[in] __x bytes 8 - 5 of Receive Vector 1.
#ifdef CFG_RWTL
#define RXVEC_RSSI1(__x)       ((int32_t)((__x) & 0x0000FF00) >> 8)
#else
#define RXVEC_RSSI1(__x)       (((__x) & 0x0000FF00) >> 8)
#endif
/// Macro retrieving the HT length of the RX vector
/// @param[in] __x bytes 12 - 9 of Receive Vector 1.
#define RXVEC_HTLENGTH_0(__x)    (((__x) & 0x0000FFFF))
/// Macro retrieving the antenna set of the RX vector
/// @param[in] __x bytes 4 - 1 of Receive Vector 1.
#define RXVEC_ANTENNA_SET(__x) (((__x) & 0x0000FF00) >> 8)

#else // (NX_MAC_VER < 20)
/// Macro retrieving the legacy rate of the RX vector
/// @param[in] __x RX Vector 1A value.
#define RXVEC_LEGRATE(__x)     (((__x) & 0x0000F000) >> 12)
/// Macro retrieving the HT length [15:0] of the RX vector
/// @param[in] __x RX Vector 1A value.
#define RXVEC_HTLENGTH_0(__x)    (((__x) & 0xFFFF0000) >> 16)
/// Macro retrieving the HT length [19:16] of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC_HTLENGTH_1(__x)    (((__x) & 0x0000000F))
/// Macro retrieving the preamble type of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC_PRETYPE(__x)     (((__x) & 0x00008000) >> 15)
/// Macro retrieving the modulation format of the RX vector
/// @param[in] __x RX Vector 1B value.
#define RXVEC_FORMATMOD(__x)   (((__x) & 0x00070000) >> 16)
/// Macro retrieving the 1st chain RSSI of the RX vector
/// @param[in] __x RX Vector 1C value.
#ifdef CFG_RWTL
#define RXVEC_RSSI1(__x)       ((int32_t)((__x) & 0xFF000000) >> 24)
#else
#define RXVEC_RSSI1(__x)       (((__x) & 0xFF000000) >> 24)
#endif
/// Macro retrieving the 2nd chain RSSI of the RX vector
/// @param[in] __x RX Vector 1D value.
#ifdef CFG_RWTL
#define RXVEC_RSSI2(__x)       ((int32_t)(((__x) & 0x000000FF) << 24) >> 24)
#else
#define RXVEC_RSSI2(__x)       (((__x) & 0x000000FF))
#endif
/// Macro retrieving the antenna set of the RX vector
/// @param[in] __x RX Vector 1C value.
#define RXVEC_ANTENNA_SET(__x) (((__x) & 0x000000FF))
#endif // (NX_MAC_VER >= 20)

/// @}

/// @name AMPDU Status Information related definitions.
/// @{
/// Macro retrieving the A-MPDU counter
/// @param[in] __x AMPDU Status Information value.
#define RX_AMPDU_AMPDUCNT(__x)   (((__x) & 0xC000) >> 14)
/// Macro retrieving the MPDU Counter inside the A-MPDU
/// @param[in] __x AMPDU Status Information value.
#define RX_AMPDU_MPDUCNT(__x)    ((__x) & 0x3F00)
/// @}


/// legacy RATE definitions
typedef enum
{
    /// 1Mbps
    HW_RATE_1MBPS = 0,
    /// 2Mbps
    HW_RATE_2MBPS,
    /// 5.5Mbps
    HW_RATE_5_5MBPS,
    /// 11Mbps
    HW_RATE_11MBPS,
    /// 6Mbps
    HW_RATE_6MBPS,
    /// 9Mbps
    HW_RATE_9MBPS,
    /// 12Mbps
    HW_RATE_12MBPS,
    /// 18Mbps
    HW_RATE_18MBPS,
    /// 24Mbps
    HW_RATE_24MBPS,
    /// 36Mbps
    HW_RATE_36MBPS,
    /// 48Mbps
    HW_RATE_48MBPS,
    /// 54Mbps
    HW_RATE_54MBPS
} HW_RATE_E;

/// MAC header content as defined in the MAC HW User Manual transmit MPDU template.
struct machdr
{
    /// Reserved.
    uint16_t reserved;
    /// Frame Control field.
    uint16_t framectrl;
    /// Duration/ID field.
    uint16_t duration;
    /// MAC Address 1.
    struct mac_addr macaddr1;
    /// MAC Address 2.
    struct mac_addr macaddr2;
    /// MAC Address 3.
    struct mac_addr macaddr3;
    /// Sequence control field.
    uint16_t seq_ctrl;
    /// MAC Address 4.
    struct mac_addr macaddr4;
    /// QoS control field.
    uint16_t qos_ctrl;
    /// Carried Frame Control field.
    uint16_t carriedfr_ctrl;
    /// High Throughput Control field.
    uint32_t ht_ctrl;
    /// Security initialization vector.
    uint32_t iv;
    /// Security extended initialization vector.
    uint32_t ext_iv;
};

/// Policy Table Structure used to store Policy Table Information used by MAC HW to
/// prepare transmit vector to used by PHY.
struct tx_policy_tbl
{
    /// Unique Pattern at the start of Policy Table
    uint32_t upatterntx;
    /// PHY Control Information 1 used by MAC HW
    uint32_t phycntrlinfo1;
    /// PHY Control Information 2 used by MAC HW
    uint32_t phycntrlinfo2;
    /// MAC Control Information 1 used by MAC HW
    uint32_t maccntrlinfo1;
    /// MAC Control Information 2 used by MAC HW
    uint32_t maccntrlinfo2;
    /// Rate Control Information used by MAC HW
    uint32_t ratecntrlinfo[RATE_CONTROL_STEPS];
    /// Power Control Information used by MAC HW
    uint32_t powercntrlinfo[RATE_CONTROL_STEPS];
};

/// Compressed Policy Table Structure used to store Policy Table Information used by MAC HW
/// to get TX information for secondary users in case of MU-MIMO PPDU transmission.
struct tx_compressed_policy_tbl
{
    /// Unique Pattern at the start of Policy Table
    uint32_t upatterntx;
    /// Secondary User Control Information
    uint32_t sec_user_control;
};

/// Definition of a TX header descriptor.
struct tx_hd
{
    /// Unique pattern for transmit DMA.
    uint32_t             upatterntx;
    /// Starting descriptor of the next atomic frame exchange sequence.
    uint32_t             nextfrmexseq_ptr;
    /// Next MPDU in the frame exchange.
    uint32_t             nextmpdudesc_ptr;
    /// First payload buffer descriptor/Secondary user 1 THD address
    union {
        /// First payload buffer descriptor
        uint32_t             first_pbd_ptr;
        /// Secondary user 1 THD address
        uint32_t             sec_user1_ptr;
    };
    /// Data buffer start/Secondary user 2 THD address.
    union {
        /// Data buffer start
        uint32_t             datastartptr;
        /// Secondary user 2 THD address
        uint32_t             sec_user2_ptr;
    };
    /// Data buffer end /Secondary user 3 THD address.
    union {
        /// Data buffer end
        uint32_t             dataendptr;
        /// Secondary user 3 THD address
        uint32_t             sec_user3_ptr;
    };
    /// Total length of the frame on air.
    uint32_t             frmlen;
    /// MSDU lifetime parameter (for EDCA frame).
    uint32_t             frmlifetime;
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             phyctrlinfo;
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             policyentryaddr;
    /// Optional length fields in case of BW drop
    uint32_t             optlen[3];
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             macctrlinfo1;
    /// Valid only for MPDU header descriptor and for singleton MPDUs.
    uint32_t             macctrlinfo2;
    /// Valid only for A-MPDU header descriptor and for singleton MPDUs.
    uint32_t             statinfo;
    /// Medium time used.
    uint32_t             mediumtimeused;
};

/// Definition of a TX payload buffer descriptor.
struct tx_pbd
{
    /// Unique pattern for transmit buffer descriptor.
    uint32_t             upatterntx;
    /// Next buffer descriptor of the MPDU when the MPDU is split over several buffers.
    uint32_t             next;
    /// Data start in the buffer associated with this buffer descriptor.
    uint32_t             datastartptr;
    /// Data end in the buffer associated with this buffer descriptor (inclusive).
    uint32_t             dataendptr;
    /// Buffer control for transmit DMA.
    uint32_t             bufctrlinfo;
};

/// Definition of a TX DMA descriptor.
struct tx_dmadesc
{
    /// Tx header descriptor
    struct tx_hd header;
    /// Tx buffer descriptor
    struct tx_pbd buffer;
    /// Mac header buffer
    struct machdr macheader;
};

#if BK_MAC
enum
{
    FMOD_NON_HT = 0,
    FMOD_NON_HT_DUP_OFDM,
    FMOD_HT_MF,
    FMOD_HT_GF,
    FMOD_VHT,
};
#endif

/// Structure for receive Vector 1
struct rx_vector_1
{
    /// Contains the bytes 4 - 1 of Receive Vector 1
    uint32_t            recvec1a;
    /// Contains the bytes 8 - 5 of Receive Vector 1
    uint32_t            recvec1b;
    /// Contains the bytes 12 - 9 of Receive Vector 1
    uint32_t            recvec1c;
    /// Contains the bytes 16 - 13 of Receive Vector 1
    uint32_t            recvec1d;
};

/// Structure for receive Vector 2
struct rx_vector_2
{
    /// Contains the bytes 4 - 1 of Receive Vector 2
    uint32_t            recvec2a;
    ///  Contains the bytes 8 - 5 of Receive Vector 2
    uint32_t            recvec2b;
};

/// Element in the pool of RX header descriptor.
struct rx_hd
{
    /// Unique pattern for receive DMA.
    uint32_t            upatternrx;
    /// Pointer to the location of the next Header Descriptor
    uint32_t            next;
    /// Pointer to the first payload buffer descriptor
    uint32_t            first_pbd_ptr;
    /// Pointer to the SW descriptor associated with this HW descriptor
    struct rxdesc      *rxdesc;
    /// Pointer to the address in buffer where the hardware should start writing the data
    uint32_t            datastartptr;
    /// Pointer to the address in buffer where the hardware should stop writing data
    uint32_t            dataendptr;
    /// Header control information. Except for a single bit which is used for enabling the
    /// Interrupt for receive DMA rest of the fields are reserved
    uint32_t            headerctrlinfo;
    /// Total length of the received MPDU
    uint16_t            frmlen;
    /// AMPDU status information
    uint16_t            ampdu_stat_info;
    /// TSF Low
    uint32_t            tsflo;
    /// TSF High
    uint32_t            tsfhi;
    /// Rx Vector 1
    struct rx_vector_1  rx_vec_1;
    /// Rx Vector 2
    struct rx_vector_2  rx_vec_2;
    /// MPDU status information
    uint32_t            statinfo;
};

/// Element in the pool of rx payload buffer descriptors.
struct rx_pbd
{
    /// Unique pattern
    uint32_t            upattern;
    /// Points to the next payload buffer descriptor of the MPDU when the MPDU is split
    /// over several buffers
    uint32_t            next;
    /// Points to the address in the buffer where the data starts
    uint32_t            datastartptr;
    /// Points to the address in the buffer where the data ends
    uint32_t            dataendptr;
    /// buffer status info for receive DMA.
    uint16_t            bufstatinfo;
    /// complete length of the buffer in memory
    uint16_t            reserved;
};

#if NX_MON_DATA
/// MAC header backup descriptor
struct rxu_machdrdesc
{
    /// Length of the buffer
    uint32_t buf_len;
    /// Buffer containing mac header, LLC and SNAP
    uint32_t buffer[RX_MACHDR_BACKUP_LEN / 4];
};
#endif

/// Definition of a Rx DMA header descriptor
struct rx_dmadesc
{
    /// Rx header descriptor (this element MUST be the first of the structure)
    struct rx_hd hd;
    /// Structure containing the information about the PHY channel that was used for this RX
    struct phy_channel_info phy_info;
    #if NX_UMAC_PRESENT
    /// Word containing some SW flags about the RX packet
    uint32_t flags;
    #if NX_AMSDU_DEAGG
    /// Array of host buffer identifiers for the other A-MSDU subframes
    uint32_t amsdu_hostids[NX_MAX_MSDU_PER_RX_AMSDU - 1];
    #endif
    #if NX_MON_DATA
    /// MAC header backup descriptor (used only for MSDU when there is a monitor and a data interface)
    struct rxu_machdrdesc mac_hdr_backup;
    #endif
    #endif
    /// Spare room for LMAC FW to write a pattern when last DMA is sent
    uint32_t pattern;
    /// IPC DMA control structure for MAC Header transfer
    struct dma_desc dma_desc;
};

/// Definition of a Rx DMA payload descriptor
struct rx_payloaddesc
{
    /// Mac header buffer (this element MUST be the first of the structure)
    struct rx_pbd pbd;
    /// IPC DMA control structures
    struct dma_desc dma_desc[NX_DMADESC_PER_RX_PDB_CNT];
    #if !NX_RX_RING
    /// Buffer containing the payload
    uint32_t buffer[NX_RX_PAYLOAD_LEN/4];
    #endif
};

/// Structure indicating the status and other information about the transmission
struct tx_cfm_tag
{
    #if NX_UMAC_PRESENT
    #if !NX_FULLY_HOSTED
    /// PN that was used for the transmission
    uint16_t pn[4];
    /// Sequence number of the packet
    uint16_t sn;
    /// Timestamp of first transmission of this MPDU
    uint16_t timestamp;
    #endif
    /// Number of flow control credits allocated
    int8_t credits;
    /// Size (in mdpu) of the A-MPDU in which this frame as been transmitted.
    /// Valid only for singleton (=1) or last frame of the A-MPDU
    uint8_t ampdu_size;
    #if NX_AMSDU_TX
    /// Size allowed for AMSDU
    uint16_t amsdu_size;
    #endif
    #endif

    /// TX status
    uint32_t status;
};

/// Definition of a TX confirmation descriptor
struct tx_hw_desc
{
    #if !NX_FULLY_HOSTED
    /// Status of the transmission
    struct tx_cfm_tag cfm;
    /// DMA control structure for status transfer
    struct dma_desc dma_desc;
    #endif
    /// TX header descriptor attached to the MPDU
    struct tx_hd    thd;
};

#if NX_RADAR_DETECT
/// Definition of an array of radar pulses
struct radar_pulse_array_desc
{
    /// Buffer containing the radar pulses
    struct phy_radar_pulse pulse[RADAR_PULSE_MAX];
    /// Index of the radar detection chain that detected those pulses
    uint32_t idx;
    /// Number of valid pulses in the buffer
    uint32_t cnt;
};

/// Definition of a Rx DMA payload descriptor
struct radar_event_desc
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// IPC DMA control structure
    struct dma_desc dma_desc;
    /// General Purpose DMA Control Structure
    struct hal_dma_desc_tag gp_dma_desc;
    /// Buffer containing the radar pulses
    struct radar_pulse_array_desc pulse_array;
};
#endif

#if NX_UF_EN
/// Definition of a RX vector descriptor
struct rx_vector_desc
{
    /// Structure containing the information about the PHY channel that was used for this RX
    struct phy_channel_info phy_info;

    /// Structure containing the rx vector 1
    struct rx_vector_1 rx_vec_1;
    #if !NX_FULLY_HOSTED
    /// Used to mark a valid rx vector
    uint32_t pattern;
    #endif // !NX_FULLY_HOSTED
};

/// Definition of a Rx DMA payload descriptor
struct unsup_rx_vector_desc
{
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    #if !NX_FULLY_HOSTED
    /// IPC DMA control structure
    struct dma_desc dma_desc;
    /// General Purpose DMA Control Structure
    struct hal_dma_desc_tag gp_dma_desc;
    /// RX vector
    #endif // !NX_FULLY_HOSTED
    struct rx_vector_desc rx_vector;
};
#endif

/// Control structure used for the debug information dump
struct debug_info_tag
{
    /// DMA descriptor for LA trace dump
    volatile struct dma_desc dma_desc;
    /// Debug information to be uploaded
    struct dbg_debug_info_tag dbg_info;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
#if NX_RADAR_DETECT
extern struct radar_event_desc radar_event_desc_array[RADAR_EVENT_MAX];
#endif

#if NX_UF_EN
extern struct unsup_rx_vector_desc rx_vector_desc_array[UNSUP_RX_VECT_MAX];
#endif

#if NX_BCN_AUTONOMOUS_TX
/// IPC DMA control structure for beacon download
extern struct dma_desc bcn_dwnld_desc;
#endif

/// Array of HW descriptors for BK queue
extern struct tx_hw_desc       tx_hw_desc0[RW_USER_MAX * NX_TXDESC_CNT0];
/// Array of HW descriptors for BE queue
extern struct tx_hw_desc       tx_hw_desc1[RW_USER_MAX * NX_TXDESC_CNT1];
/// Array of HW descriptors for VI queue
extern struct tx_hw_desc       tx_hw_desc2[RW_USER_MAX * NX_TXDESC_CNT2];
/// Array of HW descriptors for VO queue
extern struct tx_hw_desc       tx_hw_desc3[RW_USER_MAX * NX_TXDESC_CNT3];
#if (NX_BEACONING)
/// Array of HW descriptors for BCN queue
extern struct tx_hw_desc       tx_hw_desc4[NX_TXDESC_CNT4];
#endif

#if NX_DEBUG_DUMP
/// Structure containing the debug information to be uploaded to Upper MAC in case of error
extern struct debug_info_tag   debug_info;
#endif

#if BK_MAC
uint8_t phy_get_nrx(void);
#endif

/**
 *****************************************************************************************
 * @brief Retrieve the RSSI values from the RX vector
 *
 * @param[in] rx_vec_1  Receive Vector 1
 * @param[out] rx_rssi  Array of RSSI values for each RX path. (Optional)
 *
 * @return Average RSSI considering all RX paths.
 ****************************************************************************************
 */
__INLINE int8_t hal_desc_get_rssi(struct rx_vector_1 *rx_vec_1, int8_t *rx_rssi)
{
        int8_t rssi;
        int8_t rx_rssi_[2];

        if (!rx_rssi)
            rx_rssi = rx_rssi_;

        #if (NX_MAC_VER >= 20)
        rx_rssi[0] = RXVEC_RSSI1(rx_vec_1->recvec1b);
        rx_rssi[1] = RXVEC_RSSI1(rx_vec_1->recvec1b);
        #else
        rx_rssi[0] = RXVEC_RSSI1(rx_vec_1->recvec1c);
        rx_rssi[1] = RXVEC_RSSI2(rx_vec_1->recvec1d);
        #endif // (NX_MAC_VER >= 20)

        // Set the RSSI value
        if (phy_get_nrx() == 1)
        {
           rssi = (rx_rssi[0] + rx_rssi[1])/2;
        }
        else
        {
           rssi = rx_rssi[0];
        }

        return rssi;
}

/**
 *****************************************************************************************
 * @brief Retrieve the length of the HT PPDU from the RX vector
 *
 * @param[in] rx_vec_1 Receive Vector 1
 *
 * @return HT PPDU length .
 ****************************************************************************************
 */
__INLINE uint16_t hal_desc_get_ht_length(struct rx_vector_1 *rx_vec_1)
{
    uint16_t length;

    #if (NX_MAC_VER >= 20)
    length = RXVEC_HTLENGTH_0(rx_vec_1->recvec1c);
    #else
    length = RXVEC_HTLENGTH_0(rx_vec_1->recvec1a);
    #endif // (NX_MAC_VER >= 20)

    return length;
}

/**
 *****************************************************************************************
 * @brief Retrieve the modulation format from the RX vector
 *
 * @param[in] rx_vec_1 Receive Vector 1
 *
 * @return the modulation format value.
 ****************************************************************************************
 */
__INLINE uint8_t hal_desc_get_rx_format(struct rx_vector_1 *rx_vec_1)
{
    uint8_t format;

    #if (NX_MAC_VER >= 20)
    format = RXVEC_FORMATMOD(rx_vec_1->recvec1a);
    #else
    format = RXVEC_FORMATMOD(rx_vec_1->recvec1b);
    #endif // (NX_MAC_VER >= 20)

    return format;
}

/**
 *****************************************************************************************
 * @brief Retrieve the preamble type (short/long) of the RX vector
 *
 * @param[in] rx_vec_1 Receive Vector 1
 *
 * @return the preamble type.
 ****************************************************************************************
 */
__INLINE uint8_t hal_desc_get_preamble_type(struct rx_vector_1 *rx_vec_1)
{
    uint8_t pretype;

    #if (NX_MAC_VER >= 20)
    pretype = RXVEC_PRETYPE(rx_vec_1->recvec1a);
    #else
    pretype = RXVEC_PRETYPE(rx_vec_1->recvec1b);
    #endif // (NX_MAC_VER >= 20)

    return pretype;
}

/**
 *****************************************************************************************
 * @brief Retrieve Legacy Rate of the PPDU from the RX vector
 *
 * @param[in] rx_vec_1 Receive Vector 1
 *
 * @return the Legacy Rate value.
 ****************************************************************************************
 */
__INLINE uint8_t hal_desc_get_legacy_rate(struct rx_vector_1 *rx_vec_1)
{
    uint8_t rate;

    #if (NX_MAC_VER >= 20)
    rate = RXVEC_LEGRATE(rx_vec_1->recvec1b);
    #else
    rate = RXVEC_LEGRATE(rx_vec_1->recvec1a);
    #endif // (NX_MAC_VER >= 20)

    return rate;
}

/**
 *****************************************************************************************
 * @brief Retrieve antenna set from the RX vector
 *
 * @param[in] rx_vec_1 Receive Vector 1
 *
 * @return the antenna set value.
 ****************************************************************************************
 */
__INLINE uint8_t hal_desc_get_antenna_set(struct rx_vector_1 *rx_vec_1)
{
    uint8_t set;

    #if (NX_MAC_VER >= 20)
    set = RXVEC_LEGRATE(rx_vec_1->recvec1a);
    #else
    set = RXVEC_LEGRATE(rx_vec_1->recvec1c);
    #endif // (NX_MAC_VER >= 20)

    return set;
}

/// @}

#endif // _HWIF_DESC_H_
