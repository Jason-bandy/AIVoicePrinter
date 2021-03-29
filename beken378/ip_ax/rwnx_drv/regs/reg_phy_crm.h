/**
 * @file reg_phy_crm.h
 * @brief Definitions of the CRM HW block registers and register access functions.
 *
 * @defgroup REG_PHY_CRM REG_PHY_CRM
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the CRM HW block registers and register access functions.
 */
#ifndef _REG_PHY_CRM_H_
#define _REG_PHY_CRM_H_

#include "co_int.h"
#include "_reg_phy_crm.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_PHY_CRM peripheral.
 */
#define REG_PHY_CRM_COUNT 6

/** @brief Decoding mask of the REG_PHY_CRM peripheral registers from the CPU point of view.
 */
#define REG_PHY_CRM_DECODING_MASK 0x0000001F

/**
 * @name RSTCTRL register definitions
 * <table>
 * <caption id="RSTCTRL_BF">RSTCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>08 <td>         RCSWRESET <td>R <td>R/W <td>0
 * <tr><td>04 <td>        AGCSWRESET <td>R <td>R/W <td>0
 * <tr><td>00 <td>        PHYSWRESET <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the RSTCTRL register
#define CRM_RSTCTRL_ADDR   0x01050000
/// Offset of the RSTCTRL register from the base address
#define CRM_RSTCTRL_OFFSET 0x00000000
/// Index of the RSTCTRL register
#define CRM_RSTCTRL_INDEX  0x00000000
/// Reset value of the RSTCTRL register
#define CRM_RSTCTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the RSTCTRL register.
 * The RSTCTRL register will be read and its value returned.
 * @return The current value of the RSTCTRL register.
 */
__INLINE uint32_t crm_rstctrl_get(void)
{
    return REG_PL_RD(CRM_RSTCTRL_ADDR);
}

/**
 * @brief Sets the RSTCTRL register to a value.
 * The RSTCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_rstctrl_set(uint32_t value)
{
    REG_PL_WR(CRM_RSTCTRL_ADDR, value);
}

// field definitions
/// RCSWRESET field bit
#define CRM_RCSWRESET_BIT     ((uint32_t)0x00000100)
/// RCSWRESET field position
#define CRM_RCSWRESET_POS     8
/// AGCSWRESET field bit
#define CRM_AGCSWRESET_BIT    ((uint32_t)0x00000010)
/// AGCSWRESET field position
#define CRM_AGCSWRESET_POS    4
/// PHYSWRESET field bit
#define CRM_PHYSWRESET_BIT    ((uint32_t)0x00000001)
/// PHYSWRESET field position
#define CRM_PHYSWRESET_POS    0

/// RCSWRESET field reset value
#define CRM_RCSWRESET_RST     0x0
/// AGCSWRESET field reset value
#define CRM_AGCSWRESET_RST    0x0
/// PHYSWRESET field reset value
#define CRM_PHYSWRESET_RST    0x0

/**
 * @brief Constructs a value for the RSTCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rcswreset - The value to use for the RCSWRESET field.
 * @param[in] agcswreset - The value to use for the AGCSWRESET field.
 * @param[in] physwreset - The value to use for the PHYSWRESET field.
 */
__INLINE void crm_rstctrl_pack(uint8_t rcswreset, uint8_t agcswreset, uint8_t physwreset)
{
    ASSERT_ERR((((uint32_t)rcswreset << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)agcswreset << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)physwreset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR,  ((uint32_t)rcswreset << 8) | ((uint32_t)agcswreset << 4) | ((uint32_t)physwreset << 0));
}

/**
 * @brief Unpacks RSTCTRL's fields from current value of the RSTCTRL register.
 *
 * Reads the RSTCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rcswreset - Will be populated with the current value of this field from the register.
 * @param[out] agcswreset - Will be populated with the current value of this field from the register.
 * @param[out] physwreset - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_rstctrl_unpack(uint8_t* rcswreset, uint8_t* agcswreset, uint8_t* physwreset)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);

    *rcswreset = (localVal & ((uint32_t)0x00000100)) >> 8;
    *agcswreset = (localVal & ((uint32_t)0x00000010)) >> 4;
    *physwreset = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RCSWRESET field in the RSTCTRL register.
 *
 * The RSTCTRL register will be read and the RCSWRESET field's value will be returned.
 *
 * @return The current value of the RCSWRESET field in the RSTCTRL register.
 */
__INLINE uint8_t crm_rcswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the RCSWRESET field of the RSTCTRL register.
 *
 * The RSTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rcswreset - The value to set the field to.
 */
__INLINE void crm_rcswreset_setf(uint8_t rcswreset)
{
    ASSERT_ERR((((uint32_t)rcswreset << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR, (REG_PL_RD(CRM_RSTCTRL_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)rcswreset << 8));
}

/**
 * @brief Returns the current value of the AGCSWRESET field in the RSTCTRL register.
 *
 * The RSTCTRL register will be read and the AGCSWRESET field's value will be returned.
 *
 * @return The current value of the AGCSWRESET field in the RSTCTRL register.
 */
__INLINE uint8_t crm_agcswreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the AGCSWRESET field of the RSTCTRL register.
 *
 * The RSTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcswreset - The value to set the field to.
 */
__INLINE void crm_agcswreset_setf(uint8_t agcswreset)
{
    ASSERT_ERR((((uint32_t)agcswreset << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR, (REG_PL_RD(CRM_RSTCTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)agcswreset << 4));
}

/**
 * @brief Returns the current value of the PHYSWRESET field in the RSTCTRL register.
 *
 * The RSTCTRL register will be read and the PHYSWRESET field's value will be returned.
 *
 * @return The current value of the PHYSWRESET field in the RSTCTRL register.
 */
__INLINE uint8_t crm_physwreset_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_RSTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the PHYSWRESET field of the RSTCTRL register.
 *
 * The RSTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] physwreset - The value to set the field to.
 */
__INLINE void crm_physwreset_setf(uint8_t physwreset)
{
    ASSERT_ERR((((uint32_t)physwreset << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_RSTCTRL_ADDR, (REG_PL_RD(CRM_RSTCTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)physwreset << 0));
}

/// @}

/**
 * @name CLKGATEPHYFCTRL0 register definitions
 * <table>
 * <caption id="CLKGATEPHYFCTRL0_BF">CLKGATEPHYFCTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>     PHYTXCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>30 <td>      BDTXCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>29 <td>    AGCMEMCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>28 <td>       AGCCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>27 <td>        RCCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>24 <td>        FECLKFORCE <td>R <td>R/W <td>0
 * <tr><td>23 <td>       FDOCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>22 <td>       EQUCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>18 <td>    TDCOMPCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>17 <td>   TDFOESTCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>16 <td>       TBECLKFORCE <td>R <td>R/W <td>0
 * <tr><td>10 <td>   FFT1MEMCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>09 <td>      FFT1CLKFORCE <td>R <td>R/W <td>0
 * <tr><td>08 <td>   FFT0MEMCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>07 <td>      FFT0CLKFORCE <td>R <td>R/W <td>0
 * <tr><td>06 <td>     CHESTCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>04 <td>      HMEMCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>02 <td>      VTB1CLKFORCE <td>R <td>R/W <td>0
 * <tr><td>01 <td>      VTB0CLKFORCE <td>R <td>R/W <td>0
 * <tr><td>00 <td>      BDRXCLKFORCE <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the CLKGATEPHYFCTRL0 register
#define CRM_CLKGATEPHYFCTRL0_ADDR   0x01050004
/// Offset of the CLKGATEPHYFCTRL0 register from the base address
#define CRM_CLKGATEPHYFCTRL0_OFFSET 0x00000004
/// Index of the CLKGATEPHYFCTRL0 register
#define CRM_CLKGATEPHYFCTRL0_INDEX  0x00000001
/// Reset value of the CLKGATEPHYFCTRL0 register
#define CRM_CLKGATEPHYFCTRL0_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKGATEPHYFCTRL0 register.
 * The CLKGATEPHYFCTRL0 register will be read and its value returned.
 * @return The current value of the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint32_t crm_clkgatephyfctrl0_get(void)
{
    return REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
}

/**
 * @brief Sets the CLKGATEPHYFCTRL0 register to a value.
 * The CLKGATEPHYFCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkgatephyfctrl0_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, value);
}

// field definitions
/// PHYTXCLKFORCE field bit
#define CRM_PHYTXCLKFORCE_BIT      ((uint32_t)0x80000000)
/// PHYTXCLKFORCE field position
#define CRM_PHYTXCLKFORCE_POS      31
/// BDTXCLKFORCE field bit
#define CRM_BDTXCLKFORCE_BIT       ((uint32_t)0x40000000)
/// BDTXCLKFORCE field position
#define CRM_BDTXCLKFORCE_POS       30
/// AGCMEMCLKFORCE field bit
#define CRM_AGCMEMCLKFORCE_BIT     ((uint32_t)0x20000000)
/// AGCMEMCLKFORCE field position
#define CRM_AGCMEMCLKFORCE_POS     29
/// AGCCLKFORCE field bit
#define CRM_AGCCLKFORCE_BIT        ((uint32_t)0x10000000)
/// AGCCLKFORCE field position
#define CRM_AGCCLKFORCE_POS        28
/// RCCLKFORCE field bit
#define CRM_RCCLKFORCE_BIT         ((uint32_t)0x08000000)
/// RCCLKFORCE field position
#define CRM_RCCLKFORCE_POS         27
/// FECLKFORCE field bit
#define CRM_FECLKFORCE_BIT         ((uint32_t)0x01000000)
/// FECLKFORCE field position
#define CRM_FECLKFORCE_POS         24
/// FDOCLKFORCE field bit
#define CRM_FDOCLKFORCE_BIT        ((uint32_t)0x00800000)
/// FDOCLKFORCE field position
#define CRM_FDOCLKFORCE_POS        23
/// EQUCLKFORCE field bit
#define CRM_EQUCLKFORCE_BIT        ((uint32_t)0x00400000)
/// EQUCLKFORCE field position
#define CRM_EQUCLKFORCE_POS        22
/// TDCOMPCLKFORCE field bit
#define CRM_TDCOMPCLKFORCE_BIT     ((uint32_t)0x00040000)
/// TDCOMPCLKFORCE field position
#define CRM_TDCOMPCLKFORCE_POS     18
/// TDFOESTCLKFORCE field bit
#define CRM_TDFOESTCLKFORCE_BIT    ((uint32_t)0x00020000)
/// TDFOESTCLKFORCE field position
#define CRM_TDFOESTCLKFORCE_POS    17
/// TBECLKFORCE field bit
#define CRM_TBECLKFORCE_BIT        ((uint32_t)0x00010000)
/// TBECLKFORCE field position
#define CRM_TBECLKFORCE_POS        16
/// FFT1MEMCLKFORCE field bit
#define CRM_FFT1MEMCLKFORCE_BIT    ((uint32_t)0x00000400)
/// FFT1MEMCLKFORCE field position
#define CRM_FFT1MEMCLKFORCE_POS    10
/// FFT1CLKFORCE field bit
#define CRM_FFT1CLKFORCE_BIT       ((uint32_t)0x00000200)
/// FFT1CLKFORCE field position
#define CRM_FFT1CLKFORCE_POS       9
/// FFT0MEMCLKFORCE field bit
#define CRM_FFT0MEMCLKFORCE_BIT    ((uint32_t)0x00000100)
/// FFT0MEMCLKFORCE field position
#define CRM_FFT0MEMCLKFORCE_POS    8
/// FFT0CLKFORCE field bit
#define CRM_FFT0CLKFORCE_BIT       ((uint32_t)0x00000080)
/// FFT0CLKFORCE field position
#define CRM_FFT0CLKFORCE_POS       7
/// CHESTCLKFORCE field bit
#define CRM_CHESTCLKFORCE_BIT      ((uint32_t)0x00000040)
/// CHESTCLKFORCE field position
#define CRM_CHESTCLKFORCE_POS      6
/// HMEMCLKFORCE field bit
#define CRM_HMEMCLKFORCE_BIT       ((uint32_t)0x00000010)
/// HMEMCLKFORCE field position
#define CRM_HMEMCLKFORCE_POS       4
/// VTB1CLKFORCE field bit
#define CRM_VTB1CLKFORCE_BIT       ((uint32_t)0x00000004)
/// VTB1CLKFORCE field position
#define CRM_VTB1CLKFORCE_POS       2
/// VTB0CLKFORCE field bit
#define CRM_VTB0CLKFORCE_BIT       ((uint32_t)0x00000002)
/// VTB0CLKFORCE field position
#define CRM_VTB0CLKFORCE_POS       1
/// BDRXCLKFORCE field bit
#define CRM_BDRXCLKFORCE_BIT       ((uint32_t)0x00000001)
/// BDRXCLKFORCE field position
#define CRM_BDRXCLKFORCE_POS       0

/// PHYTXCLKFORCE field reset value
#define CRM_PHYTXCLKFORCE_RST      0x0
/// BDTXCLKFORCE field reset value
#define CRM_BDTXCLKFORCE_RST       0x0
/// AGCMEMCLKFORCE field reset value
#define CRM_AGCMEMCLKFORCE_RST     0x0
/// AGCCLKFORCE field reset value
#define CRM_AGCCLKFORCE_RST        0x0
/// RCCLKFORCE field reset value
#define CRM_RCCLKFORCE_RST         0x0
/// FECLKFORCE field reset value
#define CRM_FECLKFORCE_RST         0x0
/// FDOCLKFORCE field reset value
#define CRM_FDOCLKFORCE_RST        0x0
/// EQUCLKFORCE field reset value
#define CRM_EQUCLKFORCE_RST        0x0
/// TDCOMPCLKFORCE field reset value
#define CRM_TDCOMPCLKFORCE_RST     0x0
/// TDFOESTCLKFORCE field reset value
#define CRM_TDFOESTCLKFORCE_RST    0x0
/// TBECLKFORCE field reset value
#define CRM_TBECLKFORCE_RST        0x0
/// FFT1MEMCLKFORCE field reset value
#define CRM_FFT1MEMCLKFORCE_RST    0x0
/// FFT1CLKFORCE field reset value
#define CRM_FFT1CLKFORCE_RST       0x0
/// FFT0MEMCLKFORCE field reset value
#define CRM_FFT0MEMCLKFORCE_RST    0x0
/// FFT0CLKFORCE field reset value
#define CRM_FFT0CLKFORCE_RST       0x0
/// CHESTCLKFORCE field reset value
#define CRM_CHESTCLKFORCE_RST      0x0
/// HMEMCLKFORCE field reset value
#define CRM_HMEMCLKFORCE_RST       0x0
/// VTB1CLKFORCE field reset value
#define CRM_VTB1CLKFORCE_RST       0x0
/// VTB0CLKFORCE field reset value
#define CRM_VTB0CLKFORCE_RST       0x0
/// BDRXCLKFORCE field reset value
#define CRM_BDRXCLKFORCE_RST       0x0

/**
 * @brief Constructs a value for the CLKGATEPHYFCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] phytxclkforce - The value to use for the PHYTXCLKFORCE field.
 * @param[in] bdtxclkforce - The value to use for the BDTXCLKFORCE field.
 * @param[in] agcmemclkforce - The value to use for the AGCMEMCLKFORCE field.
 * @param[in] agcclkforce - The value to use for the AGCCLKFORCE field.
 * @param[in] rcclkforce - The value to use for the RCCLKFORCE field.
 * @param[in] feclkforce - The value to use for the FECLKFORCE field.
 * @param[in] fdoclkforce - The value to use for the FDOCLKFORCE field.
 * @param[in] equclkforce - The value to use for the EQUCLKFORCE field.
 * @param[in] tdcompclkforce - The value to use for the TDCOMPCLKFORCE field.
 * @param[in] tdfoestclkforce - The value to use for the TDFOESTCLKFORCE field.
 * @param[in] tbeclkforce - The value to use for the TBECLKFORCE field.
 * @param[in] fft1memclkforce - The value to use for the FFT1MEMCLKFORCE field.
 * @param[in] fft1clkforce - The value to use for the FFT1CLKFORCE field.
 * @param[in] fft0memclkforce - The value to use for the FFT0MEMCLKFORCE field.
 * @param[in] fft0clkforce - The value to use for the FFT0CLKFORCE field.
 * @param[in] chestclkforce - The value to use for the CHESTCLKFORCE field.
 * @param[in] hmemclkforce - The value to use for the HMEMCLKFORCE field.
 * @param[in] vtb1clkforce - The value to use for the VTB1CLKFORCE field.
 * @param[in] vtb0clkforce - The value to use for the VTB0CLKFORCE field.
 * @param[in] bdrxclkforce - The value to use for the BDRXCLKFORCE field.
 */
__INLINE void crm_clkgatephyfctrl0_pack(uint8_t phytxclkforce, uint8_t bdtxclkforce, uint8_t agcmemclkforce, uint8_t agcclkforce, uint8_t rcclkforce, uint8_t feclkforce, uint8_t fdoclkforce, uint8_t equclkforce, uint8_t tdcompclkforce, uint8_t tdfoestclkforce, uint8_t tbeclkforce, uint8_t fft1memclkforce, uint8_t fft1clkforce, uint8_t fft0memclkforce, uint8_t fft0clkforce, uint8_t chestclkforce, uint8_t hmemclkforce, uint8_t vtb1clkforce, uint8_t vtb0clkforce, uint8_t bdrxclkforce)
{
    ASSERT_ERR((((uint32_t)phytxclkforce << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)bdtxclkforce << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)agcmemclkforce << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)agcclkforce << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)rcclkforce << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)feclkforce << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)fdoclkforce << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)equclkforce << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)tdcompclkforce << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)tdfoestclkforce << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)tbeclkforce << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)fft1memclkforce << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)fft1clkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)fft0memclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)fft0clkforce << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)chestclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)hmemclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)vtb1clkforce << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)vtb0clkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)bdrxclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR,  ((uint32_t)phytxclkforce << 31) | ((uint32_t)bdtxclkforce << 30) | ((uint32_t)agcmemclkforce << 29) | ((uint32_t)agcclkforce << 28) | ((uint32_t)rcclkforce << 27) | ((uint32_t)feclkforce << 24) | ((uint32_t)fdoclkforce << 23) | ((uint32_t)equclkforce << 22) | ((uint32_t)tdcompclkforce << 18) | ((uint32_t)tdfoestclkforce << 17) | ((uint32_t)tbeclkforce << 16) | ((uint32_t)fft1memclkforce << 10) | ((uint32_t)fft1clkforce << 9) | ((uint32_t)fft0memclkforce << 8) | ((uint32_t)fft0clkforce << 7) | ((uint32_t)chestclkforce << 6) | ((uint32_t)hmemclkforce << 4) | ((uint32_t)vtb1clkforce << 2) | ((uint32_t)vtb0clkforce << 1) | ((uint32_t)bdrxclkforce << 0));
}

/**
 * @brief Unpacks CLKGATEPHYFCTRL0's fields from current value of the CLKGATEPHYFCTRL0 register.
 *
 * Reads the CLKGATEPHYFCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] phytxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] bdtxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] agcmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] agcclkforce - Will be populated with the current value of this field from the register.
 * @param[out] rcclkforce - Will be populated with the current value of this field from the register.
 * @param[out] feclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fdoclkforce - Will be populated with the current value of this field from the register.
 * @param[out] equclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdcompclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tdfoestclkforce - Will be populated with the current value of this field from the register.
 * @param[out] tbeclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft1memclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft1clkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft0memclkforce - Will be populated with the current value of this field from the register.
 * @param[out] fft0clkforce - Will be populated with the current value of this field from the register.
 * @param[out] chestclkforce - Will be populated with the current value of this field from the register.
 * @param[out] hmemclkforce - Will be populated with the current value of this field from the register.
 * @param[out] vtb1clkforce - Will be populated with the current value of this field from the register.
 * @param[out] vtb0clkforce - Will be populated with the current value of this field from the register.
 * @param[out] bdrxclkforce - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkgatephyfctrl0_unpack(uint8_t* phytxclkforce, uint8_t* bdtxclkforce, uint8_t* agcmemclkforce, uint8_t* agcclkforce, uint8_t* rcclkforce, uint8_t* feclkforce, uint8_t* fdoclkforce, uint8_t* equclkforce, uint8_t* tdcompclkforce, uint8_t* tdfoestclkforce, uint8_t* tbeclkforce, uint8_t* fft1memclkforce, uint8_t* fft1clkforce, uint8_t* fft0memclkforce, uint8_t* fft0clkforce, uint8_t* chestclkforce, uint8_t* hmemclkforce, uint8_t* vtb1clkforce, uint8_t* vtb0clkforce, uint8_t* bdrxclkforce)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);

    *phytxclkforce = (localVal & ((uint32_t)0x80000000)) >> 31;
    *bdtxclkforce = (localVal & ((uint32_t)0x40000000)) >> 30;
    *agcmemclkforce = (localVal & ((uint32_t)0x20000000)) >> 29;
    *agcclkforce = (localVal & ((uint32_t)0x10000000)) >> 28;
    *rcclkforce = (localVal & ((uint32_t)0x08000000)) >> 27;
    *feclkforce = (localVal & ((uint32_t)0x01000000)) >> 24;
    *fdoclkforce = (localVal & ((uint32_t)0x00800000)) >> 23;
    *equclkforce = (localVal & ((uint32_t)0x00400000)) >> 22;
    *tdcompclkforce = (localVal & ((uint32_t)0x00040000)) >> 18;
    *tdfoestclkforce = (localVal & ((uint32_t)0x00020000)) >> 17;
    *tbeclkforce = (localVal & ((uint32_t)0x00010000)) >> 16;
    *fft1memclkforce = (localVal & ((uint32_t)0x00000400)) >> 10;
    *fft1clkforce = (localVal & ((uint32_t)0x00000200)) >> 9;
    *fft0memclkforce = (localVal & ((uint32_t)0x00000100)) >> 8;
    *fft0clkforce = (localVal & ((uint32_t)0x00000080)) >> 7;
    *chestclkforce = (localVal & ((uint32_t)0x00000040)) >> 6;
    *hmemclkforce = (localVal & ((uint32_t)0x00000010)) >> 4;
    *vtb1clkforce = (localVal & ((uint32_t)0x00000004)) >> 2;
    *vtb0clkforce = (localVal & ((uint32_t)0x00000002)) >> 1;
    *bdrxclkforce = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the PHYTXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the PHYTXCLKFORCE field's value will be returned.
 *
 * @return The current value of the PHYTXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_phytxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the PHYTXCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phytxclkforce - The value to set the field to.
 */
__INLINE void crm_phytxclkforce_setf(uint8_t phytxclkforce)
{
    ASSERT_ERR((((uint32_t)phytxclkforce << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)phytxclkforce << 31));
}

/**
 * @brief Returns the current value of the BDTXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the BDTXCLKFORCE field's value will be returned.
 *
 * @return The current value of the BDTXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_bdtxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the BDTXCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bdtxclkforce - The value to set the field to.
 */
__INLINE void crm_bdtxclkforce_setf(uint8_t bdtxclkforce)
{
    ASSERT_ERR((((uint32_t)bdtxclkforce << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)bdtxclkforce << 30));
}

/**
 * @brief Returns the current value of the AGCMEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the AGCMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the AGCMEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_agcmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the AGCMEMCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcmemclkforce - The value to set the field to.
 */
__INLINE void crm_agcmemclkforce_setf(uint8_t agcmemclkforce)
{
    ASSERT_ERR((((uint32_t)agcmemclkforce << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)agcmemclkforce << 29));
}

/**
 * @brief Returns the current value of the AGCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the AGCCLKFORCE field's value will be returned.
 *
 * @return The current value of the AGCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_agcclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the AGCCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] agcclkforce - The value to set the field to.
 */
__INLINE void crm_agcclkforce_setf(uint8_t agcclkforce)
{
    ASSERT_ERR((((uint32_t)agcclkforce << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)agcclkforce << 28));
}

/**
 * @brief Returns the current value of the RCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the RCCLKFORCE field's value will be returned.
 *
 * @return The current value of the RCCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_rcclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the RCCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rcclkforce - The value to set the field to.
 */
__INLINE void crm_rcclkforce_setf(uint8_t rcclkforce)
{
    ASSERT_ERR((((uint32_t)rcclkforce << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)rcclkforce << 27));
}

/**
 * @brief Returns the current value of the FECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FECLKFORCE field's value will be returned.
 *
 * @return The current value of the FECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_feclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the FECLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] feclkforce - The value to set the field to.
 */
__INLINE void crm_feclkforce_setf(uint8_t feclkforce)
{
    ASSERT_ERR((((uint32_t)feclkforce << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)feclkforce << 24));
}

/**
 * @brief Returns the current value of the FDOCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FDOCLKFORCE field's value will be returned.
 *
 * @return The current value of the FDOCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_fdoclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the FDOCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fdoclkforce - The value to set the field to.
 */
__INLINE void crm_fdoclkforce_setf(uint8_t fdoclkforce)
{
    ASSERT_ERR((((uint32_t)fdoclkforce << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)fdoclkforce << 23));
}

/**
 * @brief Returns the current value of the EQUCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the EQUCLKFORCE field's value will be returned.
 *
 * @return The current value of the EQUCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_equclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the EQUCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] equclkforce - The value to set the field to.
 */
__INLINE void crm_equclkforce_setf(uint8_t equclkforce)
{
    ASSERT_ERR((((uint32_t)equclkforce << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)equclkforce << 22));
}

/**
 * @brief Returns the current value of the TDCOMPCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the TDCOMPCLKFORCE field's value will be returned.
 *
 * @return The current value of the TDCOMPCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_tdcompclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Sets the TDCOMPCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcompclkforce - The value to set the field to.
 */
__INLINE void crm_tdcompclkforce_setf(uint8_t tdcompclkforce)
{
    ASSERT_ERR((((uint32_t)tdcompclkforce << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)tdcompclkforce << 18));
}

/**
 * @brief Returns the current value of the TDFOESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the TDFOESTCLKFORCE field's value will be returned.
 *
 * @return The current value of the TDFOESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_tdfoestclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the TDFOESTCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfoestclkforce - The value to set the field to.
 */
__INLINE void crm_tdfoestclkforce_setf(uint8_t tdfoestclkforce)
{
    ASSERT_ERR((((uint32_t)tdfoestclkforce << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)tdfoestclkforce << 17));
}

/**
 * @brief Returns the current value of the TBECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the TBECLKFORCE field's value will be returned.
 *
 * @return The current value of the TBECLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_tbeclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the TBECLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeclkforce - The value to set the field to.
 */
__INLINE void crm_tbeclkforce_setf(uint8_t tbeclkforce)
{
    ASSERT_ERR((((uint32_t)tbeclkforce << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)tbeclkforce << 16));
}

/**
 * @brief Returns the current value of the FFT1MEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FFT1MEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT1MEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_fft1memclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the FFT1MEMCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft1memclkforce - The value to set the field to.
 */
__INLINE void crm_fft1memclkforce_setf(uint8_t fft1memclkforce)
{
    ASSERT_ERR((((uint32_t)fft1memclkforce << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)fft1memclkforce << 10));
}

/**
 * @brief Returns the current value of the FFT1CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FFT1CLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT1CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_fft1clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the FFT1CLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft1clkforce - The value to set the field to.
 */
__INLINE void crm_fft1clkforce_setf(uint8_t fft1clkforce)
{
    ASSERT_ERR((((uint32_t)fft1clkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)fft1clkforce << 9));
}

/**
 * @brief Returns the current value of the FFT0MEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FFT0MEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT0MEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_fft0memclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the FFT0MEMCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft0memclkforce - The value to set the field to.
 */
__INLINE void crm_fft0memclkforce_setf(uint8_t fft0memclkforce)
{
    ASSERT_ERR((((uint32_t)fft0memclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)fft0memclkforce << 8));
}

/**
 * @brief Returns the current value of the FFT0CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the FFT0CLKFORCE field's value will be returned.
 *
 * @return The current value of the FFT0CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_fft0clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the FFT0CLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fft0clkforce - The value to set the field to.
 */
__INLINE void crm_fft0clkforce_setf(uint8_t fft0clkforce)
{
    ASSERT_ERR((((uint32_t)fft0clkforce << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)fft0clkforce << 7));
}

/**
 * @brief Returns the current value of the CHESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the CHESTCLKFORCE field's value will be returned.
 *
 * @return The current value of the CHESTCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_chestclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the CHESTCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chestclkforce - The value to set the field to.
 */
__INLINE void crm_chestclkforce_setf(uint8_t chestclkforce)
{
    ASSERT_ERR((((uint32_t)chestclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)chestclkforce << 6));
}

/**
 * @brief Returns the current value of the HMEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the HMEMCLKFORCE field's value will be returned.
 *
 * @return The current value of the HMEMCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_hmemclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the HMEMCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hmemclkforce - The value to set the field to.
 */
__INLINE void crm_hmemclkforce_setf(uint8_t hmemclkforce)
{
    ASSERT_ERR((((uint32_t)hmemclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)hmemclkforce << 4));
}

/**
 * @brief Returns the current value of the VTB1CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the VTB1CLKFORCE field's value will be returned.
 *
 * @return The current value of the VTB1CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_vtb1clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the VTB1CLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vtb1clkforce - The value to set the field to.
 */
__INLINE void crm_vtb1clkforce_setf(uint8_t vtb1clkforce)
{
    ASSERT_ERR((((uint32_t)vtb1clkforce << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)vtb1clkforce << 2));
}

/**
 * @brief Returns the current value of the VTB0CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the VTB0CLKFORCE field's value will be returned.
 *
 * @return The current value of the VTB0CLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_vtb0clkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the VTB0CLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] vtb0clkforce - The value to set the field to.
 */
__INLINE void crm_vtb0clkforce_setf(uint8_t vtb0clkforce)
{
    ASSERT_ERR((((uint32_t)vtb0clkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)vtb0clkforce << 1));
}

/**
 * @brief Returns the current value of the BDRXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read and the BDRXCLKFORCE field's value will be returned.
 *
 * @return The current value of the BDRXCLKFORCE field in the CLKGATEPHYFCTRL0 register.
 */
__INLINE uint8_t crm_bdrxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the BDRXCLKFORCE field of the CLKGATEPHYFCTRL0 register.
 *
 * The CLKGATEPHYFCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bdrxclkforce - The value to set the field to.
 */
__INLINE void crm_bdrxclkforce_setf(uint8_t bdrxclkforce)
{
    ASSERT_ERR((((uint32_t)bdrxclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL0_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)bdrxclkforce << 0));
}

/// @}

/**
 * @name CLKGATEPHYFCTRL1 register definitions
 * <table>
 * <caption id="CLKGATEPHYFCTRL1_BF">CLKGATEPHYFCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>09 <td>     TXPKCCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>08 <td>     PHYRXCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>07 <td>       AHBCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>06 <td>   PHYOFDMCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>05 <td>  RADARTIMCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>04 <td>    MDMBTXCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>03 <td>    MDMBRXCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>01 <td>   LDPCENCCLKFORCE <td>R <td>R/W <td>0
 * <tr><td>00 <td>   LDPCDECCLKFORCE <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the CLKGATEPHYFCTRL1 register
#define CRM_CLKGATEPHYFCTRL1_ADDR   0x01050008
/// Offset of the CLKGATEPHYFCTRL1 register from the base address
#define CRM_CLKGATEPHYFCTRL1_OFFSET 0x00000008
/// Index of the CLKGATEPHYFCTRL1 register
#define CRM_CLKGATEPHYFCTRL1_INDEX  0x00000002
/// Reset value of the CLKGATEPHYFCTRL1 register
#define CRM_CLKGATEPHYFCTRL1_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKGATEPHYFCTRL1 register.
 * The CLKGATEPHYFCTRL1 register will be read and its value returned.
 * @return The current value of the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint32_t crm_clkgatephyfctrl1_get(void)
{
    return REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
}

/**
 * @brief Sets the CLKGATEPHYFCTRL1 register to a value.
 * The CLKGATEPHYFCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkgatephyfctrl1_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, value);
}

// field definitions
/// TXPKCCLKFORCE field bit
#define CRM_TXPKCCLKFORCE_BIT       ((uint32_t)0x00000200)
/// TXPKCCLKFORCE field position
#define CRM_TXPKCCLKFORCE_POS       9
/// PHYRXCLKFORCE field bit
#define CRM_PHYRXCLKFORCE_BIT       ((uint32_t)0x00000100)
/// PHYRXCLKFORCE field position
#define CRM_PHYRXCLKFORCE_POS       8
/// AHBCLKFORCE field bit
#define CRM_AHBCLKFORCE_BIT         ((uint32_t)0x00000080)
/// AHBCLKFORCE field position
#define CRM_AHBCLKFORCE_POS         7
/// PHYOFDMCLKFORCE field bit
#define CRM_PHYOFDMCLKFORCE_BIT     ((uint32_t)0x00000040)
/// PHYOFDMCLKFORCE field position
#define CRM_PHYOFDMCLKFORCE_POS     6
/// RADARTIMCLKFORCE field bit
#define CRM_RADARTIMCLKFORCE_BIT    ((uint32_t)0x00000020)
/// RADARTIMCLKFORCE field position
#define CRM_RADARTIMCLKFORCE_POS    5
/// MDMBTXCLKFORCE field bit
#define CRM_MDMBTXCLKFORCE_BIT      ((uint32_t)0x00000010)
/// MDMBTXCLKFORCE field position
#define CRM_MDMBTXCLKFORCE_POS      4
/// MDMBRXCLKFORCE field bit
#define CRM_MDMBRXCLKFORCE_BIT      ((uint32_t)0x00000008)
/// MDMBRXCLKFORCE field position
#define CRM_MDMBRXCLKFORCE_POS      3
/// LDPCENCCLKFORCE field bit
#define CRM_LDPCENCCLKFORCE_BIT     ((uint32_t)0x00000002)
/// LDPCENCCLKFORCE field position
#define CRM_LDPCENCCLKFORCE_POS     1
/// LDPCDECCLKFORCE field bit
#define CRM_LDPCDECCLKFORCE_BIT     ((uint32_t)0x00000001)
/// LDPCDECCLKFORCE field position
#define CRM_LDPCDECCLKFORCE_POS     0

/// TXPKCCLKFORCE field reset value
#define CRM_TXPKCCLKFORCE_RST       0x0
/// PHYRXCLKFORCE field reset value
#define CRM_PHYRXCLKFORCE_RST       0x0
/// AHBCLKFORCE field reset value
#define CRM_AHBCLKFORCE_RST         0x0
/// PHYOFDMCLKFORCE field reset value
#define CRM_PHYOFDMCLKFORCE_RST     0x0
/// RADARTIMCLKFORCE field reset value
#define CRM_RADARTIMCLKFORCE_RST    0x0
/// MDMBTXCLKFORCE field reset value
#define CRM_MDMBTXCLKFORCE_RST      0x0
/// MDMBRXCLKFORCE field reset value
#define CRM_MDMBRXCLKFORCE_RST      0x0
/// LDPCENCCLKFORCE field reset value
#define CRM_LDPCENCCLKFORCE_RST     0x0
/// LDPCDECCLKFORCE field reset value
#define CRM_LDPCDECCLKFORCE_RST     0x0

/**
 * @brief Constructs a value for the CLKGATEPHYFCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txpkcclkforce - The value to use for the TXPKCCLKFORCE field.
 * @param[in] phyrxclkforce - The value to use for the PHYRXCLKFORCE field.
 * @param[in] ahbclkforce - The value to use for the AHBCLKFORCE field.
 * @param[in] phyofdmclkforce - The value to use for the PHYOFDMCLKFORCE field.
 * @param[in] radartimclkforce - The value to use for the RADARTIMCLKFORCE field.
 * @param[in] mdmbtxclkforce - The value to use for the MDMBTXCLKFORCE field.
 * @param[in] mdmbrxclkforce - The value to use for the MDMBRXCLKFORCE field.
 * @param[in] ldpcencclkforce - The value to use for the LDPCENCCLKFORCE field.
 * @param[in] ldpcdecclkforce - The value to use for the LDPCDECCLKFORCE field.
 */
__INLINE void crm_clkgatephyfctrl1_pack(uint8_t txpkcclkforce, uint8_t phyrxclkforce, uint8_t ahbclkforce, uint8_t phyofdmclkforce, uint8_t radartimclkforce, uint8_t mdmbtxclkforce, uint8_t mdmbrxclkforce, uint8_t ldpcencclkforce, uint8_t ldpcdecclkforce)
{
    ASSERT_ERR((((uint32_t)txpkcclkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)phyrxclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)ahbclkforce << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)phyofdmclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)radartimclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)mdmbtxclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)mdmbrxclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)ldpcencclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR,  ((uint32_t)txpkcclkforce << 9) | ((uint32_t)phyrxclkforce << 8) | ((uint32_t)ahbclkforce << 7) | ((uint32_t)phyofdmclkforce << 6) | ((uint32_t)radartimclkforce << 5) | ((uint32_t)mdmbtxclkforce << 4) | ((uint32_t)mdmbrxclkforce << 3) | ((uint32_t)ldpcencclkforce << 1) | ((uint32_t)ldpcdecclkforce << 0));
}

/**
 * @brief Unpacks CLKGATEPHYFCTRL1's fields from current value of the CLKGATEPHYFCTRL1 register.
 *
 * Reads the CLKGATEPHYFCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txpkcclkforce - Will be populated with the current value of this field from the register.
 * @param[out] phyrxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] ahbclkforce - Will be populated with the current value of this field from the register.
 * @param[out] phyofdmclkforce - Will be populated with the current value of this field from the register.
 * @param[out] radartimclkforce - Will be populated with the current value of this field from the register.
 * @param[out] mdmbtxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] mdmbrxclkforce - Will be populated with the current value of this field from the register.
 * @param[out] ldpcencclkforce - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecclkforce - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkgatephyfctrl1_unpack(uint8_t* txpkcclkforce, uint8_t* phyrxclkforce, uint8_t* ahbclkforce, uint8_t* phyofdmclkforce, uint8_t* radartimclkforce, uint8_t* mdmbtxclkforce, uint8_t* mdmbrxclkforce, uint8_t* ldpcencclkforce, uint8_t* ldpcdecclkforce)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);

    *txpkcclkforce = (localVal & ((uint32_t)0x00000200)) >> 9;
    *phyrxclkforce = (localVal & ((uint32_t)0x00000100)) >> 8;
    *ahbclkforce = (localVal & ((uint32_t)0x00000080)) >> 7;
    *phyofdmclkforce = (localVal & ((uint32_t)0x00000040)) >> 6;
    *radartimclkforce = (localVal & ((uint32_t)0x00000020)) >> 5;
    *mdmbtxclkforce = (localVal & ((uint32_t)0x00000010)) >> 4;
    *mdmbrxclkforce = (localVal & ((uint32_t)0x00000008)) >> 3;
    *ldpcencclkforce = (localVal & ((uint32_t)0x00000002)) >> 1;
    *ldpcdecclkforce = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TXPKCCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the TXPKCCLKFORCE field's value will be returned.
 *
 * @return The current value of the TXPKCCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_txpkcclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the TXPKCCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txpkcclkforce - The value to set the field to.
 */
__INLINE void crm_txpkcclkforce_setf(uint8_t txpkcclkforce)
{
    ASSERT_ERR((((uint32_t)txpkcclkforce << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)txpkcclkforce << 9));
}

/**
 * @brief Returns the current value of the PHYRXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the PHYRXCLKFORCE field's value will be returned.
 *
 * @return The current value of the PHYRXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_phyrxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the PHYRXCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyrxclkforce - The value to set the field to.
 */
__INLINE void crm_phyrxclkforce_setf(uint8_t phyrxclkforce)
{
    ASSERT_ERR((((uint32_t)phyrxclkforce << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)phyrxclkforce << 8));
}

/**
 * @brief Returns the current value of the AHBCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the AHBCLKFORCE field's value will be returned.
 *
 * @return The current value of the AHBCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_ahbclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

/**
 * @brief Sets the AHBCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ahbclkforce - The value to set the field to.
 */
__INLINE void crm_ahbclkforce_setf(uint8_t ahbclkforce)
{
    ASSERT_ERR((((uint32_t)ahbclkforce << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)ahbclkforce << 7));
}

/**
 * @brief Returns the current value of the PHYOFDMCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the PHYOFDMCLKFORCE field's value will be returned.
 *
 * @return The current value of the PHYOFDMCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_phyofdmclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the PHYOFDMCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyofdmclkforce - The value to set the field to.
 */
__INLINE void crm_phyofdmclkforce_setf(uint8_t phyofdmclkforce)
{
    ASSERT_ERR((((uint32_t)phyofdmclkforce << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)phyofdmclkforce << 6));
}

/**
 * @brief Returns the current value of the RADARTIMCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the RADARTIMCLKFORCE field's value will be returned.
 *
 * @return The current value of the RADARTIMCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_radartimclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the RADARTIMCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] radartimclkforce - The value to set the field to.
 */
__INLINE void crm_radartimclkforce_setf(uint8_t radartimclkforce)
{
    ASSERT_ERR((((uint32_t)radartimclkforce << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)radartimclkforce << 5));
}

/**
 * @brief Returns the current value of the MDMBTXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the MDMBTXCLKFORCE field's value will be returned.
 *
 * @return The current value of the MDMBTXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_mdmbtxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the MDMBTXCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbtxclkforce - The value to set the field to.
 */
__INLINE void crm_mdmbtxclkforce_setf(uint8_t mdmbtxclkforce)
{
    ASSERT_ERR((((uint32_t)mdmbtxclkforce << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)mdmbtxclkforce << 4));
}

/**
 * @brief Returns the current value of the MDMBRXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the MDMBRXCLKFORCE field's value will be returned.
 *
 * @return The current value of the MDMBRXCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_mdmbrxclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the MDMBRXCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mdmbrxclkforce - The value to set the field to.
 */
__INLINE void crm_mdmbrxclkforce_setf(uint8_t mdmbrxclkforce)
{
    ASSERT_ERR((((uint32_t)mdmbrxclkforce << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)mdmbrxclkforce << 3));
}

/**
 * @brief Returns the current value of the LDPCENCCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the LDPCENCCLKFORCE field's value will be returned.
 *
 * @return The current value of the LDPCENCCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_ldpcencclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the LDPCENCCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcencclkforce - The value to set the field to.
 */
__INLINE void crm_ldpcencclkforce_setf(uint8_t ldpcencclkforce)
{
    ASSERT_ERR((((uint32_t)ldpcencclkforce << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)ldpcencclkforce << 1));
}

/**
 * @brief Returns the current value of the LDPCDECCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read and the LDPCDECCLKFORCE field's value will be returned.
 *
 * @return The current value of the LDPCDECCLKFORCE field in the CLKGATEPHYFCTRL1 register.
 */
__INLINE uint8_t crm_ldpcdecclkforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the LDPCDECCLKFORCE field of the CLKGATEPHYFCTRL1 register.
 *
 * The CLKGATEPHYFCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecclkforce - The value to set the field to.
 */
__INLINE void crm_ldpcdecclkforce_setf(uint8_t ldpcdecclkforce)
{
    ASSERT_ERR((((uint32_t)ldpcdecclkforce << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(CRM_CLKGATEPHYFCTRL1_ADDR, (REG_PL_RD(CRM_CLKGATEPHYFCTRL1_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)ldpcdecclkforce << 0));
}

/// @}

/**
 * @name CLKSELCTRL register definitions
 * <table>
 * <caption id="CLKSELCTRL_BF">CLKSELCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>07:06 <td>        RXBDCLKSEL <td>R <td>R/W <td>0x0
 * <tr><td>05 <td>          FECLKSEL <td>R <td>R/W <td>0
 * <tr><td>04 <td>         PHYCLKSEL <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the CLKSELCTRL register
#define CRM_CLKSELCTRL_ADDR   0x0105000C
/// Offset of the CLKSELCTRL register from the base address
#define CRM_CLKSELCTRL_OFFSET 0x0000000C
/// Index of the CLKSELCTRL register
#define CRM_CLKSELCTRL_INDEX  0x00000003
/// Reset value of the CLKSELCTRL register
#define CRM_CLKSELCTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the CLKSELCTRL register.
 * The CLKSELCTRL register will be read and its value returned.
 * @return The current value of the CLKSELCTRL register.
 */
__INLINE uint32_t crm_clkselctrl_get(void)
{
    return REG_PL_RD(CRM_CLKSELCTRL_ADDR);
}

/**
 * @brief Sets the CLKSELCTRL register to a value.
 * The CLKSELCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_clkselctrl_set(uint32_t value)
{
    REG_PL_WR(CRM_CLKSELCTRL_ADDR, value);
}

// field definitions
/// RXBDCLKSEL field mask
#define CRM_RXBDCLKSEL_MASK   ((uint32_t)0x000000C0)
/// RXBDCLKSEL field LSB position
#define CRM_RXBDCLKSEL_LSB    6
/// RXBDCLKSEL field width
#define CRM_RXBDCLKSEL_WIDTH  ((uint32_t)0x00000002)
/// FECLKSEL field bit
#define CRM_FECLKSEL_BIT      ((uint32_t)0x00000020)
/// FECLKSEL field position
#define CRM_FECLKSEL_POS      5
/// PHYCLKSEL field bit
#define CRM_PHYCLKSEL_BIT     ((uint32_t)0x00000010)
/// PHYCLKSEL field position
#define CRM_PHYCLKSEL_POS     4

/// RXBDCLKSEL field reset value
#define CRM_RXBDCLKSEL_RST    0x0
/// FECLKSEL field reset value
#define CRM_FECLKSEL_RST      0x0
/// PHYCLKSEL field reset value
#define CRM_PHYCLKSEL_RST     0x0

/**
 * @brief Constructs a value for the CLKSELCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxbdclksel - The value to use for the RXBDCLKSEL field.
 * @param[in] feclksel - The value to use for the FECLKSEL field.
 * @param[in] phyclksel - The value to use for the PHYCLKSEL field.
 */
__INLINE void crm_clkselctrl_pack(uint8_t rxbdclksel, uint8_t feclksel, uint8_t phyclksel)
{
    ASSERT_ERR((((uint32_t)rxbdclksel << 6) & ~((uint32_t)0x000000C0)) == 0);
    ASSERT_ERR((((uint32_t)feclksel << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)phyclksel << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(CRM_CLKSELCTRL_ADDR,  ((uint32_t)rxbdclksel << 6) | ((uint32_t)feclksel << 5) | ((uint32_t)phyclksel << 4));
}

/**
 * @brief Unpacks CLKSELCTRL's fields from current value of the CLKSELCTRL register.
 *
 * Reads the CLKSELCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxbdclksel - Will be populated with the current value of this field from the register.
 * @param[out] feclksel - Will be populated with the current value of this field from the register.
 * @param[out] phyclksel - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_clkselctrl_unpack(uint8_t* rxbdclksel, uint8_t* feclksel, uint8_t* phyclksel)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKSELCTRL_ADDR);

    *rxbdclksel = (localVal & ((uint32_t)0x000000C0)) >> 6;
    *feclksel = (localVal & ((uint32_t)0x00000020)) >> 5;
    *phyclksel = (localVal & ((uint32_t)0x00000010)) >> 4;
}

/**
 * @brief Returns the current value of the RXBDCLKSEL field in the CLKSELCTRL register.
 *
 * The CLKSELCTRL register will be read and the RXBDCLKSEL field's value will be returned.
 *
 * @return The current value of the RXBDCLKSEL field in the CLKSELCTRL register.
 */
__INLINE uint8_t crm_rxbdclksel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKSELCTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000C0)) >> 6);
}

/**
 * @brief Sets the RXBDCLKSEL field of the CLKSELCTRL register.
 *
 * The CLKSELCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxbdclksel - The value to set the field to.
 */
__INLINE void crm_rxbdclksel_setf(uint8_t rxbdclksel)
{
    ASSERT_ERR((((uint32_t)rxbdclksel << 6) & ~((uint32_t)0x000000C0)) == 0);
    REG_PL_WR(CRM_CLKSELCTRL_ADDR, (REG_PL_RD(CRM_CLKSELCTRL_ADDR) & ~((uint32_t)0x000000C0)) | ((uint32_t)rxbdclksel << 6));
}

/**
 * @brief Returns the current value of the FECLKSEL field in the CLKSELCTRL register.
 *
 * The CLKSELCTRL register will be read and the FECLKSEL field's value will be returned.
 *
 * @return The current value of the FECLKSEL field in the CLKSELCTRL register.
 */
__INLINE uint8_t crm_feclksel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKSELCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the FECLKSEL field of the CLKSELCTRL register.
 *
 * The CLKSELCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] feclksel - The value to set the field to.
 */
__INLINE void crm_feclksel_setf(uint8_t feclksel)
{
    ASSERT_ERR((((uint32_t)feclksel << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(CRM_CLKSELCTRL_ADDR, (REG_PL_RD(CRM_CLKSELCTRL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)feclksel << 5));
}

/**
 * @brief Returns the current value of the PHYCLKSEL field in the CLKSELCTRL register.
 *
 * The CLKSELCTRL register will be read and the PHYCLKSEL field's value will be returned.
 *
 * @return The current value of the PHYCLKSEL field in the CLKSELCTRL register.
 */
__INLINE uint8_t crm_phyclksel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_CLKSELCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the PHYCLKSEL field of the CLKSELCTRL register.
 *
 * The CLKSELCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phyclksel - The value to set the field to.
 */
__INLINE void crm_phyclksel_setf(uint8_t phyclksel)
{
    ASSERT_ERR((((uint32_t)phyclksel << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(CRM_CLKSELCTRL_ADDR, (REG_PL_RD(CRM_CLKSELCTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)phyclksel << 4));
}

/// @}

/**
 * @name PHY_DIAGSEL register definitions
 * <table>
 * <caption id="PHY_DIAGSEL_BF">PHY_DIAGSEL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>18:16 <td>   PHY_DIAG_CLKSEL <td>R <td>R/W <td>0x0
 * <tr><td>15:08 <td>     PHY_DIAGSEL_H <td>R <td>R/W <td>0x0
 * <tr><td>07:00 <td>     PHY_DIAGSEL_L <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the PHY_DIAGSEL register
#define CRM_PHY_DIAGSEL_ADDR   0x01050010
/// Offset of the PHY_DIAGSEL register from the base address
#define CRM_PHY_DIAGSEL_OFFSET 0x00000010
/// Index of the PHY_DIAGSEL register
#define CRM_PHY_DIAGSEL_INDEX  0x00000004
/// Reset value of the PHY_DIAGSEL register
#define CRM_PHY_DIAGSEL_RESET  0x00000000

/**
 * @brief Returns the current value of the PHY_DIAGSEL register.
 * The PHY_DIAGSEL register will be read and its value returned.
 * @return The current value of the PHY_DIAGSEL register.
 */
__INLINE uint32_t crm_phy_diagsel_get(void)
{
    return REG_PL_RD(CRM_PHY_DIAGSEL_ADDR);
}

/**
 * @brief Sets the PHY_DIAGSEL register to a value.
 * The PHY_DIAGSEL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_phy_diagsel_set(uint32_t value)
{
    REG_PL_WR(CRM_PHY_DIAGSEL_ADDR, value);
}

// field definitions
/// PHY_DIAG_CLKSEL field mask
#define CRM_PHY_DIAG_CLKSEL_MASK   ((uint32_t)0x00070000)
/// PHY_DIAG_CLKSEL field LSB position
#define CRM_PHY_DIAG_CLKSEL_LSB    16
/// PHY_DIAG_CLKSEL field width
#define CRM_PHY_DIAG_CLKSEL_WIDTH  ((uint32_t)0x00000003)
/// PHY_DIAGSEL_H field mask
#define CRM_PHY_DIAGSEL_H_MASK     ((uint32_t)0x0000FF00)
/// PHY_DIAGSEL_H field LSB position
#define CRM_PHY_DIAGSEL_H_LSB      8
/// PHY_DIAGSEL_H field width
#define CRM_PHY_DIAGSEL_H_WIDTH    ((uint32_t)0x00000008)
/// PHY_DIAGSEL_L field mask
#define CRM_PHY_DIAGSEL_L_MASK     ((uint32_t)0x000000FF)
/// PHY_DIAGSEL_L field LSB position
#define CRM_PHY_DIAGSEL_L_LSB      0
/// PHY_DIAGSEL_L field width
#define CRM_PHY_DIAGSEL_L_WIDTH    ((uint32_t)0x00000008)

/// PHY_DIAG_CLKSEL field reset value
#define CRM_PHY_DIAG_CLKSEL_RST    0x0
/// PHY_DIAGSEL_H field reset value
#define CRM_PHY_DIAGSEL_H_RST      0x0
/// PHY_DIAGSEL_L field reset value
#define CRM_PHY_DIAGSEL_L_RST      0x0

/**
 * @brief Constructs a value for the PHY_DIAGSEL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] phydiagclksel - The value to use for the PHY_DIAG_CLKSEL field.
 * @param[in] phydiagselh - The value to use for the PHY_DIAGSEL_H field.
 * @param[in] phydiagsell - The value to use for the PHY_DIAGSEL_L field.
 */
__INLINE void crm_phy_diagsel_pack(uint8_t phydiagclksel, uint8_t phydiagselh, uint8_t phydiagsell)
{
    ASSERT_ERR((((uint32_t)phydiagclksel << 16) & ~((uint32_t)0x00070000)) == 0);
    ASSERT_ERR((((uint32_t)phydiagselh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)phydiagsell << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(CRM_PHY_DIAGSEL_ADDR,  ((uint32_t)phydiagclksel << 16) | ((uint32_t)phydiagselh << 8) | ((uint32_t)phydiagsell << 0));
}

/**
 * @brief Unpacks PHY_DIAGSEL's fields from current value of the PHY_DIAGSEL register.
 *
 * Reads the PHY_DIAGSEL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] phydiagclksel - Will be populated with the current value of this field from the register.
 * @param[out] phydiagselh - Will be populated with the current value of this field from the register.
 * @param[out] phydiagsell - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_phy_diagsel_unpack(uint8_t* phydiagclksel, uint8_t* phydiagselh, uint8_t* phydiagsell)
{
    uint32_t localVal = REG_PL_RD(CRM_PHY_DIAGSEL_ADDR);

    *phydiagclksel = (localVal & ((uint32_t)0x00070000)) >> 16;
    *phydiagselh = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *phydiagsell = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the PHY_DIAG_CLKSEL field in the PHY_DIAGSEL register.
 *
 * The PHY_DIAGSEL register will be read and the PHY_DIAG_CLKSEL field's value will be returned.
 *
 * @return The current value of the PHY_DIAG_CLKSEL field in the PHY_DIAGSEL register.
 */
__INLINE uint8_t crm_phy_diag_clksel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_PHY_DIAGSEL_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

/**
 * @brief Sets the PHY_DIAG_CLKSEL field of the PHY_DIAGSEL register.
 *
 * The PHY_DIAGSEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phydiagclksel - The value to set the field to.
 */
__INLINE void crm_phy_diag_clksel_setf(uint8_t phydiagclksel)
{
    ASSERT_ERR((((uint32_t)phydiagclksel << 16) & ~((uint32_t)0x00070000)) == 0);
    REG_PL_WR(CRM_PHY_DIAGSEL_ADDR, (REG_PL_RD(CRM_PHY_DIAGSEL_ADDR) & ~((uint32_t)0x00070000)) | ((uint32_t)phydiagclksel << 16));
}

/**
 * @brief Returns the current value of the PHY_DIAGSEL_H field in the PHY_DIAGSEL register.
 *
 * The PHY_DIAGSEL register will be read and the PHY_DIAGSEL_H field's value will be returned.
 *
 * @return The current value of the PHY_DIAGSEL_H field in the PHY_DIAGSEL register.
 */
__INLINE uint8_t crm_phy_diagsel_h_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_PHY_DIAGSEL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the PHY_DIAGSEL_H field of the PHY_DIAGSEL register.
 *
 * The PHY_DIAGSEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phydiagselh - The value to set the field to.
 */
__INLINE void crm_phy_diagsel_h_setf(uint8_t phydiagselh)
{
    ASSERT_ERR((((uint32_t)phydiagselh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(CRM_PHY_DIAGSEL_ADDR, (REG_PL_RD(CRM_PHY_DIAGSEL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)phydiagselh << 8));
}

/**
 * @brief Returns the current value of the PHY_DIAGSEL_L field in the PHY_DIAGSEL register.
 *
 * The PHY_DIAGSEL register will be read and the PHY_DIAGSEL_L field's value will be returned.
 *
 * @return The current value of the PHY_DIAGSEL_L field in the PHY_DIAGSEL register.
 */
__INLINE uint8_t crm_phy_diagsel_l_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_PHY_DIAGSEL_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the PHY_DIAGSEL_L field of the PHY_DIAGSEL register.
 *
 * The PHY_DIAGSEL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] phydiagsell - The value to set the field to.
 */
__INLINE void crm_phy_diagsel_l_setf(uint8_t phydiagsell)
{
    ASSERT_ERR((((uint32_t)phydiagsell << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(CRM_PHY_DIAGSEL_ADDR, (REG_PL_RD(CRM_PHY_DIAGSEL_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)phydiagsell << 0));
}

/// @}

/**
 * @name PHY_DIAGVAL register definitions
 * <table>
 * <caption id="PHY_DIAGVAL_BF">PHY_DIAGVAL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:16 <td>     phy_diagval_h <td>R <td>R <td>0x0
 * <tr><td>15:00 <td>     phy_diagval_l <td>R <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the PHY_DIAGVAL register
#define CRM_PHY_DIAGVAL_ADDR   0x01050014
/// Offset of the PHY_DIAGVAL register from the base address
#define CRM_PHY_DIAGVAL_OFFSET 0x00000014
/// Index of the PHY_DIAGVAL register
#define CRM_PHY_DIAGVAL_INDEX  0x00000005
/// Reset value of the PHY_DIAGVAL register
#define CRM_PHY_DIAGVAL_RESET  0x00000000

/**
 * @brief Returns the current value of the PHY_DIAGVAL register.
 * The PHY_DIAGVAL register will be read and its value returned.
 * @return The current value of the PHY_DIAGVAL register.
 */
__INLINE uint32_t crm_phy_diagval_get(void)
{
    return REG_PL_RD(CRM_PHY_DIAGVAL_ADDR);
}

/**
 * @brief Sets the PHY_DIAGVAL register to a value.
 * The PHY_DIAGVAL register will be written.
 * @param value - The value to write.
 */
__INLINE void crm_phy_diagval_set(uint32_t value)
{
    REG_PL_WR(CRM_PHY_DIAGVAL_ADDR, value);
}

// field definitions
/// PHY_DIAGVAL_H field mask
#define CRM_PHY_DIAGVAL_H_MASK   ((uint32_t)0xFFFF0000)
/// PHY_DIAGVAL_H field LSB position
#define CRM_PHY_DIAGVAL_H_LSB    16
/// PHY_DIAGVAL_H field width
#define CRM_PHY_DIAGVAL_H_WIDTH  ((uint32_t)0x00000010)
/// PHY_DIAGVAL_L field mask
#define CRM_PHY_DIAGVAL_L_MASK   ((uint32_t)0x0000FFFF)
/// PHY_DIAGVAL_L field LSB position
#define CRM_PHY_DIAGVAL_L_LSB    0
/// PHY_DIAGVAL_L field width
#define CRM_PHY_DIAGVAL_L_WIDTH  ((uint32_t)0x00000010)

/// PHY_DIAGVAL_H field reset value
#define CRM_PHY_DIAGVAL_H_RST    0x0
/// PHY_DIAGVAL_L field reset value
#define CRM_PHY_DIAGVAL_L_RST    0x0

/**
 * @brief Unpacks PHY_DIAGVAL's fields from current value of the PHY_DIAGVAL register.
 *
 * Reads the PHY_DIAGVAL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] phydiagvalh - Will be populated with the current value of this field from the register.
 * @param[out] phydiagvall - Will be populated with the current value of this field from the register.
 */
__INLINE void crm_phy_diagval_unpack(uint16_t* phydiagvalh, uint16_t* phydiagvall)
{
    uint32_t localVal = REG_PL_RD(CRM_PHY_DIAGVAL_ADDR);

    *phydiagvalh = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *phydiagvall = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the phy_diagval_h field in the PHY_DIAGVAL register.
 *
 * The PHY_DIAGVAL register will be read and the phy_diagval_h field's value will be returned.
 *
 * @return The current value of the phy_diagval_h field in the PHY_DIAGVAL register.
 */
__INLINE uint16_t crm_phy_diagval_h_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_PHY_DIAGVAL_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the phy_diagval_l field in the PHY_DIAGVAL register.
 *
 * The PHY_DIAGVAL register will be read and the phy_diagval_l field's value will be returned.
 *
 * @return The current value of the phy_diagval_l field in the PHY_DIAGVAL register.
 */
__INLINE uint16_t crm_phy_diagval_l_getf(void)
{
    uint32_t localVal = REG_PL_RD(CRM_PHY_DIAGVAL_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}


#endif // _REG_PHY_CRM_H_

/// @}

