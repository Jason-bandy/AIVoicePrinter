/**
 * @file reg_mac_crm.h
 * @brief Definitions of the CRM HW block registers and register access functions.
 *
 * @defgroup REG_MAC_CRM REG_MAC_CRM
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the CRM HW block registers and register access functions.
 */
#ifndef _REG_MAC_CRM_H_
#define _REG_MAC_CRM_H_

#include "co_int.h"
#include "_reg_mac_crm.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_MAC_CRM peripheral.
 */
#define REG_MAC_CRM_COUNT 4

/** @brief Decoding mask of the REG_MAC_CRM peripheral registers from the CPU point of view.
 */
#define REG_MAC_CRM_DECODING_MASK 0x0000000F

/**
 * @name CLKRST_CNTL register definitions
 * <table>
 * <caption id="CLKRST_CNTL_BF">CLKRST_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>12 <td>       WT_FREQ_SEL <td>W <td>R/W <td>0
 * <tr><td>09:08 <td>     CORE_FREQ_SEL <td>W <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the CLKRST_CNTL register
#define CRM_CLKRST_CNTL_ADDR   0xC0000000
/// Offset of the CLKRST_CNTL register from the base address
#define CRM_CLKRST_CNTL_OFFSET 0x00000000
/// Index of the CLKRST_CNTL register
#define CRM_CLKRST_CNTL_INDEX  0x00000000
/// Reset value of the CLKRST_CNTL register
#define CRM_CLKRST_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKRST_CNTL register.
 * The CLKRST_CNTL register will be read and its value returned.
 * @return The current value of the CLKRST_CNTL register.
 */
__INLINE uint32_t crm_clkrst_cntl_get(void)
{
    return REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
}

/**
 * @brief Sets the CLKRST_CNTL register to a value.
 * The CLKRST_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkrst_cntl_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, value);
}

// field definitions
/// WT_FREQ_SEL field bit
#define CRM_WT_FREQ_SEL_BIT      ((uint32_t)0x00001000)
/// WT_FREQ_SEL field position
#define CRM_WT_FREQ_SEL_POS      12
/// CORE_FREQ_SEL field mask
#define CRM_CORE_FREQ_SEL_MASK   ((uint32_t)0x00000300)
/// CORE_FREQ_SEL field LSB position
#define CRM_CORE_FREQ_SEL_LSB    8
/// CORE_FREQ_SEL field width
#define CRM_CORE_FREQ_SEL_WIDTH  ((uint32_t)0x00000002)

/// WT_FREQ_SEL field reset value
#define CRM_WT_FREQ_SEL_RST      0x0
/// CORE_FREQ_SEL field reset value
#define CRM_CORE_FREQ_SEL_RST    0x0

/**
 * @brief Constructs a value for the CLKRST_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] wtfreqsel - The value to use for the WT_FREQ_SEL field.
 * @param[in] corefreqsel - The value to use for the CORE_FREQ_SEL field.
 */
__INLINE void crm_clkrst_cntl_pack(uint8_t wtfreqsel, uint8_t corefreqsel)
{
    ASSERT_ERR((((uint32_t)wtfreqsel << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)corefreqsel << 8) & ~((uint32_t)0x00000300)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR,  ((uint32_t)wtfreqsel << 12) | ((uint32_t)corefreqsel << 8));
}

/**
 * @brief Unpacks CLKRST_CNTL's fields from current value of the CLKRST_CNTL register.
 *
 * Reads the CLKRST_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] wtfreqsel - Will be populated with the current value of this field from the register.
 * @param[out] corefreqsel - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkrst_cntl_unpack(uint8_t* wtfreqsel, uint8_t* corefreqsel)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);

    *wtfreqsel = (localVal & ((uint32_t)0x00001000)) >> 12;
    *corefreqsel = (localVal & ((uint32_t)0x00000300)) >> 8;
}

/**
 * @brief Returns the current value of the WT_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the WT_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the WT_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_wt_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the WT_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] wtfreqsel - The value to set the field to.
 */
__INLINE void crm_wt_freq_sel_setf(uint8_t wtfreqsel)
{
    ASSERT_ERR((((uint32_t)wtfreqsel << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)wtfreqsel << 12));
}

/**
 * @brief Returns the current value of the CORE_FREQ_SEL field in the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read and the CORE_FREQ_SEL field's value will be returned.
 *
 * @return The current value of the CORE_FREQ_SEL field in the CLKRST_CNTL register.
 */
__INLINE uint8_t crm_core_freq_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKRST_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

/**
 * @brief Sets the CORE_FREQ_SEL field of the CLKRST_CNTL register.
 *
 * The CLKRST_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] corefreqsel - The value to set the field to.
 */
__INLINE void crm_core_freq_sel_setf(uint8_t corefreqsel)
{
    ASSERT_ERR((((uint32_t)corefreqsel << 8) & ~((uint32_t)0x00000300)) == 0);
    REG_PL_WR(CRM_CLKRST_CNTL_ADDR, (REG_PL_RD(CRM_CLKRST_CNTL_ADDR) & ~((uint32_t)0x00000300)) | ((uint32_t)corefreqsel << 8));
}

/// @}

/**
 * @name CLKGATE_CNTL register definitions
 * <table>
 * <caption id="CLKGATE_CNTL_BF">CLKGATE_CNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>16 <td>mac_pi_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>15 <td>mac_pi_tx_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>14 <td>mac_pi_rx_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>13 <td>mac_core_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>12 <td>mac_crypt_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>11 <td>mac_core_tx_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>10 <td>mac_core_rx_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>09 <td>mac_wt_clk_gating_en <td>R <td>R/W <td>0
 * <tr><td>08 <td>mpif_clk_gating_en <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the CLKGATE_CNTL register
#define CRM_CLKGATE_CNTL_ADDR   0xC0000004
/// Offset of the CLKGATE_CNTL register from the base address
#define CRM_CLKGATE_CNTL_OFFSET 0x00000004
/// Index of the CLKGATE_CNTL register
#define CRM_CLKGATE_CNTL_INDEX  0x00000001
/// Reset value of the CLKGATE_CNTL register
#define CRM_CLKGATE_CNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKGATE_CNTL register.
 * The CLKGATE_CNTL register will be read and its value returned.
 * @return The current value of the CLKGATE_CNTL register.
 */
__INLINE uint32_t crm_clkgate_cntl_get(void)
{
    return REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
}

/**
 * @brief Sets the CLKGATE_CNTL register to a value.
 * The CLKGATE_CNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkgate_cntl_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, value);
}

// field definitions
/// MAC_PI_CLK_GATING_EN field bit
#define CRM_MAC_PI_CLK_GATING_EN_BIT         ((uint32_t)0x00010000)
/// MAC_PI_CLK_GATING_EN field position
#define CRM_MAC_PI_CLK_GATING_EN_POS         16
/// MAC_PI_TX_CLK_GATING_EN field bit
#define CRM_MAC_PI_TX_CLK_GATING_EN_BIT      ((uint32_t)0x00008000)
/// MAC_PI_TX_CLK_GATING_EN field position
#define CRM_MAC_PI_TX_CLK_GATING_EN_POS      15
/// MAC_PI_RX_CLK_GATING_EN field bit
#define CRM_MAC_PI_RX_CLK_GATING_EN_BIT      ((uint32_t)0x00004000)
/// MAC_PI_RX_CLK_GATING_EN field position
#define CRM_MAC_PI_RX_CLK_GATING_EN_POS      14
/// MAC_CORE_CLK_GATING_EN field bit
#define CRM_MAC_CORE_CLK_GATING_EN_BIT       ((uint32_t)0x00002000)
/// MAC_CORE_CLK_GATING_EN field position
#define CRM_MAC_CORE_CLK_GATING_EN_POS       13
/// MAC_CRYPT_CLK_GATING_EN field bit
#define CRM_MAC_CRYPT_CLK_GATING_EN_BIT      ((uint32_t)0x00001000)
/// MAC_CRYPT_CLK_GATING_EN field position
#define CRM_MAC_CRYPT_CLK_GATING_EN_POS      12
/// MAC_CORE_TX_CLK_GATING_EN field bit
#define CRM_MAC_CORE_TX_CLK_GATING_EN_BIT    ((uint32_t)0x00000800)
/// MAC_CORE_TX_CLK_GATING_EN field position
#define CRM_MAC_CORE_TX_CLK_GATING_EN_POS    11
/// MAC_CORE_RX_CLK_GATING_EN field bit
#define CRM_MAC_CORE_RX_CLK_GATING_EN_BIT    ((uint32_t)0x00000400)
/// MAC_CORE_RX_CLK_GATING_EN field position
#define CRM_MAC_CORE_RX_CLK_GATING_EN_POS    10
/// MAC_WT_CLK_GATING_EN field bit
#define CRM_MAC_WT_CLK_GATING_EN_BIT         ((uint32_t)0x00000200)
/// MAC_WT_CLK_GATING_EN field position
#define CRM_MAC_WT_CLK_GATING_EN_POS         9
/// MPIF_CLK_GATING_EN field bit
#define CRM_MPIF_CLK_GATING_EN_BIT           ((uint32_t)0x00000100)
/// MPIF_CLK_GATING_EN field position
#define CRM_MPIF_CLK_GATING_EN_POS           8

/// MAC_PI_CLK_GATING_EN field reset value
#define CRM_MAC_PI_CLK_GATING_EN_RST         0x0
/// MAC_PI_TX_CLK_GATING_EN field reset value
#define CRM_MAC_PI_TX_CLK_GATING_EN_RST      0x0
/// MAC_PI_RX_CLK_GATING_EN field reset value
#define CRM_MAC_PI_RX_CLK_GATING_EN_RST      0x0
/// MAC_CORE_CLK_GATING_EN field reset value
#define CRM_MAC_CORE_CLK_GATING_EN_RST       0x0
/// MAC_CRYPT_CLK_GATING_EN field reset value
#define CRM_MAC_CRYPT_CLK_GATING_EN_RST      0x0
/// MAC_CORE_TX_CLK_GATING_EN field reset value
#define CRM_MAC_CORE_TX_CLK_GATING_EN_RST    0x0
/// MAC_CORE_RX_CLK_GATING_EN field reset value
#define CRM_MAC_CORE_RX_CLK_GATING_EN_RST    0x0
/// MAC_WT_CLK_GATING_EN field reset value
#define CRM_MAC_WT_CLK_GATING_EN_RST         0x0
/// MPIF_CLK_GATING_EN field reset value
#define CRM_MPIF_CLK_GATING_EN_RST           0x0

/**
 * @brief Constructs a value for the CLKGATE_CNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] macpiclkgatingen - The value to use for the mac_pi_clk_gating_en field.
 * @param[in] macpitxclkgatingen - The value to use for the mac_pi_tx_clk_gating_en field.
 * @param[in] macpirxclkgatingen - The value to use for the mac_pi_rx_clk_gating_en field.
 * @param[in] maccoreclkgatingen - The value to use for the mac_core_clk_gating_en field.
 * @param[in] maccryptclkgatingen - The value to use for the mac_crypt_clk_gating_en field.
 * @param[in] maccoretxclkgatingen - The value to use for the mac_core_tx_clk_gating_en field.
 * @param[in] maccorerxclkgatingen - The value to use for the mac_core_rx_clk_gating_en field.
 * @param[in] macwtclkgatingen - The value to use for the mac_wt_clk_gating_en field.
 * @param[in] mpifclkgatingen - The value to use for the mpif_clk_gating_en field.
 */
__INLINE void crm_clkgate_cntl_pack(uint8_t macpiclkgatingen, uint8_t macpitxclkgatingen, uint8_t macpirxclkgatingen, uint8_t maccoreclkgatingen, uint8_t maccryptclkgatingen, uint8_t maccoretxclkgatingen, uint8_t maccorerxclkgatingen, uint8_t macwtclkgatingen, uint8_t mpifclkgatingen)
{
    ASSERT_ERR((((uint32_t)macpiclkgatingen << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)macpitxclkgatingen << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)macpirxclkgatingen << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)maccoreclkgatingen << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)maccryptclkgatingen << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)maccoretxclkgatingen << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)maccorerxclkgatingen << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)macwtclkgatingen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)mpifclkgatingen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR,  ((uint32_t)macpiclkgatingen << 16) | ((uint32_t)macpitxclkgatingen << 15) | ((uint32_t)macpirxclkgatingen << 14) | ((uint32_t)maccoreclkgatingen << 13) | ((uint32_t)maccryptclkgatingen << 12) | ((uint32_t)maccoretxclkgatingen << 11) | ((uint32_t)maccorerxclkgatingen << 10) | ((uint32_t)macwtclkgatingen << 9) | ((uint32_t)mpifclkgatingen << 8));
}

/**
 * @brief Unpacks CLKGATE_CNTL's fields from current value of the CLKGATE_CNTL register.
 *
 * Reads the CLKGATE_CNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] macpiclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] macpitxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] macpirxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccoreclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccryptclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccoretxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] maccorerxclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] macwtclkgatingen - Will be populated with the current value of this field from the register.
 * @param[out] mpifclkgatingen - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkgate_cntl_unpack(uint8_t* macpiclkgatingen, uint8_t* macpitxclkgatingen, uint8_t* macpirxclkgatingen, uint8_t* maccoreclkgatingen, uint8_t* maccryptclkgatingen, uint8_t* maccoretxclkgatingen, uint8_t* maccorerxclkgatingen, uint8_t* macwtclkgatingen, uint8_t* mpifclkgatingen)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);

    *macpiclkgatingen = (localVal & ((uint32_t)0x00010000)) >> 16;
    *macpitxclkgatingen = (localVal & ((uint32_t)0x00008000)) >> 15;
    *macpirxclkgatingen = (localVal & ((uint32_t)0x00004000)) >> 14;
    *maccoreclkgatingen = (localVal & ((uint32_t)0x00002000)) >> 13;
    *maccryptclkgatingen = (localVal & ((uint32_t)0x00001000)) >> 12;
    *maccoretxclkgatingen = (localVal & ((uint32_t)0x00000800)) >> 11;
    *maccorerxclkgatingen = (localVal & ((uint32_t)0x00000400)) >> 10;
    *macwtclkgatingen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *mpifclkgatingen = (localVal & ((uint32_t)0x00000100)) >> 8;
}

/**
 * @brief Returns the current value of the mac_pi_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_pi_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_pi_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_pi_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the mac_pi_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macpiclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_pi_clk_gating_en_setf(uint8_t macpiclkgatingen)
{
    ASSERT_ERR((((uint32_t)macpiclkgatingen << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)macpiclkgatingen << 16));
}

/**
 * @brief Returns the current value of the mac_pi_tx_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_pi_tx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_pi_tx_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_pi_tx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

/**
 * @brief Sets the mac_pi_tx_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macpitxclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_pi_tx_clk_gating_en_setf(uint8_t macpitxclkgatingen)
{
    ASSERT_ERR((((uint32_t)macpitxclkgatingen << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)macpitxclkgatingen << 15));
}

/**
 * @brief Returns the current value of the mac_pi_rx_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_pi_rx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_pi_rx_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_pi_rx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

/**
 * @brief Sets the mac_pi_rx_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macpirxclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_pi_rx_clk_gating_en_setf(uint8_t macpirxclkgatingen)
{
    ASSERT_ERR((((uint32_t)macpirxclkgatingen << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)macpirxclkgatingen << 14));
}

/**
 * @brief Returns the current value of the mac_core_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_core_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_core_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_core_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

/**
 * @brief Sets the mac_core_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccoreclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_core_clk_gating_en_setf(uint8_t maccoreclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccoreclkgatingen << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)maccoreclkgatingen << 13));
}

/**
 * @brief Returns the current value of the mac_crypt_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_crypt_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_crypt_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_crypt_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the mac_crypt_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccryptclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_crypt_clk_gating_en_setf(uint8_t maccryptclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccryptclkgatingen << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)maccryptclkgatingen << 12));
}

/**
 * @brief Returns the current value of the mac_core_tx_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_core_tx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_core_tx_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_core_tx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the mac_core_tx_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccoretxclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_core_tx_clk_gating_en_setf(uint8_t maccoretxclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccoretxclkgatingen << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)maccoretxclkgatingen << 11));
}

/**
 * @brief Returns the current value of the mac_core_rx_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_core_rx_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_core_rx_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_core_rx_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the mac_core_rx_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maccorerxclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_core_rx_clk_gating_en_setf(uint8_t maccorerxclkgatingen)
{
    ASSERT_ERR((((uint32_t)maccorerxclkgatingen << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)maccorerxclkgatingen << 10));
}

/**
 * @brief Returns the current value of the mac_wt_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mac_wt_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mac_wt_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mac_wt_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the mac_wt_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macwtclkgatingen - The value to set the field to.
 */
__INLINE void crm_mac_wt_clk_gating_en_setf(uint8_t macwtclkgatingen)
{
    ASSERT_ERR((((uint32_t)macwtclkgatingen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)macwtclkgatingen << 9));
}

/**
 * @brief Returns the current value of the mpif_clk_gating_en field in the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read and the mpif_clk_gating_en field's value will be returned.
 *
 * @return The current value of the mpif_clk_gating_en field in the CLKGATE_CNTL register.
 */
__INLINE uint8_t crm_mpif_clk_gating_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATE_CNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the mpif_clk_gating_en field of the CLKGATE_CNTL register.
 *
 * The CLKGATE_CNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mpifclkgatingen - The value to set the field to.
 */
__INLINE void crm_mpif_clk_gating_en_setf(uint8_t mpifclkgatingen)
{
    ASSERT_ERR((((uint32_t)mpifclkgatingen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(CRM_CLKGATE_CNTL_ADDR, (REG_PL_RD(CRM_CLKGATE_CNTL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)mpifclkgatingen << 8));
}

/// @}

/**
 * @name MAC_DIAGSEL register definitions
 * <table>
 * <caption id="MAC_DIAGSEL_BF">MAC_DIAGSEL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>17:16 <td>   mac_diag_clksel <td>R <td>R/W <td>0x0
 * <tr><td>15:08 <td>     mac_diagsel_h <td>R <td>R/W <td>0x0
 * <tr><td>07:00 <td>     mac_diagsel_l <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the MAC_DIAGSEL register
#define CRM_MAC_DIAGSEL_ADDR   0xC0000008
/// Offset of the MAC_DIAGSEL register from the base address
#define CRM_MAC_DIAGSEL_OFFSET 0x00000008
/// Index of the MAC_DIAGSEL register
#define CRM_MAC_DIAGSEL_INDEX  0x00000002
/// Reset value of the MAC_DIAGSEL register
#define CRM_MAC_DIAGSEL_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_DIAGSEL register.
 * The MAC_DIAGSEL register will be read and its value returned.
 * @return The current value of the MAC_DIAGSEL register.
 */
__INLINE uint32_t crm_mac_diagsel_get(void)
{
    return REG_PL_RD(CRM_MAC_DIAGSEL_ADDR);
}

/**
 * @brief Sets the MAC_DIAGSEL register to a value.
 * The MAC_DIAGSEL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_mac_diagsel_set(uint32_t value)
{
    REG_PL_WR(CRM_MAC_DIAGSEL_ADDR, value);
}

// field definitions
/// MAC_DIAG_CLKSEL field mask
#define CRM_MAC_DIAG_CLKSEL_MASK   ((uint32_t)0x00030000)
/// MAC_DIAG_CLKSEL field LSB position
#define CRM_MAC_DIAG_CLKSEL_LSB    16
/// MAC_DIAG_CLKSEL field width
#define CRM_MAC_DIAG_CLKSEL_WIDTH  ((uint32_t)0x00000002)
/// MAC_DIAGSEL_H field mask
#define CRM_MAC_DIAGSEL_H_MASK     ((uint32_t)0x0000FF00)
/// MAC_DIAGSEL_H field LSB position
#define CRM_MAC_DIAGSEL_H_LSB      8
/// MAC_DIAGSEL_H field width
#define CRM_MAC_DIAGSEL_H_WIDTH    ((uint32_t)0x00000008)
/// MAC_DIAGSEL_L field mask
#define CRM_MAC_DIAGSEL_L_MASK     ((uint32_t)0x000000FF)
/// MAC_DIAGSEL_L field LSB position
#define CRM_MAC_DIAGSEL_L_LSB      0
/// MAC_DIAGSEL_L field width
#define CRM_MAC_DIAGSEL_L_WIDTH    ((uint32_t)0x00000008)

/// MAC_DIAG_CLKSEL field reset value
#define CRM_MAC_DIAG_CLKSEL_RST    0x0
/// MAC_DIAGSEL_H field reset value
#define CRM_MAC_DIAGSEL_H_RST      0x0
/// MAC_DIAGSEL_L field reset value
#define CRM_MAC_DIAGSEL_L_RST      0x0

/**
 * @brief Constructs a value for the MAC_DIAGSEL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] macdiagclksel - The value to use for the mac_diag_clksel field.
 * @param[in] macdiagselh - The value to use for the mac_diagsel_h field.
 * @param[in] macdiagsell - The value to use for the mac_diagsel_l field.
 */
__INLINE void crm_mac_diagsel_pack(uint8_t macdiagclksel, uint8_t macdiagselh, uint8_t macdiagsell)
{
    ASSERT_ERR((((uint32_t)macdiagclksel << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)macdiagselh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)macdiagsell << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(CRM_MAC_DIAGSEL_ADDR,  ((uint32_t)macdiagclksel << 16) | ((uint32_t)macdiagselh << 8) | ((uint32_t)macdiagsell << 0));
}

/**
 * @brief Unpacks MAC_DIAGSEL's fields from current value of the MAC_DIAGSEL register.
 *
 * Reads the MAC_DIAGSEL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] macdiagclksel - Will be populated with the current value of this field from the register.
 * @param[out] macdiagselh - Will be populated with the current value of this field from the register.
 * @param[out] macdiagsell - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_mac_diagsel_unpack(uint8_t* macdiagclksel, uint8_t* macdiagselh, uint8_t* macdiagsell)
{
    uint32_t localVal = REG_PL_RD(CRM_MAC_DIAGSEL_ADDR);

    *macdiagclksel = (localVal & ((uint32_t)0x00030000)) >> 16;
    *macdiagselh = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *macdiagsell = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the mac_diag_clksel field in the MAC_DIAGSEL register.
 *
 * The MAC_DIAGSEL register will be read and the mac_diag_clksel field's value will be returned.
 *
 * @return The current value of the mac_diag_clksel field in the MAC_DIAGSEL register.
 */
__INLINE uint8_t crm_mac_diag_clksel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_MAC_DIAGSEL_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the mac_diag_clksel field of the MAC_DIAGSEL register.
 *
 * The MAC_DIAGSEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macdiagclksel - The value to set the field to.
 */
__INLINE void crm_mac_diag_clksel_setf(uint8_t macdiagclksel)
{
    ASSERT_ERR((((uint32_t)macdiagclksel << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(CRM_MAC_DIAGSEL_ADDR, (REG_PL_RD(CRM_MAC_DIAGSEL_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)macdiagclksel << 16));
}

/**
 * @brief Returns the current value of the mac_diagsel_h field in the MAC_DIAGSEL register.
 *
 * The MAC_DIAGSEL register will be read and the mac_diagsel_h field's value will be returned.
 *
 * @return The current value of the mac_diagsel_h field in the MAC_DIAGSEL register.
 */
__INLINE uint8_t crm_mac_diagsel_h_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_MAC_DIAGSEL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the mac_diagsel_h field of the MAC_DIAGSEL register.
 *
 * The MAC_DIAGSEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macdiagselh - The value to set the field to.
 */
__INLINE void crm_mac_diagsel_h_setf(uint8_t macdiagselh)
{
    ASSERT_ERR((((uint32_t)macdiagselh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(CRM_MAC_DIAGSEL_ADDR, (REG_PL_RD(CRM_MAC_DIAGSEL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)macdiagselh << 8));
}

/**
 * @brief Returns the current value of the mac_diagsel_l field in the MAC_DIAGSEL register.
 *
 * The MAC_DIAGSEL register will be read and the mac_diagsel_l field's value will be returned.
 *
 * @return The current value of the mac_diagsel_l field in the MAC_DIAGSEL register.
 */
__INLINE uint8_t crm_mac_diagsel_l_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_MAC_DIAGSEL_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the mac_diagsel_l field of the MAC_DIAGSEL register.
 *
 * The MAC_DIAGSEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] macdiagsell - The value to set the field to.
 */
__INLINE void crm_mac_diagsel_l_setf(uint8_t macdiagsell)
{
    ASSERT_ERR((((uint32_t)macdiagsell << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(CRM_MAC_DIAGSEL_ADDR, (REG_PL_RD(CRM_MAC_DIAGSEL_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)macdiagsell << 0));
}

/// @}

/**
 * @name MAC_DIAGVAL register definitions
 * <table>
 * <caption id="MAC_DIAGVAL_BF">MAC_DIAGVAL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:16 <td>     mac_diagval_h <td>R <td>R <td>0x0
 * <tr><td>15:00 <td>     mac_diagval_l <td>R <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the MAC_DIAGVAL register
#define CRM_MAC_DIAGVAL_ADDR   0xC000000C
/// Offset of the MAC_DIAGVAL register from the base address
#define CRM_MAC_DIAGVAL_OFFSET 0x0000000C
/// Index of the MAC_DIAGVAL register
#define CRM_MAC_DIAGVAL_INDEX  0x00000003
/// Reset value of the MAC_DIAGVAL register
#define CRM_MAC_DIAGVAL_RESET  0x00000000

/**
 * @brief Returns the current value of the MAC_DIAGVAL register.
 * The MAC_DIAGVAL register will be read and its value returned.
 * @return The current value of the MAC_DIAGVAL register.
 */
__INLINE uint32_t crm_mac_diagval_get(void)
{
    return REG_PL_RD(CRM_MAC_DIAGVAL_ADDR);
}

/**
 * @brief Sets the MAC_DIAGVAL register to a value.
 * The MAC_DIAGVAL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_mac_diagval_set(uint32_t value)
{
    REG_PL_WR(CRM_MAC_DIAGVAL_ADDR, value);
}

// field definitions
/// MAC_DIAGVAL_H field mask
#define CRM_MAC_DIAGVAL_H_MASK   ((uint32_t)0xFFFF0000)
/// MAC_DIAGVAL_H field LSB position
#define CRM_MAC_DIAGVAL_H_LSB    16
/// MAC_DIAGVAL_H field width
#define CRM_MAC_DIAGVAL_H_WIDTH  ((uint32_t)0x00000010)
/// MAC_DIAGVAL_L field mask
#define CRM_MAC_DIAGVAL_L_MASK   ((uint32_t)0x0000FFFF)
/// MAC_DIAGVAL_L field LSB position
#define CRM_MAC_DIAGVAL_L_LSB    0
/// MAC_DIAGVAL_L field width
#define CRM_MAC_DIAGVAL_L_WIDTH  ((uint32_t)0x00000010)

/// MAC_DIAGVAL_H field reset value
#define CRM_MAC_DIAGVAL_H_RST    0x0
/// MAC_DIAGVAL_L field reset value
#define CRM_MAC_DIAGVAL_L_RST    0x0

/**
 * @brief Unpacks MAC_DIAGVAL's fields from current value of the MAC_DIAGVAL register.
 *
 * Reads the MAC_DIAGVAL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] macdiagvalh - Will be populated with the current value of this field from the register.
 * @param[out] macdiagvall - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_mac_diagval_unpack(uint16_t* macdiagvalh, uint16_t* macdiagvall)
{
    uint32_t localVal = REG_PL_RD(CRM_MAC_DIAGVAL_ADDR);

    *macdiagvalh = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *macdiagvall = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the mac_diagval_h field in the MAC_DIAGVAL register.
 *
 * The MAC_DIAGVAL register will be read and the mac_diagval_h field's value will be returned.
 *
 * @return The current value of the mac_diagval_h field in the MAC_DIAGVAL register.
 */
__INLINE uint16_t crm_mac_diagval_h_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_MAC_DIAGVAL_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the mac_diagval_l field in the MAC_DIAGVAL register.
 *
 * The MAC_DIAGVAL register will be read and the mac_diagval_l field's value will be returned.
 *
 * @return The current value of the mac_diagval_l field in the MAC_DIAGVAL register.
 */
__INLINE uint16_t crm_mac_diagval_l_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_MAC_DIAGVAL_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}


#endif // _REG_MAC_CRM_H_

/// @}

