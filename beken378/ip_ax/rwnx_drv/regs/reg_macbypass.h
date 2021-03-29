/**
 * @file reg_macbypass.h
 * @brief Definitions of the MACBYP HW block registers and register access functions.
 *
 * @defgroup REG_MACBYPASS REG_MACBYPASS
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the MACBYP HW block registers and register access functions.
 */
#ifndef _REG_MACBYPASS_H_
#define _REG_MACBYPASS_H_

#include "co_int.h"
#include "_reg_macbypass.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_MACBYPASS peripheral.
 */
#define REG_MACBYPASS_COUNT 256

/** @brief Decoding mask of the REG_MACBYPASS peripheral registers from the CPU point of view.
 */
#define REG_MACBYPASS_DECODING_MASK 0x000003FF

/**
 * @name CTRL register definitions
 * <table>
 * <caption id="CTRL_BF">CTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>           TX_DONE <td>W <td>R <td>0
 * <tr><td>09:08 <td>              MODE <td>R/W <td>R/W <td>0x0
 * <tr><td>04 <td>        CLEAR_STAT <td>R/W <td>R/W <td>0
 * <tr><td>03 <td>     DEFRFSHUTDOWN <td>R <td>R/W <td>0
 * <tr><td>02 <td>       DEFKEEPRFON <td>R <td>R/W <td>0
 * <tr><td>01 <td>          DEFRXREQ <td>R <td>R/W <td>0
 * <tr><td>00 <td>            BYPASS <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the CTRL register
#define MACBYP_CTRL_ADDR   0x01060000
/// Offset of the CTRL register from the base address
#define MACBYP_CTRL_OFFSET 0x00000000
/// Index of the CTRL register
#define MACBYP_CTRL_INDEX  0x00000000
/// Reset value of the CTRL register
#define MACBYP_CTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the CTRL register.
 * The CTRL register will be read and its value returned.
 * @return The current value of the CTRL register.
 */
__INLINE uint32_t macbyp_ctrl_get(void)
{
    return REG_PL_RD(MACBYP_CTRL_ADDR);
}

/**
 * @brief Sets the CTRL register to a value.
 * The CTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_ctrl_set(uint32_t value)
{
    REG_PL_WR(MACBYP_CTRL_ADDR, value);
}

// field definitions
/// TX_DONE field bit
#define MACBYP_TX_DONE_BIT          ((uint32_t)0x80000000)
/// TX_DONE field position
#define MACBYP_TX_DONE_POS          31
/// MODE field mask
#define MACBYP_MODE_MASK            ((uint32_t)0x00000300)
/// MODE field LSB position
#define MACBYP_MODE_LSB             8
/// MODE field width
#define MACBYP_MODE_WIDTH           ((uint32_t)0x00000002)
/// CLEAR_STAT field bit
#define MACBYP_CLEAR_STAT_BIT       ((uint32_t)0x00000010)
/// CLEAR_STAT field position
#define MACBYP_CLEAR_STAT_POS       4
/// DEFRFSHUTDOWN field bit
#define MACBYP_DEFRFSHUTDOWN_BIT    ((uint32_t)0x00000008)
/// DEFRFSHUTDOWN field position
#define MACBYP_DEFRFSHUTDOWN_POS    3
/// DEFKEEPRFON field bit
#define MACBYP_DEFKEEPRFON_BIT      ((uint32_t)0x00000004)
/// DEFKEEPRFON field position
#define MACBYP_DEFKEEPRFON_POS      2
/// DEFRXREQ field bit
#define MACBYP_DEFRXREQ_BIT         ((uint32_t)0x00000002)
/// DEFRXREQ field position
#define MACBYP_DEFRXREQ_POS         1
/// BYPASS field bit
#define MACBYP_BYPASS_BIT           ((uint32_t)0x00000001)
/// BYPASS field position
#define MACBYP_BYPASS_POS           0

/// TX_DONE field reset value
#define MACBYP_TX_DONE_RST          0x0
/// MODE field reset value
#define MACBYP_MODE_RST             0x0
/// CLEAR_STAT field reset value
#define MACBYP_CLEAR_STAT_RST       0x0
/// DEFRFSHUTDOWN field reset value
#define MACBYP_DEFRFSHUTDOWN_RST    0x0
/// DEFKEEPRFON field reset value
#define MACBYP_DEFKEEPRFON_RST      0x0
/// DEFRXREQ field reset value
#define MACBYP_DEFRXREQ_RST         0x0
/// BYPASS field reset value
#define MACBYP_BYPASS_RST           0x0

/**
 * @brief Constructs a value for the CTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mode - The value to use for the MODE field.
 * @param[in] clearstat - The value to use for the CLEAR_STAT field.
 * @param[in] defrfshutdown - The value to use for the DEFRFSHUTDOWN field.
 * @param[in] defkeeprfon - The value to use for the DEFKEEPRFON field.
 * @param[in] defrxreq - The value to use for the DEFRXREQ field.
 * @param[in] bypass - The value to use for the BYPASS field.
 */
__INLINE void macbyp_ctrl_pack(uint8_t mode, uint8_t clearstat, uint8_t defrfshutdown, uint8_t defkeeprfon, uint8_t defrxreq, uint8_t bypass)
{
    ASSERT_ERR((((uint32_t)mode << 8) & ~((uint32_t)0x00000300)) == 0);
    ASSERT_ERR((((uint32_t)clearstat << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)defrfshutdown << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)defkeeprfon << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)defrxreq << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)bypass << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_CTRL_ADDR,  ((uint32_t)mode << 8) | ((uint32_t)clearstat << 4) | ((uint32_t)defrfshutdown << 3) | ((uint32_t)defkeeprfon << 2) | ((uint32_t)defrxreq << 1) | ((uint32_t)bypass << 0));
}

/**
 * @brief Unpacks CTRL's fields from current value of the CTRL register.
 *
 * Reads the CTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txdone - Will be populated with the current value of this field from the register.
 * @param[out] mode - Will be populated with the current value of this field from the register.
 * @param[out] clearstat - Will be populated with the current value of this field from the register.
 * @param[out] defrfshutdown - Will be populated with the current value of this field from the register.
 * @param[out] defkeeprfon - Will be populated with the current value of this field from the register.
 * @param[out] defrxreq - Will be populated with the current value of this field from the register.
 * @param[out] bypass - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_ctrl_unpack(uint8_t* txdone, uint8_t* mode, uint8_t* clearstat, uint8_t* defrfshutdown, uint8_t* defkeeprfon, uint8_t* defrxreq, uint8_t* bypass)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);

    *txdone = (localVal & ((uint32_t)0x80000000)) >> 31;
    *mode = (localVal & ((uint32_t)0x00000300)) >> 8;
    *clearstat = (localVal & ((uint32_t)0x00000010)) >> 4;
    *defrfshutdown = (localVal & ((uint32_t)0x00000008)) >> 3;
    *defkeeprfon = (localVal & ((uint32_t)0x00000004)) >> 2;
    *defrxreq = (localVal & ((uint32_t)0x00000002)) >> 1;
    *bypass = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TX_DONE field in the CTRL register.
 *
 * The CTRL register will be read and the TX_DONE field's value will be returned.
 *
 * @return The current value of the TX_DONE field in the CTRL register.
 */
__INLINE uint8_t macbyp_tx_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the MODE field in the CTRL register.
 *
 * The CTRL register will be read and the MODE field's value will be returned.
 *
 * @return The current value of the MODE field in the CTRL register.
 */
__INLINE uint8_t macbyp_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

/**
 * @brief Sets the MODE field of the CTRL register.
 *
 * The CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mode - The value to set the field to.
 */
__INLINE void macbyp_mode_setf(uint8_t mode)
{
    ASSERT_ERR((((uint32_t)mode << 8) & ~((uint32_t)0x00000300)) == 0);
    REG_PL_WR(MACBYP_CTRL_ADDR, (uint32_t)mode << 8);
}

/**
 * @brief Returns the current value of the CLEAR_STAT field in the CTRL register.
 *
 * The CTRL register will be read and the CLEAR_STAT field's value will be returned.
 *
 * @return The current value of the CLEAR_STAT field in the CTRL register.
 */
__INLINE uint8_t macbyp_clear_stat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the CLEAR_STAT field of the CTRL register.
 *
 * The CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] clearstat - The value to set the field to.
 */
__INLINE void macbyp_clear_stat_setf(uint8_t clearstat)
{
    ASSERT_ERR((((uint32_t)clearstat << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MACBYP_CTRL_ADDR, (uint32_t)clearstat << 4);
}

/**
 * @brief Returns the current value of the DEFRFSHUTDOWN field in the CTRL register.
 *
 * The CTRL register will be read and the DEFRFSHUTDOWN field's value will be returned.
 *
 * @return The current value of the DEFRFSHUTDOWN field in the CTRL register.
 */
__INLINE uint8_t macbyp_defrfshutdown_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the DEFRFSHUTDOWN field of the CTRL register.
 *
 * The CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] defrfshutdown - The value to set the field to.
 */
__INLINE void macbyp_defrfshutdown_setf(uint8_t defrfshutdown)
{
    ASSERT_ERR((((uint32_t)defrfshutdown << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MACBYP_CTRL_ADDR, (uint32_t)defrfshutdown << 3);
}

/**
 * @brief Returns the current value of the DEFKEEPRFON field in the CTRL register.
 *
 * The CTRL register will be read and the DEFKEEPRFON field's value will be returned.
 *
 * @return The current value of the DEFKEEPRFON field in the CTRL register.
 */
__INLINE uint8_t macbyp_defkeeprfon_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the DEFKEEPRFON field of the CTRL register.
 *
 * The CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] defkeeprfon - The value to set the field to.
 */
__INLINE void macbyp_defkeeprfon_setf(uint8_t defkeeprfon)
{
    ASSERT_ERR((((uint32_t)defkeeprfon << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MACBYP_CTRL_ADDR, (uint32_t)defkeeprfon << 2);
}

/**
 * @brief Returns the current value of the DEFRXREQ field in the CTRL register.
 *
 * The CTRL register will be read and the DEFRXREQ field's value will be returned.
 *
 * @return The current value of the DEFRXREQ field in the CTRL register.
 */
__INLINE uint8_t macbyp_defrxreq_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the DEFRXREQ field of the CTRL register.
 *
 * The CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] defrxreq - The value to set the field to.
 */
__INLINE void macbyp_defrxreq_setf(uint8_t defrxreq)
{
    ASSERT_ERR((((uint32_t)defrxreq << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MACBYP_CTRL_ADDR, (uint32_t)defrxreq << 1);
}

/**
 * @brief Returns the current value of the BYPASS field in the CTRL register.
 *
 * The CTRL register will be read and the BYPASS field's value will be returned.
 *
 * @return The current value of the BYPASS field in the CTRL register.
 */
__INLINE uint8_t macbyp_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the BYPASS field of the CTRL register.
 *
 * The CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bypass - The value to set the field to.
 */
__INLINE void macbyp_bypass_setf(uint8_t bypass)
{
    ASSERT_ERR((((uint32_t)bypass << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_CTRL_ADDR, (uint32_t)bypass << 0);
}

/// @}

/**
 * @name PAYLOAD register definitions
 * <table>
 * <caption id="PAYLOAD_BF">PAYLOAD bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>19:16 <td>        PAYLOAD_TX <td>R <td>R/W <td>0x0
 * <tr><td>03:00 <td>        PAYLOAD_RX <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the PAYLOAD register
#define MACBYP_PAYLOAD_ADDR   0x01060004
/// Offset of the PAYLOAD register from the base address
#define MACBYP_PAYLOAD_OFFSET 0x00000004
/// Index of the PAYLOAD register
#define MACBYP_PAYLOAD_INDEX  0x00000001
/// Reset value of the PAYLOAD register
#define MACBYP_PAYLOAD_RESET  0x00000000

/**
 * @brief Returns the current value of the PAYLOAD register.
 * The PAYLOAD register will be read and its value returned.
 * @return The current value of the PAYLOAD register.
 */
__INLINE uint32_t macbyp_payload_get(void)
{
    return REG_PL_RD(MACBYP_PAYLOAD_ADDR);
}

/**
 * @brief Sets the PAYLOAD register to a value.
 * The PAYLOAD register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_payload_set(uint32_t value)
{
    REG_PL_WR(MACBYP_PAYLOAD_ADDR, value);
}

// field definitions
/// PAYLOAD_TX field mask
#define MACBYP_PAYLOAD_TX_MASK   ((uint32_t)0x000F0000)
/// PAYLOAD_TX field LSB position
#define MACBYP_PAYLOAD_TX_LSB    16
/// PAYLOAD_TX field width
#define MACBYP_PAYLOAD_TX_WIDTH  ((uint32_t)0x00000004)
/// PAYLOAD_RX field mask
#define MACBYP_PAYLOAD_RX_MASK   ((uint32_t)0x0000000F)
/// PAYLOAD_RX field LSB position
#define MACBYP_PAYLOAD_RX_LSB    0
/// PAYLOAD_RX field width
#define MACBYP_PAYLOAD_RX_WIDTH  ((uint32_t)0x00000004)

/// PAYLOAD_TX field reset value
#define MACBYP_PAYLOAD_TX_RST    0x0
/// PAYLOAD_RX field reset value
#define MACBYP_PAYLOAD_RX_RST    0x0

/**
 * @brief Constructs a value for the PAYLOAD register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] payloadtx - The value to use for the PAYLOAD_TX field.
 * @param[in] payloadrx - The value to use for the PAYLOAD_RX field.
 */
__INLINE void macbyp_payload_pack(uint8_t payloadtx, uint8_t payloadrx)
{
    ASSERT_ERR((((uint32_t)payloadtx << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)payloadrx << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MACBYP_PAYLOAD_ADDR,  ((uint32_t)payloadtx << 16) | ((uint32_t)payloadrx << 0));
}

/**
 * @brief Unpacks PAYLOAD's fields from current value of the PAYLOAD register.
 *
 * Reads the PAYLOAD register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] payloadtx - Will be populated with the current value of this field from the register.
 * @param[out] payloadrx - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_payload_unpack(uint8_t* payloadtx, uint8_t* payloadrx)
{
    uint32_t localVal = REG_PL_RD(MACBYP_PAYLOAD_ADDR);

    *payloadtx = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *payloadrx = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the PAYLOAD_TX field in the PAYLOAD register.
 *
 * The PAYLOAD register will be read and the PAYLOAD_TX field's value will be returned.
 *
 * @return The current value of the PAYLOAD_TX field in the PAYLOAD register.
 */
__INLINE uint8_t macbyp_payload_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_PAYLOAD_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the PAYLOAD_TX field of the PAYLOAD register.
 *
 * The PAYLOAD register will be read, modified to contain the new field value, and written.
 *
 * @param[in] payloadtx - The value to set the field to.
 */
__INLINE void macbyp_payload_tx_setf(uint8_t payloadtx)
{
    ASSERT_ERR((((uint32_t)payloadtx << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(MACBYP_PAYLOAD_ADDR, (REG_PL_RD(MACBYP_PAYLOAD_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)payloadtx << 16));
}

/**
 * @brief Returns the current value of the PAYLOAD_RX field in the PAYLOAD register.
 *
 * The PAYLOAD register will be read and the PAYLOAD_RX field's value will be returned.
 *
 * @return The current value of the PAYLOAD_RX field in the PAYLOAD register.
 */
__INLINE uint8_t macbyp_payload_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_PAYLOAD_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the PAYLOAD_RX field of the PAYLOAD register.
 *
 * The PAYLOAD register will be read, modified to contain the new field value, and written.
 *
 * @param[in] payloadrx - The value to set the field to.
 */
__INLINE void macbyp_payload_rx_setf(uint8_t payloadrx)
{
    ASSERT_ERR((((uint32_t)payloadrx << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MACBYP_PAYLOAD_ADDR, (REG_PL_RD(MACBYP_PAYLOAD_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)payloadrx << 0));
}

/// @}

/**
 * @name TRIGGER register definitions
 * <table>
 * <caption id="TRIGGER_BF">TRIGGER bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>07:04 <td>          TRIGGER1 <td>R <td>R/W <td>0x0
 * <tr><td>03:00 <td>          TRIGGER0 <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIGGER register
#define MACBYP_TRIGGER_ADDR   0x01060008
/// Offset of the TRIGGER register from the base address
#define MACBYP_TRIGGER_OFFSET 0x00000008
/// Index of the TRIGGER register
#define MACBYP_TRIGGER_INDEX  0x00000002
/// Reset value of the TRIGGER register
#define MACBYP_TRIGGER_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIGGER register.
 * The TRIGGER register will be read and its value returned.
 * @return The current value of the TRIGGER register.
 */
__INLINE uint32_t macbyp_trigger_get(void)
{
    return REG_PL_RD(MACBYP_TRIGGER_ADDR);
}

/**
 * @brief Sets the TRIGGER register to a value.
 * The TRIGGER register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trigger_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIGGER_ADDR, value);
}

// field definitions
/// TRIGGER1 field mask
#define MACBYP_TRIGGER1_MASK   ((uint32_t)0x000000F0)
/// TRIGGER1 field LSB position
#define MACBYP_TRIGGER1_LSB    4
/// TRIGGER1 field width
#define MACBYP_TRIGGER1_WIDTH  ((uint32_t)0x00000004)
/// TRIGGER0 field mask
#define MACBYP_TRIGGER0_MASK   ((uint32_t)0x0000000F)
/// TRIGGER0 field LSB position
#define MACBYP_TRIGGER0_LSB    0
/// TRIGGER0 field width
#define MACBYP_TRIGGER0_WIDTH  ((uint32_t)0x00000004)

/// TRIGGER1 field reset value
#define MACBYP_TRIGGER1_RST    0x0
/// TRIGGER0 field reset value
#define MACBYP_TRIGGER0_RST    0x0

/**
 * @brief Constructs a value for the TRIGGER register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigger1 - The value to use for the TRIGGER1 field.
 * @param[in] trigger0 - The value to use for the TRIGGER0 field.
 */
__INLINE void macbyp_trigger_pack(uint8_t trigger1, uint8_t trigger0)
{
    ASSERT_ERR((((uint32_t)trigger1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)trigger0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MACBYP_TRIGGER_ADDR,  ((uint32_t)trigger1 << 4) | ((uint32_t)trigger0 << 0));
}

/**
 * @brief Unpacks TRIGGER's fields from current value of the TRIGGER register.
 *
 * Reads the TRIGGER register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigger1 - Will be populated with the current value of this field from the register.
 * @param[out] trigger0 - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trigger_unpack(uint8_t* trigger1, uint8_t* trigger0)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIGGER_ADDR);

    *trigger1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *trigger0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TRIGGER1 field in the TRIGGER register.
 *
 * The TRIGGER register will be read and the TRIGGER1 field's value will be returned.
 *
 * @return The current value of the TRIGGER1 field in the TRIGGER register.
 */
__INLINE uint8_t macbyp_trigger1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIGGER_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the TRIGGER1 field of the TRIGGER register.
 *
 * The TRIGGER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigger1 - The value to set the field to.
 */
__INLINE void macbyp_trigger1_setf(uint8_t trigger1)
{
    ASSERT_ERR((((uint32_t)trigger1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MACBYP_TRIGGER_ADDR, (REG_PL_RD(MACBYP_TRIGGER_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)trigger1 << 4));
}

/**
 * @brief Returns the current value of the TRIGGER0 field in the TRIGGER register.
 *
 * The TRIGGER register will be read and the TRIGGER0 field's value will be returned.
 *
 * @return The current value of the TRIGGER0 field in the TRIGGER register.
 */
__INLINE uint8_t macbyp_trigger0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIGGER_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the TRIGGER0 field of the TRIGGER register.
 *
 * The TRIGGER register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigger0 - The value to set the field to.
 */
__INLINE void macbyp_trigger0_setf(uint8_t trigger0)
{
    ASSERT_ERR((((uint32_t)trigger0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MACBYP_TRIGGER_ADDR, (REG_PL_RD(MACBYP_TRIGGER_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)trigger0 << 0));
}

/// @}

/**
 * @name CLKEN register definitions
 * <table>
 * <caption id="CLKEN_BF">CLKEN bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>00 <td>             CLKEN <td>R <td>R/W <td>1
 * </table>
 *
 * @{
 */

/// Address of the CLKEN register
#define MACBYP_CLKEN_ADDR   0x0106000C
/// Offset of the CLKEN register from the base address
#define MACBYP_CLKEN_OFFSET 0x0000000C
/// Index of the CLKEN register
#define MACBYP_CLKEN_INDEX  0x00000003
/// Reset value of the CLKEN register
#define MACBYP_CLKEN_RESET  0x00000001

/**
 * @brief Returns the current value of the CLKEN register.
 * The CLKEN register will be read and its value returned.
 * @return The current value of the CLKEN register.
 */
__INLINE uint32_t macbyp_clken_get(void)
{
    return REG_PL_RD(MACBYP_CLKEN_ADDR);
}

/**
 * @brief Sets the CLKEN register to a value.
 * The CLKEN register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_clken_set(uint32_t value)
{
    REG_PL_WR(MACBYP_CLKEN_ADDR, value);
}

// field definitions
/// CLKEN field bit
#define MACBYP_CLKEN_BIT    ((uint32_t)0x00000001)
/// CLKEN field position
#define MACBYP_CLKEN_POS    0

/// CLKEN field reset value
#define MACBYP_CLKEN_RST    0x1

/**
 * @brief Returns the current value of the CLKEN field in the CLKEN register.
 *
 * The CLKEN register will be read and the CLKEN field's value will be returned.
 *
 * @return The current value of the CLKEN field in the CLKEN register.
 */
__INLINE uint8_t macbyp_clken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_CLKEN_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the CLKEN field of the CLKEN register.
 *
 * The CLKEN register will be read, modified to contain the new field value, and written.
 *
 * @param[in] clken - The value to set the field to.
 */
__INLINE void macbyp_clken_setf(uint8_t clken)
{
    ASSERT_ERR((((uint32_t)clken << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_CLKEN_ADDR, (uint32_t)clken << 0);
}

/// @}

/**
 * @name FRAMEPERBURST register definitions
 * <table>
 * <caption id="FRAMEPERBURST_BF">FRAMEPERBURST bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>15:00 <td>            NFRAME <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the FRAMEPERBURST register
#define MACBYP_FRAMEPERBURST_ADDR   0x01060010
/// Offset of the FRAMEPERBURST register from the base address
#define MACBYP_FRAMEPERBURST_OFFSET 0x00000010
/// Index of the FRAMEPERBURST register
#define MACBYP_FRAMEPERBURST_INDEX  0x00000004
/// Reset value of the FRAMEPERBURST register
#define MACBYP_FRAMEPERBURST_RESET  0x00000000

/**
 * @brief Returns the current value of the FRAMEPERBURST register.
 * The FRAMEPERBURST register will be read and its value returned.
 * @return The current value of the FRAMEPERBURST register.
 */
__INLINE uint32_t macbyp_frameperburst_get(void)
{
    return REG_PL_RD(MACBYP_FRAMEPERBURST_ADDR);
}

/**
 * @brief Sets the FRAMEPERBURST register to a value.
 * The FRAMEPERBURST register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_frameperburst_set(uint32_t value)
{
    REG_PL_WR(MACBYP_FRAMEPERBURST_ADDR, value);
}

// field definitions
/// NFRAME field mask
#define MACBYP_NFRAME_MASK   ((uint32_t)0x0000FFFF)
/// NFRAME field LSB position
#define MACBYP_NFRAME_LSB    0
/// NFRAME field width
#define MACBYP_NFRAME_WIDTH  ((uint32_t)0x00000010)

/// NFRAME field reset value
#define MACBYP_NFRAME_RST    0x0

/**
 * @brief Returns the current value of the NFRAME field in the FRAMEPERBURST register.
 *
 * The FRAMEPERBURST register will be read and the NFRAME field's value will be returned.
 *
 * @return The current value of the NFRAME field in the FRAMEPERBURST register.
 */
__INLINE uint16_t macbyp_nframe_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_FRAMEPERBURST_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the NFRAME field of the FRAMEPERBURST register.
 *
 * The FRAMEPERBURST register will be read, modified to contain the new field value, and written.
 *
 * @param[in] nframe - The value to set the field to.
 */
__INLINE void macbyp_nframe_setf(uint16_t nframe)
{
    ASSERT_ERR((((uint32_t)nframe << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(MACBYP_FRAMEPERBURST_ADDR, (uint32_t)nframe << 0);
}

/// @}

/**
 * @name INTERFRAME_DELAY register definitions
 * <table>
 * <caption id="INTERFRAME_DELAY_BF">INTERFRAME_DELAY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>19:00 <td>  INTERFRAME_DELAY <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the INTERFRAME_DELAY register
#define MACBYP_INTERFRAME_DELAY_ADDR   0x01060048
/// Offset of the INTERFRAME_DELAY register from the base address
#define MACBYP_INTERFRAME_DELAY_OFFSET 0x00000048
/// Index of the INTERFRAME_DELAY register
#define MACBYP_INTERFRAME_DELAY_INDEX  0x00000012
/// Reset value of the INTERFRAME_DELAY register
#define MACBYP_INTERFRAME_DELAY_RESET  0x00000000

/**
 * @brief Returns the current value of the INTERFRAME_DELAY register.
 * The INTERFRAME_DELAY register will be read and its value returned.
 * @return The current value of the INTERFRAME_DELAY register.
 */
__INLINE uint32_t macbyp_interframe_delay_get(void)
{
    return REG_PL_RD(MACBYP_INTERFRAME_DELAY_ADDR);
}

/**
 * @brief Sets the INTERFRAME_DELAY register to a value.
 * The INTERFRAME_DELAY register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_interframe_delay_set(uint32_t value)
{
    REG_PL_WR(MACBYP_INTERFRAME_DELAY_ADDR, value);
}

// field definitions
/// INTERFRAME_DELAY field mask
#define MACBYP_INTERFRAME_DELAY_MASK   ((uint32_t)0x000FFFFF)
/// INTERFRAME_DELAY field LSB position
#define MACBYP_INTERFRAME_DELAY_LSB    0
/// INTERFRAME_DELAY field width
#define MACBYP_INTERFRAME_DELAY_WIDTH  ((uint32_t)0x00000014)

/// INTERFRAME_DELAY field reset value
#define MACBYP_INTERFRAME_DELAY_RST    0x0

/**
 * @brief Returns the current value of the INTERFRAME_DELAY field in the INTERFRAME_DELAY register.
 *
 * The INTERFRAME_DELAY register will be read and the INTERFRAME_DELAY field's value will be returned.
 *
 * @return The current value of the INTERFRAME_DELAY field in the INTERFRAME_DELAY register.
 */
__INLINE uint32_t macbyp_interframe_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INTERFRAME_DELAY_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the INTERFRAME_DELAY field of the INTERFRAME_DELAY register.
 *
 * The INTERFRAME_DELAY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] interframedelay - The value to set the field to.
 */
__INLINE void macbyp_interframe_delay_setf(uint32_t interframedelay)
{
    ASSERT_ERR((((uint32_t)interframedelay << 0) & ~((uint32_t)0x000FFFFF)) == 0);
    REG_PL_WR(MACBYP_INTERFRAME_DELAY_ADDR, (uint32_t)interframedelay << 0);
}

/// @}

/**
 * @name STAT_FRAME_OK register definitions
 * <table>
 * <caption id="STAT_FRAME_OK_BF">STAT_FRAME_OK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>     STAT_FRAME_OK <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the STAT_FRAME_OK register
#define MACBYP_STAT_FRAME_OK_ADDR   0x01060080
/// Offset of the STAT_FRAME_OK register from the base address
#define MACBYP_STAT_FRAME_OK_OFFSET 0x00000080
/// Index of the STAT_FRAME_OK register
#define MACBYP_STAT_FRAME_OK_INDEX  0x00000020
/// Reset value of the STAT_FRAME_OK register
#define MACBYP_STAT_FRAME_OK_RESET  0x00000000

/**
 * @brief Returns the current value of the STAT_FRAME_OK register.
 * The STAT_FRAME_OK register will be read and its value returned.
 * @return The current value of the STAT_FRAME_OK register.
 */
__INLINE uint32_t macbyp_stat_frame_ok_get(void)
{
    return REG_PL_RD(MACBYP_STAT_FRAME_OK_ADDR);
}

// field definitions
/// STAT_FRAME_OK field mask
#define MACBYP_STAT_FRAME_OK_MASK   ((uint32_t)0xFFFFFFFF)
/// STAT_FRAME_OK field LSB position
#define MACBYP_STAT_FRAME_OK_LSB    0
/// STAT_FRAME_OK field width
#define MACBYP_STAT_FRAME_OK_WIDTH  ((uint32_t)0x00000020)

/// STAT_FRAME_OK field reset value
#define MACBYP_STAT_FRAME_OK_RST    0x0

/**
 * @brief Returns the current value of the STAT_FRAME_OK field in the STAT_FRAME_OK register.
 *
 * The STAT_FRAME_OK register will be read and the STAT_FRAME_OK field's value will be returned.
 *
 * @return The current value of the STAT_FRAME_OK field in the STAT_FRAME_OK register.
 */
__INLINE uint32_t macbyp_stat_frame_ok_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_FRAME_OK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name STAT_FRAME_BAD register definitions
 * <table>
 * <caption id="STAT_FRAME_BAD_BF">STAT_FRAME_BAD bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>    STAT_FRAME_BAD <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the STAT_FRAME_BAD register
#define MACBYP_STAT_FRAME_BAD_ADDR   0x01060084
/// Offset of the STAT_FRAME_BAD register from the base address
#define MACBYP_STAT_FRAME_BAD_OFFSET 0x00000084
/// Index of the STAT_FRAME_BAD register
#define MACBYP_STAT_FRAME_BAD_INDEX  0x00000021
/// Reset value of the STAT_FRAME_BAD register
#define MACBYP_STAT_FRAME_BAD_RESET  0x00000000

/**
 * @brief Returns the current value of the STAT_FRAME_BAD register.
 * The STAT_FRAME_BAD register will be read and its value returned.
 * @return The current value of the STAT_FRAME_BAD register.
 */
__INLINE uint32_t macbyp_stat_frame_bad_get(void)
{
    return REG_PL_RD(MACBYP_STAT_FRAME_BAD_ADDR);
}

// field definitions
/// STAT_FRAME_BAD field mask
#define MACBYP_STAT_FRAME_BAD_MASK   ((uint32_t)0xFFFFFFFF)
/// STAT_FRAME_BAD field LSB position
#define MACBYP_STAT_FRAME_BAD_LSB    0
/// STAT_FRAME_BAD field width
#define MACBYP_STAT_FRAME_BAD_WIDTH  ((uint32_t)0x00000020)

/// STAT_FRAME_BAD field reset value
#define MACBYP_STAT_FRAME_BAD_RST    0x0

/**
 * @brief Returns the current value of the STAT_FRAME_BAD field in the STAT_FRAME_BAD register.
 *
 * The STAT_FRAME_BAD register will be read and the STAT_FRAME_BAD field's value will be returned.
 *
 * @return The current value of the STAT_FRAME_BAD field in the STAT_FRAME_BAD register.
 */
__INLINE uint32_t macbyp_stat_frame_bad_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_FRAME_BAD_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name STAT_RXEND register definitions
 * <table>
 * <caption id="STAT_RXEND_BF">STAT_RXEND bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>  STAT_FRAME_RXEND <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the STAT_RXEND register
#define MACBYP_STAT_RXEND_ADDR   0x01060088
/// Offset of the STAT_RXEND register from the base address
#define MACBYP_STAT_RXEND_OFFSET 0x00000088
/// Index of the STAT_RXEND register
#define MACBYP_STAT_RXEND_INDEX  0x00000022
/// Reset value of the STAT_RXEND register
#define MACBYP_STAT_RXEND_RESET  0x00000000

/**
 * @brief Returns the current value of the STAT_RXEND register.
 * The STAT_RXEND register will be read and its value returned.
 * @return The current value of the STAT_RXEND register.
 */
__INLINE uint32_t macbyp_stat_rxend_get(void)
{
    return REG_PL_RD(MACBYP_STAT_RXEND_ADDR);
}

// field definitions
/// STAT_FRAME_RXEND field mask
#define MACBYP_STAT_FRAME_RXEND_MASK   ((uint32_t)0xFFFFFFFF)
/// STAT_FRAME_RXEND field LSB position
#define MACBYP_STAT_FRAME_RXEND_LSB    0
/// STAT_FRAME_RXEND field width
#define MACBYP_STAT_FRAME_RXEND_WIDTH  ((uint32_t)0x00000020)

/// STAT_FRAME_RXEND field reset value
#define MACBYP_STAT_FRAME_RXEND_RST    0x0

/**
 * @brief Returns the current value of the STAT_FRAME_RXEND field in the STAT_RXEND register.
 *
 * The STAT_RXEND register will be read and the STAT_FRAME_RXEND field's value will be returned.
 *
 * @return The current value of the STAT_FRAME_RXEND field in the STAT_RXEND register.
 */
__INLINE uint32_t macbyp_stat_frame_rxend_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_RXEND_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name STAT_RXERROR register definitions
 * <table>
 * <caption id="STAT_RXERROR_BF">STAT_RXERROR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>STAT_FRAME_RXERROR <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the STAT_RXERROR register
#define MACBYP_STAT_RXERROR_ADDR   0x0106008C
/// Offset of the STAT_RXERROR register from the base address
#define MACBYP_STAT_RXERROR_OFFSET 0x0000008C
/// Index of the STAT_RXERROR register
#define MACBYP_STAT_RXERROR_INDEX  0x00000023
/// Reset value of the STAT_RXERROR register
#define MACBYP_STAT_RXERROR_RESET  0x00000000

/**
 * @brief Returns the current value of the STAT_RXERROR register.
 * The STAT_RXERROR register will be read and its value returned.
 * @return The current value of the STAT_RXERROR register.
 */
__INLINE uint32_t macbyp_stat_rxerror_get(void)
{
    return REG_PL_RD(MACBYP_STAT_RXERROR_ADDR);
}

// field definitions
/// STAT_FRAME_RXERROR field mask
#define MACBYP_STAT_FRAME_RXERROR_MASK   ((uint32_t)0xFFFFFFFF)
/// STAT_FRAME_RXERROR field LSB position
#define MACBYP_STAT_FRAME_RXERROR_LSB    0
/// STAT_FRAME_RXERROR field width
#define MACBYP_STAT_FRAME_RXERROR_WIDTH  ((uint32_t)0x00000020)

/// STAT_FRAME_RXERROR field reset value
#define MACBYP_STAT_FRAME_RXERROR_RST    0x0

/**
 * @brief Returns the current value of the STAT_FRAME_RXERROR field in the STAT_RXERROR register.
 *
 * The STAT_RXERROR register will be read and the STAT_FRAME_RXERROR field's value will be returned.
 *
 * @return The current value of the STAT_FRAME_RXERROR field in the STAT_RXERROR register.
 */
__INLINE uint32_t macbyp_stat_frame_rxerror_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_RXERROR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name STAT_EVM register definitions
 * <table>
 * <caption id="STAT_EVM_BF">STAT_EVM bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>07:00 <td>               EVM <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the STAT_EVM register
#define MACBYP_STAT_EVM_ADDR   0x01060090
/// Offset of the STAT_EVM register from the base address
#define MACBYP_STAT_EVM_OFFSET 0x00000090
/// Index of the STAT_EVM register
#define MACBYP_STAT_EVM_INDEX  0x00000024
/// Reset value of the STAT_EVM register
#define MACBYP_STAT_EVM_RESET  0x00000000

/**
 * @brief Returns the current value of the STAT_EVM register.
 * The STAT_EVM register will be read and its value returned.
 * @return The current value of the STAT_EVM register.
 */
__INLINE uint32_t macbyp_stat_evm_get(void)
{
    return REG_PL_RD(MACBYP_STAT_EVM_ADDR);
}

// field definitions
/// EVM field mask
#define MACBYP_EVM_MASK   ((uint32_t)0x000000FF)
/// EVM field LSB position
#define MACBYP_EVM_LSB    0
/// EVM field width
#define MACBYP_EVM_WIDTH  ((uint32_t)0x00000008)

/// EVM field reset value
#define MACBYP_EVM_RST    0x0

/**
 * @brief Returns the current value of the EVM field in the STAT_EVM register.
 *
 * The STAT_EVM register will be read and the EVM field's value will be returned.
 *
 * @return The current value of the EVM field in the STAT_EVM register.
 */
__INLINE uint8_t macbyp_evm_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_EVM_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name STAT_RSSI register definitions
 * <table>
 * <caption id="STAT_RSSI_BF">STAT_RSSI bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:24 <td>          RSSI_RX3 <td>W <td>R <td>0x0
 * <tr><td>23:16 <td>          RSSI_RX2 <td>W <td>R <td>0x0
 * <tr><td>15:08 <td>          RSSI_RX1 <td>W <td>R <td>0x0
 * <tr><td>07:00 <td>          RSSI_RX0 <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the STAT_RSSI register
#define MACBYP_STAT_RSSI_ADDR   0x01060094
/// Offset of the STAT_RSSI register from the base address
#define MACBYP_STAT_RSSI_OFFSET 0x00000094
/// Index of the STAT_RSSI register
#define MACBYP_STAT_RSSI_INDEX  0x00000025
/// Reset value of the STAT_RSSI register
#define MACBYP_STAT_RSSI_RESET  0x00000000

/**
 * @brief Returns the current value of the STAT_RSSI register.
 * The STAT_RSSI register will be read and its value returned.
 * @return The current value of the STAT_RSSI register.
 */
__INLINE uint32_t macbyp_stat_rssi_get(void)
{
    return REG_PL_RD(MACBYP_STAT_RSSI_ADDR);
}

// field definitions
/// RSSI_RX3 field mask
#define MACBYP_RSSI_RX3_MASK   ((uint32_t)0xFF000000)
/// RSSI_RX3 field LSB position
#define MACBYP_RSSI_RX3_LSB    24
/// RSSI_RX3 field width
#define MACBYP_RSSI_RX3_WIDTH  ((uint32_t)0x00000008)
/// RSSI_RX2 field mask
#define MACBYP_RSSI_RX2_MASK   ((uint32_t)0x00FF0000)
/// RSSI_RX2 field LSB position
#define MACBYP_RSSI_RX2_LSB    16
/// RSSI_RX2 field width
#define MACBYP_RSSI_RX2_WIDTH  ((uint32_t)0x00000008)
/// RSSI_RX1 field mask
#define MACBYP_RSSI_RX1_MASK   ((uint32_t)0x0000FF00)
/// RSSI_RX1 field LSB position
#define MACBYP_RSSI_RX1_LSB    8
/// RSSI_RX1 field width
#define MACBYP_RSSI_RX1_WIDTH  ((uint32_t)0x00000008)
/// RSSI_RX0 field mask
#define MACBYP_RSSI_RX0_MASK   ((uint32_t)0x000000FF)
/// RSSI_RX0 field LSB position
#define MACBYP_RSSI_RX0_LSB    0
/// RSSI_RX0 field width
#define MACBYP_RSSI_RX0_WIDTH  ((uint32_t)0x00000008)

/// RSSI_RX3 field reset value
#define MACBYP_RSSI_RX3_RST    0x0
/// RSSI_RX2 field reset value
#define MACBYP_RSSI_RX2_RST    0x0
/// RSSI_RX1 field reset value
#define MACBYP_RSSI_RX1_RST    0x0
/// RSSI_RX0 field reset value
#define MACBYP_RSSI_RX0_RST    0x0

/**
 * @brief Unpacks STAT_RSSI's fields from current value of the STAT_RSSI register.
 *
 * Reads the STAT_RSSI register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rssirx3 - Will be populated with the current value of this field from the register.
 * @param[out] rssirx2 - Will be populated with the current value of this field from the register.
 * @param[out] rssirx1 - Will be populated with the current value of this field from the register.
 * @param[out] rssirx0 - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_stat_rssi_unpack(uint8_t* rssirx3, uint8_t* rssirx2, uint8_t* rssirx1, uint8_t* rssirx0)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_RSSI_ADDR);

    *rssirx3 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rssirx2 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rssirx1 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rssirx0 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RSSI_RX3 field in the STAT_RSSI register.
 *
 * The STAT_RSSI register will be read and the RSSI_RX3 field's value will be returned.
 *
 * @return The current value of the RSSI_RX3 field in the STAT_RSSI register.
 */
__INLINE uint8_t macbyp_rssi_rx3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_RSSI_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the RSSI_RX2 field in the STAT_RSSI register.
 *
 * The STAT_RSSI register will be read and the RSSI_RX2 field's value will be returned.
 *
 * @return The current value of the RSSI_RX2 field in the STAT_RSSI register.
 */
__INLINE uint8_t macbyp_rssi_rx2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_RSSI_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the RSSI_RX1 field in the STAT_RSSI register.
 *
 * The STAT_RSSI register will be read and the RSSI_RX1 field's value will be returned.
 *
 * @return The current value of the RSSI_RX1 field in the STAT_RSSI register.
 */
__INLINE uint8_t macbyp_rssi_rx1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_RSSI_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the RSSI_RX0 field in the STAT_RSSI register.
 *
 * The STAT_RSSI register will be read and the RSSI_RX0 field's value will be returned.
 *
 * @return The current value of the RSSI_RX0 field in the STAT_RSSI register.
 */
__INLINE uint8_t macbyp_rssi_rx0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_RSSI_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name STAT_PHYERR register definitions
 * <table>
 * <caption id="STAT_PHYERR_BF">STAT_PHYERR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>       STAT_PHYERR <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the STAT_PHYERR register
#define MACBYP_STAT_PHYERR_ADDR   0x01060098
/// Offset of the STAT_PHYERR register from the base address
#define MACBYP_STAT_PHYERR_OFFSET 0x00000098
/// Index of the STAT_PHYERR register
#define MACBYP_STAT_PHYERR_INDEX  0x00000026
/// Reset value of the STAT_PHYERR register
#define MACBYP_STAT_PHYERR_RESET  0x00000000

/**
 * @brief Returns the current value of the STAT_PHYERR register.
 * The STAT_PHYERR register will be read and its value returned.
 * @return The current value of the STAT_PHYERR register.
 */
__INLINE uint32_t macbyp_stat_phyerr_get(void)
{
    return REG_PL_RD(MACBYP_STAT_PHYERR_ADDR);
}

/**
 * @brief Sets the STAT_PHYERR register to a value.
 * The STAT_PHYERR register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_stat_phyerr_set(uint32_t value)
{
    REG_PL_WR(MACBYP_STAT_PHYERR_ADDR, value);
}

// field definitions
/// STAT_PHYERR field mask
#define MACBYP_STAT_PHYERR_MASK   ((uint32_t)0xFFFFFFFF)
/// STAT_PHYERR field LSB position
#define MACBYP_STAT_PHYERR_LSB    0
/// STAT_PHYERR field width
#define MACBYP_STAT_PHYERR_WIDTH  ((uint32_t)0x00000020)

/// STAT_PHYERR field reset value
#define MACBYP_STAT_PHYERR_RST    0x0

/**
 * @brief Returns the current value of the STAT_PHYERR field in the STAT_PHYERR register.
 *
 * The STAT_PHYERR register will be read and the STAT_PHYERR field's value will be returned.
 *
 * @return The current value of the STAT_PHYERR field in the STAT_PHYERR register.
 */
__INLINE uint32_t macbyp_stat_phyerr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_STAT_PHYERR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name INT_CTRL register definitions
 * <table>
 * <caption id="INT_CTRL_BF">INT_CTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>16 <td>       RXV_LOCK_EN <td>R <td>R/W <td>0
 * <tr><td>03 <td>     INTERRUPT3_EN <td>R <td>R/W <td>0
 * <tr><td>02 <td>     INTERRUPT2_EN <td>R <td>R/W <td>0
 * <tr><td>01 <td>     INTERRUPT1_EN <td>R <td>R/W <td>0
 * <tr><td>00 <td>     INTERRUPT0_EN <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the INT_CTRL register
#define MACBYP_INT_CTRL_ADDR   0x010600A0
/// Offset of the INT_CTRL register from the base address
#define MACBYP_INT_CTRL_OFFSET 0x000000A0
/// Index of the INT_CTRL register
#define MACBYP_INT_CTRL_INDEX  0x00000028
/// Reset value of the INT_CTRL register
#define MACBYP_INT_CTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the INT_CTRL register.
 * The INT_CTRL register will be read and its value returned.
 * @return The current value of the INT_CTRL register.
 */
__INLINE uint32_t macbyp_int_ctrl_get(void)
{
    return REG_PL_RD(MACBYP_INT_CTRL_ADDR);
}

/**
 * @brief Sets the INT_CTRL register to a value.
 * The INT_CTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_int_ctrl_set(uint32_t value)
{
    REG_PL_WR(MACBYP_INT_CTRL_ADDR, value);
}

// field definitions
/// RXV_LOCK_EN field bit
#define MACBYP_RXV_LOCK_EN_BIT      ((uint32_t)0x00010000)
/// RXV_LOCK_EN field position
#define MACBYP_RXV_LOCK_EN_POS      16
/// INTERRUPT3_EN field bit
#define MACBYP_INTERRUPT3_EN_BIT    ((uint32_t)0x00000008)
/// INTERRUPT3_EN field position
#define MACBYP_INTERRUPT3_EN_POS    3
/// INTERRUPT2_EN field bit
#define MACBYP_INTERRUPT2_EN_BIT    ((uint32_t)0x00000004)
/// INTERRUPT2_EN field position
#define MACBYP_INTERRUPT2_EN_POS    2
/// INTERRUPT1_EN field bit
#define MACBYP_INTERRUPT1_EN_BIT    ((uint32_t)0x00000002)
/// INTERRUPT1_EN field position
#define MACBYP_INTERRUPT1_EN_POS    1
/// INTERRUPT0_EN field bit
#define MACBYP_INTERRUPT0_EN_BIT    ((uint32_t)0x00000001)
/// INTERRUPT0_EN field position
#define MACBYP_INTERRUPT0_EN_POS    0

/// RXV_LOCK_EN field reset value
#define MACBYP_RXV_LOCK_EN_RST      0x0
/// INTERRUPT3_EN field reset value
#define MACBYP_INTERRUPT3_EN_RST    0x0
/// INTERRUPT2_EN field reset value
#define MACBYP_INTERRUPT2_EN_RST    0x0
/// INTERRUPT1_EN field reset value
#define MACBYP_INTERRUPT1_EN_RST    0x0
/// INTERRUPT0_EN field reset value
#define MACBYP_INTERRUPT0_EN_RST    0x0

/**
 * @brief Constructs a value for the INT_CTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxvlocken - The value to use for the RXV_LOCK_EN field.
 * @param[in] interrupt3en - The value to use for the INTERRUPT3_EN field.
 * @param[in] interrupt2en - The value to use for the INTERRUPT2_EN field.
 * @param[in] interrupt1en - The value to use for the INTERRUPT1_EN field.
 * @param[in] interrupt0en - The value to use for the INTERRUPT0_EN field.
 */
__INLINE void macbyp_int_ctrl_pack(uint8_t rxvlocken, uint8_t interrupt3en, uint8_t interrupt2en, uint8_t interrupt1en, uint8_t interrupt0en)
{
    ASSERT_ERR((((uint32_t)rxvlocken << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)interrupt3en << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)interrupt2en << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)interrupt1en << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)interrupt0en << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_INT_CTRL_ADDR,  ((uint32_t)rxvlocken << 16) | ((uint32_t)interrupt3en << 3) | ((uint32_t)interrupt2en << 2) | ((uint32_t)interrupt1en << 1) | ((uint32_t)interrupt0en << 0));
}

/**
 * @brief Unpacks INT_CTRL's fields from current value of the INT_CTRL register.
 *
 * Reads the INT_CTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxvlocken - Will be populated with the current value of this field from the register.
 * @param[out] interrupt3en - Will be populated with the current value of this field from the register.
 * @param[out] interrupt2en - Will be populated with the current value of this field from the register.
 * @param[out] interrupt1en - Will be populated with the current value of this field from the register.
 * @param[out] interrupt0en - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_int_ctrl_unpack(uint8_t* rxvlocken, uint8_t* interrupt3en, uint8_t* interrupt2en, uint8_t* interrupt1en, uint8_t* interrupt0en)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_CTRL_ADDR);

    *rxvlocken = (localVal & ((uint32_t)0x00010000)) >> 16;
    *interrupt3en = (localVal & ((uint32_t)0x00000008)) >> 3;
    *interrupt2en = (localVal & ((uint32_t)0x00000004)) >> 2;
    *interrupt1en = (localVal & ((uint32_t)0x00000002)) >> 1;
    *interrupt0en = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RXV_LOCK_EN field in the INT_CTRL register.
 *
 * The INT_CTRL register will be read and the RXV_LOCK_EN field's value will be returned.
 *
 * @return The current value of the RXV_LOCK_EN field in the INT_CTRL register.
 */
__INLINE uint8_t macbyp_rxv_lock_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the RXV_LOCK_EN field of the INT_CTRL register.
 *
 * The INT_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxvlocken - The value to set the field to.
 */
__INLINE void macbyp_rxv_lock_en_setf(uint8_t rxvlocken)
{
    ASSERT_ERR((((uint32_t)rxvlocken << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MACBYP_INT_CTRL_ADDR, (REG_PL_RD(MACBYP_INT_CTRL_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)rxvlocken << 16));
}

/**
 * @brief Returns the current value of the INTERRUPT3_EN field in the INT_CTRL register.
 *
 * The INT_CTRL register will be read and the INTERRUPT3_EN field's value will be returned.
 *
 * @return The current value of the INTERRUPT3_EN field in the INT_CTRL register.
 */
__INLINE uint8_t macbyp_interrupt3_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the INTERRUPT3_EN field of the INT_CTRL register.
 *
 * The INT_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] interrupt3en - The value to set the field to.
 */
__INLINE void macbyp_interrupt3_en_setf(uint8_t interrupt3en)
{
    ASSERT_ERR((((uint32_t)interrupt3en << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MACBYP_INT_CTRL_ADDR, (REG_PL_RD(MACBYP_INT_CTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)interrupt3en << 3));
}

/**
 * @brief Returns the current value of the INTERRUPT2_EN field in the INT_CTRL register.
 *
 * The INT_CTRL register will be read and the INTERRUPT2_EN field's value will be returned.
 *
 * @return The current value of the INTERRUPT2_EN field in the INT_CTRL register.
 */
__INLINE uint8_t macbyp_interrupt2_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the INTERRUPT2_EN field of the INT_CTRL register.
 *
 * The INT_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] interrupt2en - The value to set the field to.
 */
__INLINE void macbyp_interrupt2_en_setf(uint8_t interrupt2en)
{
    ASSERT_ERR((((uint32_t)interrupt2en << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MACBYP_INT_CTRL_ADDR, (REG_PL_RD(MACBYP_INT_CTRL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)interrupt2en << 2));
}

/**
 * @brief Returns the current value of the INTERRUPT1_EN field in the INT_CTRL register.
 *
 * The INT_CTRL register will be read and the INTERRUPT1_EN field's value will be returned.
 *
 * @return The current value of the INTERRUPT1_EN field in the INT_CTRL register.
 */
__INLINE uint8_t macbyp_interrupt1_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the INTERRUPT1_EN field of the INT_CTRL register.
 *
 * The INT_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] interrupt1en - The value to set the field to.
 */
__INLINE void macbyp_interrupt1_en_setf(uint8_t interrupt1en)
{
    ASSERT_ERR((((uint32_t)interrupt1en << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MACBYP_INT_CTRL_ADDR, (REG_PL_RD(MACBYP_INT_CTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)interrupt1en << 1));
}

/**
 * @brief Returns the current value of the INTERRUPT0_EN field in the INT_CTRL register.
 *
 * The INT_CTRL register will be read and the INTERRUPT0_EN field's value will be returned.
 *
 * @return The current value of the INTERRUPT0_EN field in the INT_CTRL register.
 */
__INLINE uint8_t macbyp_interrupt0_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the INTERRUPT0_EN field of the INT_CTRL register.
 *
 * The INT_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] interrupt0en - The value to set the field to.
 */
__INLINE void macbyp_interrupt0_en_setf(uint8_t interrupt0en)
{
    ASSERT_ERR((((uint32_t)interrupt0en << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_INT_CTRL_ADDR, (REG_PL_RD(MACBYP_INT_CTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)interrupt0en << 0));
}

/// @}

/**
 * @name INT_GENERATION register definitions
 * <table>
 * <caption id="INT_GENERATION_BF">INT_GENERATION bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:24 <td>          INT3_GEN <td>R <td>R/W <td>0x0
 * <tr><td>23:16 <td>          INT2_GEN <td>R <td>R/W <td>0x0
 * <tr><td>15:08 <td>          INT1_GEN <td>R <td>R/W <td>0x0
 * <tr><td>07:00 <td>          INT0_GEN <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the INT_GENERATION register
#define MACBYP_INT_GENERATION_ADDR   0x010600A4
/// Offset of the INT_GENERATION register from the base address
#define MACBYP_INT_GENERATION_OFFSET 0x000000A4
/// Index of the INT_GENERATION register
#define MACBYP_INT_GENERATION_INDEX  0x00000029
/// Reset value of the INT_GENERATION register
#define MACBYP_INT_GENERATION_RESET  0x00000000

/**
 * @brief Returns the current value of the INT_GENERATION register.
 * The INT_GENERATION register will be read and its value returned.
 * @return The current value of the INT_GENERATION register.
 */
__INLINE uint32_t macbyp_int_generation_get(void)
{
    return REG_PL_RD(MACBYP_INT_GENERATION_ADDR);
}

/**
 * @brief Sets the INT_GENERATION register to a value.
 * The INT_GENERATION register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_int_generation_set(uint32_t value)
{
    REG_PL_WR(MACBYP_INT_GENERATION_ADDR, value);
}

// field definitions
/// INT3_GEN field mask
#define MACBYP_INT3_GEN_MASK   ((uint32_t)0xFF000000)
/// INT3_GEN field LSB position
#define MACBYP_INT3_GEN_LSB    24
/// INT3_GEN field width
#define MACBYP_INT3_GEN_WIDTH  ((uint32_t)0x00000008)
/// INT2_GEN field mask
#define MACBYP_INT2_GEN_MASK   ((uint32_t)0x00FF0000)
/// INT2_GEN field LSB position
#define MACBYP_INT2_GEN_LSB    16
/// INT2_GEN field width
#define MACBYP_INT2_GEN_WIDTH  ((uint32_t)0x00000008)
/// INT1_GEN field mask
#define MACBYP_INT1_GEN_MASK   ((uint32_t)0x0000FF00)
/// INT1_GEN field LSB position
#define MACBYP_INT1_GEN_LSB    8
/// INT1_GEN field width
#define MACBYP_INT1_GEN_WIDTH  ((uint32_t)0x00000008)
/// INT0_GEN field mask
#define MACBYP_INT0_GEN_MASK   ((uint32_t)0x000000FF)
/// INT0_GEN field LSB position
#define MACBYP_INT0_GEN_LSB    0
/// INT0_GEN field width
#define MACBYP_INT0_GEN_WIDTH  ((uint32_t)0x00000008)

/// INT3_GEN field reset value
#define MACBYP_INT3_GEN_RST    0x0
/// INT2_GEN field reset value
#define MACBYP_INT2_GEN_RST    0x0
/// INT1_GEN field reset value
#define MACBYP_INT1_GEN_RST    0x0
/// INT0_GEN field reset value
#define MACBYP_INT0_GEN_RST    0x0

/**
 * @brief Constructs a value for the INT_GENERATION register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] int3gen - The value to use for the INT3_GEN field.
 * @param[in] int2gen - The value to use for the INT2_GEN field.
 * @param[in] int1gen - The value to use for the INT1_GEN field.
 * @param[in] int0gen - The value to use for the INT0_GEN field.
 */
__INLINE void macbyp_int_generation_pack(uint8_t int3gen, uint8_t int2gen, uint8_t int1gen, uint8_t int0gen)
{
    ASSERT_ERR((((uint32_t)int3gen << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)int2gen << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)int1gen << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)int0gen << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MACBYP_INT_GENERATION_ADDR,  ((uint32_t)int3gen << 24) | ((uint32_t)int2gen << 16) | ((uint32_t)int1gen << 8) | ((uint32_t)int0gen << 0));
}

/**
 * @brief Unpacks INT_GENERATION's fields from current value of the INT_GENERATION register.
 *
 * Reads the INT_GENERATION register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] int3gen - Will be populated with the current value of this field from the register.
 * @param[out] int2gen - Will be populated with the current value of this field from the register.
 * @param[out] int1gen - Will be populated with the current value of this field from the register.
 * @param[out] int0gen - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_int_generation_unpack(uint8_t* int3gen, uint8_t* int2gen, uint8_t* int1gen, uint8_t* int0gen)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_GENERATION_ADDR);

    *int3gen = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *int2gen = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *int1gen = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *int0gen = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the INT3_GEN field in the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read and the INT3_GEN field's value will be returned.
 *
 * @return The current value of the INT3_GEN field in the INT_GENERATION register.
 */
__INLINE uint8_t macbyp_int3_gen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_GENERATION_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the INT3_GEN field of the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int3gen - The value to set the field to.
 */
__INLINE void macbyp_int3_gen_setf(uint8_t int3gen)
{
    ASSERT_ERR((((uint32_t)int3gen << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(MACBYP_INT_GENERATION_ADDR, (REG_PL_RD(MACBYP_INT_GENERATION_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)int3gen << 24));
}

/**
 * @brief Returns the current value of the INT2_GEN field in the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read and the INT2_GEN field's value will be returned.
 *
 * @return The current value of the INT2_GEN field in the INT_GENERATION register.
 */
__INLINE uint8_t macbyp_int2_gen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_GENERATION_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the INT2_GEN field of the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int2gen - The value to set the field to.
 */
__INLINE void macbyp_int2_gen_setf(uint8_t int2gen)
{
    ASSERT_ERR((((uint32_t)int2gen << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(MACBYP_INT_GENERATION_ADDR, (REG_PL_RD(MACBYP_INT_GENERATION_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)int2gen << 16));
}

/**
 * @brief Returns the current value of the INT1_GEN field in the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read and the INT1_GEN field's value will be returned.
 *
 * @return The current value of the INT1_GEN field in the INT_GENERATION register.
 */
__INLINE uint8_t macbyp_int1_gen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_GENERATION_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the INT1_GEN field of the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int1gen - The value to set the field to.
 */
__INLINE void macbyp_int1_gen_setf(uint8_t int1gen)
{
    ASSERT_ERR((((uint32_t)int1gen << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MACBYP_INT_GENERATION_ADDR, (REG_PL_RD(MACBYP_INT_GENERATION_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)int1gen << 8));
}

/**
 * @brief Returns the current value of the INT0_GEN field in the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read and the INT0_GEN field's value will be returned.
 *
 * @return The current value of the INT0_GEN field in the INT_GENERATION register.
 */
__INLINE uint8_t macbyp_int0_gen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_GENERATION_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the INT0_GEN field of the INT_GENERATION register.
 *
 * The INT_GENERATION register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int0gen - The value to set the field to.
 */
__INLINE void macbyp_int0_gen_setf(uint8_t int0gen)
{
    ASSERT_ERR((((uint32_t)int0gen << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MACBYP_INT_GENERATION_ADDR, (REG_PL_RD(MACBYP_INT_GENERATION_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)int0gen << 0));
}

/// @}

/**
 * @name INT_STAT_RAW register definitions
 * <table>
 * <caption id="INT_STAT_RAW_BF">INT_STAT_RAW bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>16 <td>    RXV_LOCK_STATE <td>R/W <td>R <td>0
 * <tr><td>03 <td>        INT3_STATE <td>R/W <td>R <td>0
 * <tr><td>02 <td>        INT2_STATE <td>R/W <td>R <td>0
 * <tr><td>01 <td>        INT1_STATE <td>R/W <td>R <td>0
 * <tr><td>00 <td>        INT0_STATE <td>R/W <td>R <td>0
 * </table>
 *
 * @{
 */

/// Address of the INT_STAT_RAW register
#define MACBYP_INT_STAT_RAW_ADDR   0x010600A8
/// Offset of the INT_STAT_RAW register from the base address
#define MACBYP_INT_STAT_RAW_OFFSET 0x000000A8
/// Index of the INT_STAT_RAW register
#define MACBYP_INT_STAT_RAW_INDEX  0x0000002A
/// Reset value of the INT_STAT_RAW register
#define MACBYP_INT_STAT_RAW_RESET  0x00000000

/**
 * @brief Returns the current value of the INT_STAT_RAW register.
 * The INT_STAT_RAW register will be read and its value returned.
 * @return The current value of the INT_STAT_RAW register.
 */
__INLINE uint32_t macbyp_int_stat_raw_get(void)
{
    return REG_PL_RD(MACBYP_INT_STAT_RAW_ADDR);
}

// field definitions
/// RXV_LOCK_STATE field bit
#define MACBYP_RXV_LOCK_STATE_BIT    ((uint32_t)0x00010000)
/// RXV_LOCK_STATE field position
#define MACBYP_RXV_LOCK_STATE_POS    16
/// INT3_STATE field bit
#define MACBYP_INT3_STATE_BIT        ((uint32_t)0x00000008)
/// INT3_STATE field position
#define MACBYP_INT3_STATE_POS        3
/// INT2_STATE field bit
#define MACBYP_INT2_STATE_BIT        ((uint32_t)0x00000004)
/// INT2_STATE field position
#define MACBYP_INT2_STATE_POS        2
/// INT1_STATE field bit
#define MACBYP_INT1_STATE_BIT        ((uint32_t)0x00000002)
/// INT1_STATE field position
#define MACBYP_INT1_STATE_POS        1
/// INT0_STATE field bit
#define MACBYP_INT0_STATE_BIT        ((uint32_t)0x00000001)
/// INT0_STATE field position
#define MACBYP_INT0_STATE_POS        0

/// RXV_LOCK_STATE field reset value
#define MACBYP_RXV_LOCK_STATE_RST    0x0
/// INT3_STATE field reset value
#define MACBYP_INT3_STATE_RST        0x0
/// INT2_STATE field reset value
#define MACBYP_INT2_STATE_RST        0x0
/// INT1_STATE field reset value
#define MACBYP_INT1_STATE_RST        0x0
/// INT0_STATE field reset value
#define MACBYP_INT0_STATE_RST        0x0

/**
 * @brief Unpacks INT_STAT_RAW's fields from current value of the INT_STAT_RAW register.
 *
 * Reads the INT_STAT_RAW register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxvlockstate - Will be populated with the current value of this field from the register.
 * @param[out] int3state - Will be populated with the current value of this field from the register.
 * @param[out] int2state - Will be populated with the current value of this field from the register.
 * @param[out] int1state - Will be populated with the current value of this field from the register.
 * @param[out] int0state - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_int_stat_raw_unpack(uint8_t* rxvlockstate, uint8_t* int3state, uint8_t* int2state, uint8_t* int1state, uint8_t* int0state)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_RAW_ADDR);

    *rxvlockstate = (localVal & ((uint32_t)0x00010000)) >> 16;
    *int3state = (localVal & ((uint32_t)0x00000008)) >> 3;
    *int2state = (localVal & ((uint32_t)0x00000004)) >> 2;
    *int1state = (localVal & ((uint32_t)0x00000002)) >> 1;
    *int0state = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RXV_LOCK_STATE field in the INT_STAT_RAW register.
 *
 * The INT_STAT_RAW register will be read and the RXV_LOCK_STATE field's value will be returned.
 *
 * @return The current value of the RXV_LOCK_STATE field in the INT_STAT_RAW register.
 */
__INLINE uint8_t macbyp_rxv_lock_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Returns the current value of the INT3_STATE field in the INT_STAT_RAW register.
 *
 * The INT_STAT_RAW register will be read and the INT3_STATE field's value will be returned.
 *
 * @return The current value of the INT3_STATE field in the INT_STAT_RAW register.
 */
__INLINE uint8_t macbyp_int3_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Returns the current value of the INT2_STATE field in the INT_STAT_RAW register.
 *
 * The INT_STAT_RAW register will be read and the INT2_STATE field's value will be returned.
 *
 * @return The current value of the INT2_STATE field in the INT_STAT_RAW register.
 */
__INLINE uint8_t macbyp_int2_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the INT1_STATE field in the INT_STAT_RAW register.
 *
 * The INT_STAT_RAW register will be read and the INT1_STATE field's value will be returned.
 *
 * @return The current value of the INT1_STATE field in the INT_STAT_RAW register.
 */
__INLINE uint8_t macbyp_int1_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the INT0_STATE field in the INT_STAT_RAW register.
 *
 * The INT_STAT_RAW register will be read and the INT0_STATE field's value will be returned.
 *
 * @return The current value of the INT0_STATE field in the INT_STAT_RAW register.
 */
__INLINE uint8_t macbyp_int0_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name INT_STAT_ACK register definitions
 * <table>
 * <caption id="INT_STAT_ACK_BF">INT_STAT_ACK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>16 <td>  RXV_LOCK_RELEASE <td>R/W <td>C <td>0
 * <tr><td>03 <td>          INT3_ACK <td>R/W <td>C <td>0
 * <tr><td>02 <td>          INT2_ACK <td>R/W <td>C <td>0
 * <tr><td>01 <td>          INT1_ACK <td>R/W <td>C <td>0
 * <tr><td>00 <td>          INT0_ACK <td>R/W <td>C <td>0
 * </table>
 *
 * @{
 */

/// Address of the INT_STAT_ACK register
#define MACBYP_INT_STAT_ACK_ADDR   0x010600AC
/// Offset of the INT_STAT_ACK register from the base address
#define MACBYP_INT_STAT_ACK_OFFSET 0x000000AC
/// Index of the INT_STAT_ACK register
#define MACBYP_INT_STAT_ACK_INDEX  0x0000002B
/// Reset value of the INT_STAT_ACK register
#define MACBYP_INT_STAT_ACK_RESET  0x00000000

/**
 * @brief Returns the current value of the INT_STAT_ACK register.
 * The INT_STAT_ACK register will be read and its value returned.
 * @return The current value of the INT_STAT_ACK register.
 */
__INLINE uint32_t macbyp_int_stat_ack_get(void)
{
    return REG_PL_RD(MACBYP_INT_STAT_ACK_ADDR);
}

/**
 * @brief Sets the INT_STAT_ACK register to a value.
 * The INT_STAT_ACK register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_int_stat_ack_clear(uint32_t value)
{
    REG_PL_WR(MACBYP_INT_STAT_ACK_ADDR, value);
}

// field definitions
/// RXV_LOCK_RELEASE field bit
#define MACBYP_RXV_LOCK_RELEASE_BIT    ((uint32_t)0x00010000)
/// RXV_LOCK_RELEASE field position
#define MACBYP_RXV_LOCK_RELEASE_POS    16
/// INT3_ACK field bit
#define MACBYP_INT3_ACK_BIT            ((uint32_t)0x00000008)
/// INT3_ACK field position
#define MACBYP_INT3_ACK_POS            3
/// INT2_ACK field bit
#define MACBYP_INT2_ACK_BIT            ((uint32_t)0x00000004)
/// INT2_ACK field position
#define MACBYP_INT2_ACK_POS            2
/// INT1_ACK field bit
#define MACBYP_INT1_ACK_BIT            ((uint32_t)0x00000002)
/// INT1_ACK field position
#define MACBYP_INT1_ACK_POS            1
/// INT0_ACK field bit
#define MACBYP_INT0_ACK_BIT            ((uint32_t)0x00000001)
/// INT0_ACK field position
#define MACBYP_INT0_ACK_POS            0

/// RXV_LOCK_RELEASE field reset value
#define MACBYP_RXV_LOCK_RELEASE_RST    0x0
/// INT3_ACK field reset value
#define MACBYP_INT3_ACK_RST            0x0
/// INT2_ACK field reset value
#define MACBYP_INT2_ACK_RST            0x0
/// INT1_ACK field reset value
#define MACBYP_INT1_ACK_RST            0x0
/// INT0_ACK field reset value
#define MACBYP_INT0_ACK_RST            0x0

/**
 * @brief Constructs a value for the INT_STAT_ACK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxvlockrelease - The value to use for the RXV_LOCK_RELEASE field.
 * @param[in] int3ack - The value to use for the INT3_ACK field.
 * @param[in] int2ack - The value to use for the INT2_ACK field.
 * @param[in] int1ack - The value to use for the INT1_ACK field.
 * @param[in] int0ack - The value to use for the INT0_ACK field.
 */
__INLINE void macbyp_int_stat_ack_pack(uint8_t rxvlockrelease, uint8_t int3ack, uint8_t int2ack, uint8_t int1ack, uint8_t int0ack)
{
    ASSERT_ERR((((uint32_t)rxvlockrelease << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)int3ack << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)int2ack << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)int1ack << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)int0ack << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_INT_STAT_ACK_ADDR,  ((uint32_t)rxvlockrelease << 16) | ((uint32_t)int3ack << 3) | ((uint32_t)int2ack << 2) | ((uint32_t)int1ack << 1) | ((uint32_t)int0ack << 0));
}

/**
 * @brief Unpacks INT_STAT_ACK's fields from current value of the INT_STAT_ACK register.
 *
 * Reads the INT_STAT_ACK register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxvlockrelease - Will be populated with the current value of this field from the register.
 * @param[out] int3ack - Will be populated with the current value of this field from the register.
 * @param[out] int2ack - Will be populated with the current value of this field from the register.
 * @param[out] int1ack - Will be populated with the current value of this field from the register.
 * @param[out] int0ack - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_int_stat_ack_unpack(uint8_t* rxvlockrelease, uint8_t* int3ack, uint8_t* int2ack, uint8_t* int1ack, uint8_t* int0ack)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_ACK_ADDR);

    *rxvlockrelease = (localVal & ((uint32_t)0x00010000)) >> 16;
    *int3ack = (localVal & ((uint32_t)0x00000008)) >> 3;
    *int2ack = (localVal & ((uint32_t)0x00000004)) >> 2;
    *int1ack = (localVal & ((uint32_t)0x00000002)) >> 1;
    *int0ack = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RXV_LOCK_RELEASE field in the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read and the RXV_LOCK_RELEASE field's value will be returned.
 *
 * @return The current value of the RXV_LOCK_RELEASE field in the INT_STAT_ACK register.
 */
__INLINE uint8_t macbyp_rxv_lock_release_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_ACK_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the RXV_LOCK_RELEASE field of the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxvlockrelease - The value to set the field to.
 */
__INLINE void macbyp_rxv_lock_release_clearf(uint8_t rxvlockrelease)
{
    ASSERT_ERR((((uint32_t)rxvlockrelease << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MACBYP_INT_STAT_ACK_ADDR, (uint32_t)rxvlockrelease << 16);
}

/**
 * @brief Returns the current value of the INT3_ACK field in the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read and the INT3_ACK field's value will be returned.
 *
 * @return The current value of the INT3_ACK field in the INT_STAT_ACK register.
 */
__INLINE uint8_t macbyp_int3_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_ACK_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the INT3_ACK field of the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int3ack - The value to set the field to.
 */
__INLINE void macbyp_int3_ack_clearf(uint8_t int3ack)
{
    ASSERT_ERR((((uint32_t)int3ack << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MACBYP_INT_STAT_ACK_ADDR, (uint32_t)int3ack << 3);
}

/**
 * @brief Returns the current value of the INT2_ACK field in the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read and the INT2_ACK field's value will be returned.
 *
 * @return The current value of the INT2_ACK field in the INT_STAT_ACK register.
 */
__INLINE uint8_t macbyp_int2_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_ACK_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the INT2_ACK field of the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int2ack - The value to set the field to.
 */
__INLINE void macbyp_int2_ack_clearf(uint8_t int2ack)
{
    ASSERT_ERR((((uint32_t)int2ack << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MACBYP_INT_STAT_ACK_ADDR, (uint32_t)int2ack << 2);
}

/**
 * @brief Returns the current value of the INT1_ACK field in the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read and the INT1_ACK field's value will be returned.
 *
 * @return The current value of the INT1_ACK field in the INT_STAT_ACK register.
 */
__INLINE uint8_t macbyp_int1_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_ACK_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the INT1_ACK field of the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int1ack - The value to set the field to.
 */
__INLINE void macbyp_int1_ack_clearf(uint8_t int1ack)
{
    ASSERT_ERR((((uint32_t)int1ack << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MACBYP_INT_STAT_ACK_ADDR, (uint32_t)int1ack << 1);
}

/**
 * @brief Returns the current value of the INT0_ACK field in the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read and the INT0_ACK field's value will be returned.
 *
 * @return The current value of the INT0_ACK field in the INT_STAT_ACK register.
 */
__INLINE uint8_t macbyp_int0_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_STAT_ACK_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the INT0_ACK field of the INT_STAT_ACK register.
 *
 * The INT_STAT_ACK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] int0ack - The value to set the field to.
 */
__INLINE void macbyp_int0_ack_clearf(uint8_t int0ack)
{
    ASSERT_ERR((((uint32_t)int0ack << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_INT_STAT_ACK_ADDR, (uint32_t)int0ack << 0);
}

/// @}

/**
 * @name INT_RXV0 register definitions
 * <table>
 * <caption id="INT_RXV0_BF">INT_RXV0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:24 <td>              RXV3 <td>R/W <td>R <td>0x0
 * <tr><td>23:16 <td>              RXV2 <td>R/W <td>R <td>0x0
 * <tr><td>15:08 <td>              RXV1 <td>R/W <td>R <td>0x0
 * <tr><td>07:00 <td>              RXV0 <td>R/W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the INT_RXV0 register
#define MACBYP_INT_RXV0_ADDR   0x010600B0
/// Offset of the INT_RXV0 register from the base address
#define MACBYP_INT_RXV0_OFFSET 0x000000B0
/// Index of the INT_RXV0 register
#define MACBYP_INT_RXV0_INDEX  0x0000002C
/// Reset value of the INT_RXV0 register
#define MACBYP_INT_RXV0_RESET  0x00000000

/**
 * @brief Returns the current value of the INT_RXV0 register.
 * The INT_RXV0 register will be read and its value returned.
 * @return The current value of the INT_RXV0 register.
 */
__INLINE uint32_t macbyp_int_rxv0_get(void)
{
    return REG_PL_RD(MACBYP_INT_RXV0_ADDR);
}

// field definitions
/// RXV3 field mask
#define MACBYP_RXV3_MASK   ((uint32_t)0xFF000000)
/// RXV3 field LSB position
#define MACBYP_RXV3_LSB    24
/// RXV3 field width
#define MACBYP_RXV3_WIDTH  ((uint32_t)0x00000008)
/// RXV2 field mask
#define MACBYP_RXV2_MASK   ((uint32_t)0x00FF0000)
/// RXV2 field LSB position
#define MACBYP_RXV2_LSB    16
/// RXV2 field width
#define MACBYP_RXV2_WIDTH  ((uint32_t)0x00000008)
/// RXV1 field mask
#define MACBYP_RXV1_MASK   ((uint32_t)0x0000FF00)
/// RXV1 field LSB position
#define MACBYP_RXV1_LSB    8
/// RXV1 field width
#define MACBYP_RXV1_WIDTH  ((uint32_t)0x00000008)
/// RXV0 field mask
#define MACBYP_RXV0_MASK   ((uint32_t)0x000000FF)
/// RXV0 field LSB position
#define MACBYP_RXV0_LSB    0
/// RXV0 field width
#define MACBYP_RXV0_WIDTH  ((uint32_t)0x00000008)

/// RXV3 field reset value
#define MACBYP_RXV3_RST    0x0
/// RXV2 field reset value
#define MACBYP_RXV2_RST    0x0
/// RXV1 field reset value
#define MACBYP_RXV1_RST    0x0
/// RXV0 field reset value
#define MACBYP_RXV0_RST    0x0

/**
 * @brief Unpacks INT_RXV0's fields from current value of the INT_RXV0 register.
 *
 * Reads the INT_RXV0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxv3 - Will be populated with the current value of this field from the register.
 * @param[out] rxv2 - Will be populated with the current value of this field from the register.
 * @param[out] rxv1 - Will be populated with the current value of this field from the register.
 * @param[out] rxv0 - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_int_rxv0_unpack(uint8_t* rxv3, uint8_t* rxv2, uint8_t* rxv1, uint8_t* rxv0)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV0_ADDR);

    *rxv3 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rxv2 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rxv1 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rxv0 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXV3 field in the INT_RXV0 register.
 *
 * The INT_RXV0 register will be read and the RXV3 field's value will be returned.
 *
 * @return The current value of the RXV3 field in the INT_RXV0 register.
 */
__INLINE uint8_t macbyp_rxv3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV0_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the RXV2 field in the INT_RXV0 register.
 *
 * The INT_RXV0 register will be read and the RXV2 field's value will be returned.
 *
 * @return The current value of the RXV2 field in the INT_RXV0 register.
 */
__INLINE uint8_t macbyp_rxv2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV0_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the RXV1 field in the INT_RXV0 register.
 *
 * The INT_RXV0 register will be read and the RXV1 field's value will be returned.
 *
 * @return The current value of the RXV1 field in the INT_RXV0 register.
 */
__INLINE uint8_t macbyp_rxv1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV0_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the RXV0 field in the INT_RXV0 register.
 *
 * The INT_RXV0 register will be read and the RXV0 field's value will be returned.
 *
 * @return The current value of the RXV0 field in the INT_RXV0 register.
 */
__INLINE uint8_t macbyp_rxv0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV0_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name INT_RXV4 register definitions
 * <table>
 * <caption id="INT_RXV4_BF">INT_RXV4 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:24 <td>              RXV7 <td>R/W <td>R <td>0x0
 * <tr><td>23:16 <td>              RXV6 <td>R/W <td>R <td>0x0
 * <tr><td>15:08 <td>              RXV5 <td>R/W <td>R <td>0x0
 * <tr><td>07:00 <td>              RXV4 <td>R/W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the INT_RXV4 register
#define MACBYP_INT_RXV4_ADDR   0x010600B4
/// Offset of the INT_RXV4 register from the base address
#define MACBYP_INT_RXV4_OFFSET 0x000000B4
/// Index of the INT_RXV4 register
#define MACBYP_INT_RXV4_INDEX  0x0000002D
/// Reset value of the INT_RXV4 register
#define MACBYP_INT_RXV4_RESET  0x00000000

/**
 * @brief Returns the current value of the INT_RXV4 register.
 * The INT_RXV4 register will be read and its value returned.
 * @return The current value of the INT_RXV4 register.
 */
__INLINE uint32_t macbyp_int_rxv4_get(void)
{
    return REG_PL_RD(MACBYP_INT_RXV4_ADDR);
}

// field definitions
/// RXV7 field mask
#define MACBYP_RXV7_MASK   ((uint32_t)0xFF000000)
/// RXV7 field LSB position
#define MACBYP_RXV7_LSB    24
/// RXV7 field width
#define MACBYP_RXV7_WIDTH  ((uint32_t)0x00000008)
/// RXV6 field mask
#define MACBYP_RXV6_MASK   ((uint32_t)0x00FF0000)
/// RXV6 field LSB position
#define MACBYP_RXV6_LSB    16
/// RXV6 field width
#define MACBYP_RXV6_WIDTH  ((uint32_t)0x00000008)
/// RXV5 field mask
#define MACBYP_RXV5_MASK   ((uint32_t)0x0000FF00)
/// RXV5 field LSB position
#define MACBYP_RXV5_LSB    8
/// RXV5 field width
#define MACBYP_RXV5_WIDTH  ((uint32_t)0x00000008)
/// RXV4 field mask
#define MACBYP_RXV4_MASK   ((uint32_t)0x000000FF)
/// RXV4 field LSB position
#define MACBYP_RXV4_LSB    0
/// RXV4 field width
#define MACBYP_RXV4_WIDTH  ((uint32_t)0x00000008)

/// RXV7 field reset value
#define MACBYP_RXV7_RST    0x0
/// RXV6 field reset value
#define MACBYP_RXV6_RST    0x0
/// RXV5 field reset value
#define MACBYP_RXV5_RST    0x0
/// RXV4 field reset value
#define MACBYP_RXV4_RST    0x0

/**
 * @brief Unpacks INT_RXV4's fields from current value of the INT_RXV4 register.
 *
 * Reads the INT_RXV4 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxv7 - Will be populated with the current value of this field from the register.
 * @param[out] rxv6 - Will be populated with the current value of this field from the register.
 * @param[out] rxv5 - Will be populated with the current value of this field from the register.
 * @param[out] rxv4 - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_int_rxv4_unpack(uint8_t* rxv7, uint8_t* rxv6, uint8_t* rxv5, uint8_t* rxv4)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV4_ADDR);

    *rxv7 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rxv6 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rxv5 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rxv4 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXV7 field in the INT_RXV4 register.
 *
 * The INT_RXV4 register will be read and the RXV7 field's value will be returned.
 *
 * @return The current value of the RXV7 field in the INT_RXV4 register.
 */
__INLINE uint8_t macbyp_rxv7_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV4_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the RXV6 field in the INT_RXV4 register.
 *
 * The INT_RXV4 register will be read and the RXV6 field's value will be returned.
 *
 * @return The current value of the RXV6 field in the INT_RXV4 register.
 */
__INLINE uint8_t macbyp_rxv6_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV4_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the RXV5 field in the INT_RXV4 register.
 *
 * The INT_RXV4 register will be read and the RXV5 field's value will be returned.
 *
 * @return The current value of the RXV5 field in the INT_RXV4 register.
 */
__INLINE uint8_t macbyp_rxv5_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV4_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the RXV4 field in the INT_RXV4 register.
 *
 * The INT_RXV4 register will be read and the RXV4 field's value will be returned.
 *
 * @return The current value of the RXV4 field in the INT_RXV4 register.
 */
__INLINE uint8_t macbyp_rxv4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV4_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name INT_RXV8 register definitions
 * <table>
 * <caption id="INT_RXV8_BF">INT_RXV8 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:24 <td>             RXV11 <td>R/W <td>R <td>0x0
 * <tr><td>23:16 <td>             RXV10 <td>R/W <td>R <td>0x0
 * <tr><td>15:08 <td>              RXV9 <td>R/W <td>R <td>0x0
 * <tr><td>07:00 <td>              RXV8 <td>R/W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the INT_RXV8 register
#define MACBYP_INT_RXV8_ADDR   0x010600B8
/// Offset of the INT_RXV8 register from the base address
#define MACBYP_INT_RXV8_OFFSET 0x000000B8
/// Index of the INT_RXV8 register
#define MACBYP_INT_RXV8_INDEX  0x0000002E
/// Reset value of the INT_RXV8 register
#define MACBYP_INT_RXV8_RESET  0x00000000

/**
 * @brief Returns the current value of the INT_RXV8 register.
 * The INT_RXV8 register will be read and its value returned.
 * @return The current value of the INT_RXV8 register.
 */
__INLINE uint32_t macbyp_int_rxv8_get(void)
{
    return REG_PL_RD(MACBYP_INT_RXV8_ADDR);
}

// field definitions
/// RXV11 field mask
#define MACBYP_RXV11_MASK   ((uint32_t)0xFF000000)
/// RXV11 field LSB position
#define MACBYP_RXV11_LSB    24
/// RXV11 field width
#define MACBYP_RXV11_WIDTH  ((uint32_t)0x00000008)
/// RXV10 field mask
#define MACBYP_RXV10_MASK   ((uint32_t)0x00FF0000)
/// RXV10 field LSB position
#define MACBYP_RXV10_LSB    16
/// RXV10 field width
#define MACBYP_RXV10_WIDTH  ((uint32_t)0x00000008)
/// RXV9 field mask
#define MACBYP_RXV9_MASK    ((uint32_t)0x0000FF00)
/// RXV9 field LSB position
#define MACBYP_RXV9_LSB     8
/// RXV9 field width
#define MACBYP_RXV9_WIDTH   ((uint32_t)0x00000008)
/// RXV8 field mask
#define MACBYP_RXV8_MASK    ((uint32_t)0x000000FF)
/// RXV8 field LSB position
#define MACBYP_RXV8_LSB     0
/// RXV8 field width
#define MACBYP_RXV8_WIDTH   ((uint32_t)0x00000008)

/// RXV11 field reset value
#define MACBYP_RXV11_RST    0x0
/// RXV10 field reset value
#define MACBYP_RXV10_RST    0x0
/// RXV9 field reset value
#define MACBYP_RXV9_RST     0x0
/// RXV8 field reset value
#define MACBYP_RXV8_RST     0x0

/**
 * @brief Unpacks INT_RXV8's fields from current value of the INT_RXV8 register.
 *
 * Reads the INT_RXV8 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxv11 - Will be populated with the current value of this field from the register.
 * @param[out] rxv10 - Will be populated with the current value of this field from the register.
 * @param[out] rxv9 - Will be populated with the current value of this field from the register.
 * @param[out] rxv8 - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_int_rxv8_unpack(uint8_t* rxv11, uint8_t* rxv10, uint8_t* rxv9, uint8_t* rxv8)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV8_ADDR);

    *rxv11 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rxv10 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rxv9 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rxv8 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXV11 field in the INT_RXV8 register.
 *
 * The INT_RXV8 register will be read and the RXV11 field's value will be returned.
 *
 * @return The current value of the RXV11 field in the INT_RXV8 register.
 */
__INLINE uint8_t macbyp_rxv11_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV8_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the RXV10 field in the INT_RXV8 register.
 *
 * The INT_RXV8 register will be read and the RXV10 field's value will be returned.
 *
 * @return The current value of the RXV10 field in the INT_RXV8 register.
 */
__INLINE uint8_t macbyp_rxv10_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV8_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the RXV9 field in the INT_RXV8 register.
 *
 * The INT_RXV8 register will be read and the RXV9 field's value will be returned.
 *
 * @return The current value of the RXV9 field in the INT_RXV8 register.
 */
__INLINE uint8_t macbyp_rxv9_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV8_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the RXV8 field in the INT_RXV8 register.
 *
 * The INT_RXV8 register will be read and the RXV8 field's value will be returned.
 *
 * @return The current value of the RXV8 field in the INT_RXV8 register.
 */
__INLINE uint8_t macbyp_rxv8_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_INT_RXV8_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name RXV12 register definitions
 * <table>
 * <caption id="RXV12_BF">RXV12 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>23:16 <td>             RXV14 <td>R/W <td>R <td>0x0
 * <tr><td>15:08 <td>             RXV13 <td>R/W <td>R <td>0x0
 * <tr><td>07:00 <td>             RXV12 <td>R/W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the RXV12 register
#define MACBYP_RXV12_ADDR   0x010600BC
/// Offset of the RXV12 register from the base address
#define MACBYP_RXV12_OFFSET 0x000000BC
/// Index of the RXV12 register
#define MACBYP_RXV12_INDEX  0x0000002F
/// Reset value of the RXV12 register
#define MACBYP_RXV12_RESET  0x00000000

/**
 * @brief Returns the current value of the RXV12 register.
 * The RXV12 register will be read and its value returned.
 * @return The current value of the RXV12 register.
 */
__INLINE uint32_t macbyp_rxv12_get(void)
{
    return REG_PL_RD(MACBYP_RXV12_ADDR);
}

// field definitions
/// RXV14 field mask
#define MACBYP_RXV14_MASK   ((uint32_t)0x00FF0000)
/// RXV14 field LSB position
#define MACBYP_RXV14_LSB    16
/// RXV14 field width
#define MACBYP_RXV14_WIDTH  ((uint32_t)0x00000008)
/// RXV13 field mask
#define MACBYP_RXV13_MASK   ((uint32_t)0x0000FF00)
/// RXV13 field LSB position
#define MACBYP_RXV13_LSB    8
/// RXV13 field width
#define MACBYP_RXV13_WIDTH  ((uint32_t)0x00000008)
/// RXV12 field mask
#define MACBYP_RXV12_MASK   ((uint32_t)0x000000FF)
/// RXV12 field LSB position
#define MACBYP_RXV12_LSB    0
/// RXV12 field width
#define MACBYP_RXV12_WIDTH  ((uint32_t)0x00000008)

/// RXV14 field reset value
#define MACBYP_RXV14_RST    0x0
/// RXV13 field reset value
#define MACBYP_RXV13_RST    0x0
/// RXV12 field reset value
#define MACBYP_RXV12_RST    0x0

/**
 * @brief Unpacks RXV12's fields from current value of the RXV12 register.
 *
 * Reads the RXV12 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxv14 - Will be populated with the current value of this field from the register.
 * @param[out] rxv13 - Will be populated with the current value of this field from the register.
 * @param[out] rxv12 - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_rxv12_unpack(uint8_t* rxv14, uint8_t* rxv13, uint8_t* rxv12)
{
    uint32_t localVal = REG_PL_RD(MACBYP_RXV12_ADDR);

    *rxv14 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rxv13 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *rxv12 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXV14 field in the RXV12 register.
 *
 * The RXV12 register will be read and the RXV14 field's value will be returned.
 *
 * @return The current value of the RXV14 field in the RXV12 register.
 */
__INLINE uint8_t macbyp_rxv14_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_RXV12_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the RXV13 field in the RXV12 register.
 *
 * The RXV12 register will be read and the RXV13 field's value will be returned.
 *
 * @return The current value of the RXV13 field in the RXV12 register.
 */
__INLINE uint8_t macbyp_rxv13_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_RXV12_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Returns the current value of the RXV12 field in the RXV12 register.
 *
 * The RXV12 register will be read and the RXV12 field's value will be returned.
 *
 * @return The current value of the RXV12 field in the RXV12 register.
 */
__INLINE uint8_t macbyp_rxv12_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_RXV12_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

/**
 * @name TRIG_FORMATMOD register definitions
 * <table>
 * <caption id="TRIG_FORMATMOD_BF">TRIG_FORMATMOD bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td> TRIG_FORMATMOD_EN <td>R <td>R/W <td>0
 * <tr><td>02:00 <td>    TRIG_FORMATMOD <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_FORMATMOD register
#define MACBYP_TRIG_FORMATMOD_ADDR   0x01060100
/// Offset of the TRIG_FORMATMOD register from the base address
#define MACBYP_TRIG_FORMATMOD_OFFSET 0x00000100
/// Index of the TRIG_FORMATMOD register
#define MACBYP_TRIG_FORMATMOD_INDEX  0x00000040
/// Reset value of the TRIG_FORMATMOD register
#define MACBYP_TRIG_FORMATMOD_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_FORMATMOD register.
 * The TRIG_FORMATMOD register will be read and its value returned.
 * @return The current value of the TRIG_FORMATMOD register.
 */
__INLINE uint32_t macbyp_trig_formatmod_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_FORMATMOD_ADDR);
}

/**
 * @brief Sets the TRIG_FORMATMOD register to a value.
 * The TRIG_FORMATMOD register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_formatmod_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_FORMATMOD_ADDR, value);
}

// field definitions
/// TRIG_FORMATMOD_EN field bit
#define MACBYP_TRIG_FORMATMOD_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_FORMATMOD_EN field position
#define MACBYP_TRIG_FORMATMOD_EN_POS    31
/// TRIG_FORMATMOD field mask
#define MACBYP_TRIG_FORMATMOD_MASK      ((uint32_t)0x00000007)
/// TRIG_FORMATMOD field LSB position
#define MACBYP_TRIG_FORMATMOD_LSB       0
/// TRIG_FORMATMOD field width
#define MACBYP_TRIG_FORMATMOD_WIDTH     ((uint32_t)0x00000003)

/// TRIG_FORMATMOD_EN field reset value
#define MACBYP_TRIG_FORMATMOD_EN_RST    0x0
/// TRIG_FORMATMOD field reset value
#define MACBYP_TRIG_FORMATMOD_RST       0x0

/**
 * @brief Constructs a value for the TRIG_FORMATMOD register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigformatmoden - The value to use for the TRIG_FORMATMOD_EN field.
 * @param[in] trigformatmod - The value to use for the TRIG_FORMATMOD field.
 */
__INLINE void macbyp_trig_formatmod_pack(uint8_t trigformatmoden, uint8_t trigformatmod)
{
    ASSERT_ERR((((uint32_t)trigformatmoden << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigformatmod << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(MACBYP_TRIG_FORMATMOD_ADDR,  ((uint32_t)trigformatmoden << 31) | ((uint32_t)trigformatmod << 0));
}

/**
 * @brief Unpacks TRIG_FORMATMOD's fields from current value of the TRIG_FORMATMOD register.
 *
 * Reads the TRIG_FORMATMOD register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigformatmoden - Will be populated with the current value of this field from the register.
 * @param[out] trigformatmod - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_formatmod_unpack(uint8_t* trigformatmoden, uint8_t* trigformatmod)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_FORMATMOD_ADDR);

    *trigformatmoden = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigformatmod = (localVal & ((uint32_t)0x00000007)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_FORMATMOD_EN field in the TRIG_FORMATMOD register.
 *
 * The TRIG_FORMATMOD register will be read and the TRIG_FORMATMOD_EN field's value will be returned.
 *
 * @return The current value of the TRIG_FORMATMOD_EN field in the TRIG_FORMATMOD register.
 */
__INLINE uint8_t macbyp_trig_formatmod_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_FORMATMOD_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_FORMATMOD_EN field of the TRIG_FORMATMOD register.
 *
 * The TRIG_FORMATMOD register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigformatmoden - The value to set the field to.
 */
__INLINE void macbyp_trig_formatmod_en_setf(uint8_t trigformatmoden)
{
    ASSERT_ERR((((uint32_t)trigformatmoden << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_FORMATMOD_ADDR, (REG_PL_RD(MACBYP_TRIG_FORMATMOD_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigformatmoden << 31));
}

/**
 * @brief Returns the current value of the TRIG_FORMATMOD field in the TRIG_FORMATMOD register.
 *
 * The TRIG_FORMATMOD register will be read and the TRIG_FORMATMOD field's value will be returned.
 *
 * @return The current value of the TRIG_FORMATMOD field in the TRIG_FORMATMOD register.
 */
__INLINE uint8_t macbyp_trig_formatmod_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_FORMATMOD_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

/**
 * @brief Sets the TRIG_FORMATMOD field of the TRIG_FORMATMOD register.
 *
 * The TRIG_FORMATMOD register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigformatmod - The value to set the field to.
 */
__INLINE void macbyp_trig_formatmod_setf(uint8_t trigformatmod)
{
    ASSERT_ERR((((uint32_t)trigformatmod << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(MACBYP_TRIG_FORMATMOD_ADDR, (REG_PL_RD(MACBYP_TRIG_FORMATMOD_ADDR) & ~((uint32_t)0x00000007)) | ((uint32_t)trigformatmod << 0));
}

/// @}

/**
 * @name TRIG_MCS register definitions
 * <table>
 * <caption id="TRIG_MCS_BF">TRIG_MCS bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>       TRIG_MCS_EN <td>R <td>R/W <td>0
 * <tr><td>06:00 <td>          TRIG_MCS <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_MCS register
#define MACBYP_TRIG_MCS_ADDR   0x01060104
/// Offset of the TRIG_MCS register from the base address
#define MACBYP_TRIG_MCS_OFFSET 0x00000104
/// Index of the TRIG_MCS register
#define MACBYP_TRIG_MCS_INDEX  0x00000041
/// Reset value of the TRIG_MCS register
#define MACBYP_TRIG_MCS_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_MCS register.
 * The TRIG_MCS register will be read and its value returned.
 * @return The current value of the TRIG_MCS register.
 */
__INLINE uint32_t macbyp_trig_mcs_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_MCS_ADDR);
}

/**
 * @brief Sets the TRIG_MCS register to a value.
 * The TRIG_MCS register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_mcs_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_MCS_ADDR, value);
}

// field definitions
/// TRIG_MCS_EN field bit
#define MACBYP_TRIG_MCS_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_MCS_EN field position
#define MACBYP_TRIG_MCS_EN_POS    31
/// TRIG_MCS field mask
#define MACBYP_TRIG_MCS_MASK      ((uint32_t)0x0000007F)
/// TRIG_MCS field LSB position
#define MACBYP_TRIG_MCS_LSB       0
/// TRIG_MCS field width
#define MACBYP_TRIG_MCS_WIDTH     ((uint32_t)0x00000007)

/// TRIG_MCS_EN field reset value
#define MACBYP_TRIG_MCS_EN_RST    0x0
/// TRIG_MCS field reset value
#define MACBYP_TRIG_MCS_RST       0x0

/**
 * @brief Constructs a value for the TRIG_MCS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigmcsen - The value to use for the TRIG_MCS_EN field.
 * @param[in] trigmcs - The value to use for the TRIG_MCS field.
 */
__INLINE void macbyp_trig_mcs_pack(uint8_t trigmcsen, uint8_t trigmcs)
{
    ASSERT_ERR((((uint32_t)trigmcsen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigmcs << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MACBYP_TRIG_MCS_ADDR,  ((uint32_t)trigmcsen << 31) | ((uint32_t)trigmcs << 0));
}

/**
 * @brief Unpacks TRIG_MCS's fields from current value of the TRIG_MCS register.
 *
 * Reads the TRIG_MCS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigmcsen - Will be populated with the current value of this field from the register.
 * @param[out] trigmcs - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_mcs_unpack(uint8_t* trigmcsen, uint8_t* trigmcs)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_MCS_ADDR);

    *trigmcsen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigmcs = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_MCS_EN field in the TRIG_MCS register.
 *
 * The TRIG_MCS register will be read and the TRIG_MCS_EN field's value will be returned.
 *
 * @return The current value of the TRIG_MCS_EN field in the TRIG_MCS register.
 */
__INLINE uint8_t macbyp_trig_mcs_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_MCS_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_MCS_EN field of the TRIG_MCS register.
 *
 * The TRIG_MCS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigmcsen - The value to set the field to.
 */
__INLINE void macbyp_trig_mcs_en_setf(uint8_t trigmcsen)
{
    ASSERT_ERR((((uint32_t)trigmcsen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_MCS_ADDR, (REG_PL_RD(MACBYP_TRIG_MCS_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigmcsen << 31));
}

/**
 * @brief Returns the current value of the TRIG_MCS field in the TRIG_MCS register.
 *
 * The TRIG_MCS register will be read and the TRIG_MCS field's value will be returned.
 *
 * @return The current value of the TRIG_MCS field in the TRIG_MCS register.
 */
__INLINE uint8_t macbyp_trig_mcs_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_MCS_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the TRIG_MCS field of the TRIG_MCS register.
 *
 * The TRIG_MCS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigmcs - The value to set the field to.
 */
__INLINE void macbyp_trig_mcs_setf(uint8_t trigmcs)
{
    ASSERT_ERR((((uint32_t)trigmcs << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MACBYP_TRIG_MCS_ADDR, (REG_PL_RD(MACBYP_TRIG_MCS_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)trigmcs << 0));
}

/// @}

/**
 * @name TRIG_SGI register definitions
 * <table>
 * <caption id="TRIG_SGI_BF">TRIG_SGI bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>       TRIG_SGI_EN <td>R <td>R/W <td>0
 * <tr><td>00 <td>          TRIG_SGI <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_SGI register
#define MACBYP_TRIG_SGI_ADDR   0x01060108
/// Offset of the TRIG_SGI register from the base address
#define MACBYP_TRIG_SGI_OFFSET 0x00000108
/// Index of the TRIG_SGI register
#define MACBYP_TRIG_SGI_INDEX  0x00000042
/// Reset value of the TRIG_SGI register
#define MACBYP_TRIG_SGI_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_SGI register.
 * The TRIG_SGI register will be read and its value returned.
 * @return The current value of the TRIG_SGI register.
 */
__INLINE uint32_t macbyp_trig_sgi_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_SGI_ADDR);
}

/**
 * @brief Sets the TRIG_SGI register to a value.
 * The TRIG_SGI register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_sgi_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_SGI_ADDR, value);
}

// field definitions
/// TRIG_SGI_EN field bit
#define MACBYP_TRIG_SGI_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_SGI_EN field position
#define MACBYP_TRIG_SGI_EN_POS    31
/// TRIG_SGI field bit
#define MACBYP_TRIG_SGI_BIT       ((uint32_t)0x00000001)
/// TRIG_SGI field position
#define MACBYP_TRIG_SGI_POS       0

/// TRIG_SGI_EN field reset value
#define MACBYP_TRIG_SGI_EN_RST    0x0
/// TRIG_SGI field reset value
#define MACBYP_TRIG_SGI_RST       0x0

/**
 * @brief Constructs a value for the TRIG_SGI register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigsgien - The value to use for the TRIG_SGI_EN field.
 * @param[in] trigsgi - The value to use for the TRIG_SGI field.
 */
__INLINE void macbyp_trig_sgi_pack(uint8_t trigsgien, uint8_t trigsgi)
{
    ASSERT_ERR((((uint32_t)trigsgien << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigsgi << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_SGI_ADDR,  ((uint32_t)trigsgien << 31) | ((uint32_t)trigsgi << 0));
}

/**
 * @brief Unpacks TRIG_SGI's fields from current value of the TRIG_SGI register.
 *
 * Reads the TRIG_SGI register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigsgien - Will be populated with the current value of this field from the register.
 * @param[out] trigsgi - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_sgi_unpack(uint8_t* trigsgien, uint8_t* trigsgi)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SGI_ADDR);

    *trigsgien = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigsgi = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_SGI_EN field in the TRIG_SGI register.
 *
 * The TRIG_SGI register will be read and the TRIG_SGI_EN field's value will be returned.
 *
 * @return The current value of the TRIG_SGI_EN field in the TRIG_SGI register.
 */
__INLINE uint8_t macbyp_trig_sgi_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SGI_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_SGI_EN field of the TRIG_SGI register.
 *
 * The TRIG_SGI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigsgien - The value to set the field to.
 */
__INLINE void macbyp_trig_sgi_en_setf(uint8_t trigsgien)
{
    ASSERT_ERR((((uint32_t)trigsgien << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_SGI_ADDR, (REG_PL_RD(MACBYP_TRIG_SGI_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigsgien << 31));
}

/**
 * @brief Returns the current value of the TRIG_SGI field in the TRIG_SGI register.
 *
 * The TRIG_SGI register will be read and the TRIG_SGI field's value will be returned.
 *
 * @return The current value of the TRIG_SGI field in the TRIG_SGI register.
 */
__INLINE uint8_t macbyp_trig_sgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SGI_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the TRIG_SGI field of the TRIG_SGI register.
 *
 * The TRIG_SGI register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigsgi - The value to set the field to.
 */
__INLINE void macbyp_trig_sgi_setf(uint8_t trigsgi)
{
    ASSERT_ERR((((uint32_t)trigsgi << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_SGI_ADDR, (REG_PL_RD(MACBYP_TRIG_SGI_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)trigsgi << 0));
}

/// @}

/**
 * @name TRIG_FEC register definitions
 * <table>
 * <caption id="TRIG_FEC_BF">TRIG_FEC bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>       TRIG_FEC_EN <td>R <td>R/W <td>0
 * <tr><td>00 <td>          TRIG_FEC <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_FEC register
#define MACBYP_TRIG_FEC_ADDR   0x0106010C
/// Offset of the TRIG_FEC register from the base address
#define MACBYP_TRIG_FEC_OFFSET 0x0000010C
/// Index of the TRIG_FEC register
#define MACBYP_TRIG_FEC_INDEX  0x00000043
/// Reset value of the TRIG_FEC register
#define MACBYP_TRIG_FEC_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_FEC register.
 * The TRIG_FEC register will be read and its value returned.
 * @return The current value of the TRIG_FEC register.
 */
__INLINE uint32_t macbyp_trig_fec_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_FEC_ADDR);
}

/**
 * @brief Sets the TRIG_FEC register to a value.
 * The TRIG_FEC register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_fec_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_FEC_ADDR, value);
}

// field definitions
/// TRIG_FEC_EN field bit
#define MACBYP_TRIG_FEC_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_FEC_EN field position
#define MACBYP_TRIG_FEC_EN_POS    31
/// TRIG_FEC field bit
#define MACBYP_TRIG_FEC_BIT       ((uint32_t)0x00000001)
/// TRIG_FEC field position
#define MACBYP_TRIG_FEC_POS       0

/// TRIG_FEC_EN field reset value
#define MACBYP_TRIG_FEC_EN_RST    0x0
/// TRIG_FEC field reset value
#define MACBYP_TRIG_FEC_RST       0x0

/**
 * @brief Constructs a value for the TRIG_FEC register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigfecen - The value to use for the TRIG_FEC_EN field.
 * @param[in] trigfec - The value to use for the TRIG_FEC field.
 */
__INLINE void macbyp_trig_fec_pack(uint8_t trigfecen, uint8_t trigfec)
{
    ASSERT_ERR((((uint32_t)trigfecen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigfec << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_FEC_ADDR,  ((uint32_t)trigfecen << 31) | ((uint32_t)trigfec << 0));
}

/**
 * @brief Unpacks TRIG_FEC's fields from current value of the TRIG_FEC register.
 *
 * Reads the TRIG_FEC register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigfecen - Will be populated with the current value of this field from the register.
 * @param[out] trigfec - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_fec_unpack(uint8_t* trigfecen, uint8_t* trigfec)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_FEC_ADDR);

    *trigfecen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigfec = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_FEC_EN field in the TRIG_FEC register.
 *
 * The TRIG_FEC register will be read and the TRIG_FEC_EN field's value will be returned.
 *
 * @return The current value of the TRIG_FEC_EN field in the TRIG_FEC register.
 */
__INLINE uint8_t macbyp_trig_fec_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_FEC_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_FEC_EN field of the TRIG_FEC register.
 *
 * The TRIG_FEC register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigfecen - The value to set the field to.
 */
__INLINE void macbyp_trig_fec_en_setf(uint8_t trigfecen)
{
    ASSERT_ERR((((uint32_t)trigfecen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_FEC_ADDR, (REG_PL_RD(MACBYP_TRIG_FEC_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigfecen << 31));
}

/**
 * @brief Returns the current value of the TRIG_FEC field in the TRIG_FEC register.
 *
 * The TRIG_FEC register will be read and the TRIG_FEC field's value will be returned.
 *
 * @return The current value of the TRIG_FEC field in the TRIG_FEC register.
 */
__INLINE uint8_t macbyp_trig_fec_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_FEC_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the TRIG_FEC field of the TRIG_FEC register.
 *
 * The TRIG_FEC register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigfec - The value to set the field to.
 */
__INLINE void macbyp_trig_fec_setf(uint8_t trigfec)
{
    ASSERT_ERR((((uint32_t)trigfec << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_FEC_ADDR, (REG_PL_RD(MACBYP_TRIG_FEC_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)trigfec << 0));
}

/// @}

/**
 * @name TRIG_STBC register definitions
 * <table>
 * <caption id="TRIG_STBC_BF">TRIG_STBC bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>      TRIG_STBC_EN <td>R <td>R/W <td>0
 * <tr><td>01:00 <td>         TRIG_STBC <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_STBC register
#define MACBYP_TRIG_STBC_ADDR   0x01060110
/// Offset of the TRIG_STBC register from the base address
#define MACBYP_TRIG_STBC_OFFSET 0x00000110
/// Index of the TRIG_STBC register
#define MACBYP_TRIG_STBC_INDEX  0x00000044
/// Reset value of the TRIG_STBC register
#define MACBYP_TRIG_STBC_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_STBC register.
 * The TRIG_STBC register will be read and its value returned.
 * @return The current value of the TRIG_STBC register.
 */
__INLINE uint32_t macbyp_trig_stbc_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_STBC_ADDR);
}

/**
 * @brief Sets the TRIG_STBC register to a value.
 * The TRIG_STBC register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_stbc_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_STBC_ADDR, value);
}

// field definitions
/// TRIG_STBC_EN field bit
#define MACBYP_TRIG_STBC_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_STBC_EN field position
#define MACBYP_TRIG_STBC_EN_POS    31
/// TRIG_STBC field mask
#define MACBYP_TRIG_STBC_MASK      ((uint32_t)0x00000003)
/// TRIG_STBC field LSB position
#define MACBYP_TRIG_STBC_LSB       0
/// TRIG_STBC field width
#define MACBYP_TRIG_STBC_WIDTH     ((uint32_t)0x00000002)

/// TRIG_STBC_EN field reset value
#define MACBYP_TRIG_STBC_EN_RST    0x0
/// TRIG_STBC field reset value
#define MACBYP_TRIG_STBC_RST       0x0

/**
 * @brief Constructs a value for the TRIG_STBC register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigstbcen - The value to use for the TRIG_STBC_EN field.
 * @param[in] trigstbc - The value to use for the TRIG_STBC field.
 */
__INLINE void macbyp_trig_stbc_pack(uint8_t trigstbcen, uint8_t trigstbc)
{
    ASSERT_ERR((((uint32_t)trigstbcen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigstbc << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MACBYP_TRIG_STBC_ADDR,  ((uint32_t)trigstbcen << 31) | ((uint32_t)trigstbc << 0));
}

/**
 * @brief Unpacks TRIG_STBC's fields from current value of the TRIG_STBC register.
 *
 * Reads the TRIG_STBC register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigstbcen - Will be populated with the current value of this field from the register.
 * @param[out] trigstbc - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_stbc_unpack(uint8_t* trigstbcen, uint8_t* trigstbc)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_STBC_ADDR);

    *trigstbcen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigstbc = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_STBC_EN field in the TRIG_STBC register.
 *
 * The TRIG_STBC register will be read and the TRIG_STBC_EN field's value will be returned.
 *
 * @return The current value of the TRIG_STBC_EN field in the TRIG_STBC register.
 */
__INLINE uint8_t macbyp_trig_stbc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_STBC_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_STBC_EN field of the TRIG_STBC register.
 *
 * The TRIG_STBC register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigstbcen - The value to set the field to.
 */
__INLINE void macbyp_trig_stbc_en_setf(uint8_t trigstbcen)
{
    ASSERT_ERR((((uint32_t)trigstbcen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_STBC_ADDR, (REG_PL_RD(MACBYP_TRIG_STBC_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigstbcen << 31));
}

/**
 * @brief Returns the current value of the TRIG_STBC field in the TRIG_STBC register.
 *
 * The TRIG_STBC register will be read and the TRIG_STBC field's value will be returned.
 *
 * @return The current value of the TRIG_STBC field in the TRIG_STBC register.
 */
__INLINE uint8_t macbyp_trig_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_STBC_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the TRIG_STBC field of the TRIG_STBC register.
 *
 * The TRIG_STBC register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigstbc - The value to set the field to.
 */
__INLINE void macbyp_trig_stbc_setf(uint8_t trigstbc)
{
    ASSERT_ERR((((uint32_t)trigstbc << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MACBYP_TRIG_STBC_ADDR, (REG_PL_RD(MACBYP_TRIG_STBC_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)trigstbc << 0));
}

/// @}

/**
 * @name TRIG_CHBW register definitions
 * <table>
 * <caption id="TRIG_CHBW_BF">TRIG_CHBW bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>      TRIG_CHBW_EN <td>R <td>R/W <td>0
 * <tr><td>01:00 <td>         TRIG_CHBW <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_CHBW register
#define MACBYP_TRIG_CHBW_ADDR   0x01060114
/// Offset of the TRIG_CHBW register from the base address
#define MACBYP_TRIG_CHBW_OFFSET 0x00000114
/// Index of the TRIG_CHBW register
#define MACBYP_TRIG_CHBW_INDEX  0x00000045
/// Reset value of the TRIG_CHBW register
#define MACBYP_TRIG_CHBW_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_CHBW register.
 * The TRIG_CHBW register will be read and its value returned.
 * @return The current value of the TRIG_CHBW register.
 */
__INLINE uint32_t macbyp_trig_chbw_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_CHBW_ADDR);
}

/**
 * @brief Sets the TRIG_CHBW register to a value.
 * The TRIG_CHBW register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_chbw_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_CHBW_ADDR, value);
}

// field definitions
/// TRIG_CHBW_EN field bit
#define MACBYP_TRIG_CHBW_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_CHBW_EN field position
#define MACBYP_TRIG_CHBW_EN_POS    31
/// TRIG_CHBW field mask
#define MACBYP_TRIG_CHBW_MASK      ((uint32_t)0x00000003)
/// TRIG_CHBW field LSB position
#define MACBYP_TRIG_CHBW_LSB       0
/// TRIG_CHBW field width
#define MACBYP_TRIG_CHBW_WIDTH     ((uint32_t)0x00000002)

/// TRIG_CHBW_EN field reset value
#define MACBYP_TRIG_CHBW_EN_RST    0x0
/// TRIG_CHBW field reset value
#define MACBYP_TRIG_CHBW_RST       0x0

/**
 * @brief Constructs a value for the TRIG_CHBW register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigchbwen - The value to use for the TRIG_CHBW_EN field.
 * @param[in] trigchbw - The value to use for the TRIG_CHBW field.
 */
__INLINE void macbyp_trig_chbw_pack(uint8_t trigchbwen, uint8_t trigchbw)
{
    ASSERT_ERR((((uint32_t)trigchbwen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigchbw << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MACBYP_TRIG_CHBW_ADDR,  ((uint32_t)trigchbwen << 31) | ((uint32_t)trigchbw << 0));
}

/**
 * @brief Unpacks TRIG_CHBW's fields from current value of the TRIG_CHBW register.
 *
 * Reads the TRIG_CHBW register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigchbwen - Will be populated with the current value of this field from the register.
 * @param[out] trigchbw - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_chbw_unpack(uint8_t* trigchbwen, uint8_t* trigchbw)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_CHBW_ADDR);

    *trigchbwen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigchbw = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_CHBW_EN field in the TRIG_CHBW register.
 *
 * The TRIG_CHBW register will be read and the TRIG_CHBW_EN field's value will be returned.
 *
 * @return The current value of the TRIG_CHBW_EN field in the TRIG_CHBW register.
 */
__INLINE uint8_t macbyp_trig_chbw_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_CHBW_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_CHBW_EN field of the TRIG_CHBW register.
 *
 * The TRIG_CHBW register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigchbwen - The value to set the field to.
 */
__INLINE void macbyp_trig_chbw_en_setf(uint8_t trigchbwen)
{
    ASSERT_ERR((((uint32_t)trigchbwen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_CHBW_ADDR, (REG_PL_RD(MACBYP_TRIG_CHBW_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigchbwen << 31));
}

/**
 * @brief Returns the current value of the TRIG_CHBW field in the TRIG_CHBW register.
 *
 * The TRIG_CHBW register will be read and the TRIG_CHBW field's value will be returned.
 *
 * @return The current value of the TRIG_CHBW field in the TRIG_CHBW register.
 */
__INLINE uint8_t macbyp_trig_chbw_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_CHBW_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the TRIG_CHBW field of the TRIG_CHBW register.
 *
 * The TRIG_CHBW register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigchbw - The value to set the field to.
 */
__INLINE void macbyp_trig_chbw_setf(uint8_t trigchbw)
{
    ASSERT_ERR((((uint32_t)trigchbw << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MACBYP_TRIG_CHBW_ADDR, (REG_PL_RD(MACBYP_TRIG_CHBW_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)trigchbw << 0));
}

/// @}

/**
 * @name TRIG_NSTS register definitions
 * <table>
 * <caption id="TRIG_NSTS_BF">TRIG_NSTS bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>      TRIG_NSTS_EN <td>R <td>R/W <td>0
 * <tr><td>02:00 <td>         TRIG_NSTS <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_NSTS register
#define MACBYP_TRIG_NSTS_ADDR   0x01060118
/// Offset of the TRIG_NSTS register from the base address
#define MACBYP_TRIG_NSTS_OFFSET 0x00000118
/// Index of the TRIG_NSTS register
#define MACBYP_TRIG_NSTS_INDEX  0x00000046
/// Reset value of the TRIG_NSTS register
#define MACBYP_TRIG_NSTS_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_NSTS register.
 * The TRIG_NSTS register will be read and its value returned.
 * @return The current value of the TRIG_NSTS register.
 */
__INLINE uint32_t macbyp_trig_nsts_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_NSTS_ADDR);
}

/**
 * @brief Sets the TRIG_NSTS register to a value.
 * The TRIG_NSTS register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_nsts_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_NSTS_ADDR, value);
}

// field definitions
/// TRIG_NSTS_EN field bit
#define MACBYP_TRIG_NSTS_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_NSTS_EN field position
#define MACBYP_TRIG_NSTS_EN_POS    31
/// TRIG_NSTS field mask
#define MACBYP_TRIG_NSTS_MASK      ((uint32_t)0x00000007)
/// TRIG_NSTS field LSB position
#define MACBYP_TRIG_NSTS_LSB       0
/// TRIG_NSTS field width
#define MACBYP_TRIG_NSTS_WIDTH     ((uint32_t)0x00000003)

/// TRIG_NSTS_EN field reset value
#define MACBYP_TRIG_NSTS_EN_RST    0x0
/// TRIG_NSTS field reset value
#define MACBYP_TRIG_NSTS_RST       0x0

/**
 * @brief Constructs a value for the TRIG_NSTS register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trignstsen - The value to use for the TRIG_NSTS_EN field.
 * @param[in] trignsts - The value to use for the TRIG_NSTS field.
 */
__INLINE void macbyp_trig_nsts_pack(uint8_t trignstsen, uint8_t trignsts)
{
    ASSERT_ERR((((uint32_t)trignstsen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trignsts << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(MACBYP_TRIG_NSTS_ADDR,  ((uint32_t)trignstsen << 31) | ((uint32_t)trignsts << 0));
}

/**
 * @brief Unpacks TRIG_NSTS's fields from current value of the TRIG_NSTS register.
 *
 * Reads the TRIG_NSTS register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trignstsen - Will be populated with the current value of this field from the register.
 * @param[out] trignsts - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_nsts_unpack(uint8_t* trignstsen, uint8_t* trignsts)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_NSTS_ADDR);

    *trignstsen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trignsts = (localVal & ((uint32_t)0x00000007)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_NSTS_EN field in the TRIG_NSTS register.
 *
 * The TRIG_NSTS register will be read and the TRIG_NSTS_EN field's value will be returned.
 *
 * @return The current value of the TRIG_NSTS_EN field in the TRIG_NSTS register.
 */
__INLINE uint8_t macbyp_trig_nsts_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_NSTS_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_NSTS_EN field of the TRIG_NSTS register.
 *
 * The TRIG_NSTS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trignstsen - The value to set the field to.
 */
__INLINE void macbyp_trig_nsts_en_setf(uint8_t trignstsen)
{
    ASSERT_ERR((((uint32_t)trignstsen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_NSTS_ADDR, (REG_PL_RD(MACBYP_TRIG_NSTS_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trignstsen << 31));
}

/**
 * @brief Returns the current value of the TRIG_NSTS field in the TRIG_NSTS register.
 *
 * The TRIG_NSTS register will be read and the TRIG_NSTS field's value will be returned.
 *
 * @return The current value of the TRIG_NSTS field in the TRIG_NSTS register.
 */
__INLINE uint8_t macbyp_trig_nsts_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_NSTS_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

/**
 * @brief Sets the TRIG_NSTS field of the TRIG_NSTS register.
 *
 * The TRIG_NSTS register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trignsts - The value to set the field to.
 */
__INLINE void macbyp_trig_nsts_setf(uint8_t trignsts)
{
    ASSERT_ERR((((uint32_t)trignsts << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(MACBYP_TRIG_NSTS_ADDR, (REG_PL_RD(MACBYP_TRIG_NSTS_ADDR) & ~((uint32_t)0x00000007)) | ((uint32_t)trignsts << 0));
}

/// @}

/**
 * @name TRIG_SOUNDING register definitions
 * <table>
 * <caption id="TRIG_SOUNDING_BF">TRIG_SOUNDING bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>  TRIG_SOUNDING_EN <td>R <td>R/W <td>0
 * <tr><td>00 <td>     TRIG_SOUNDING <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_SOUNDING register
#define MACBYP_TRIG_SOUNDING_ADDR   0x0106011C
/// Offset of the TRIG_SOUNDING register from the base address
#define MACBYP_TRIG_SOUNDING_OFFSET 0x0000011C
/// Index of the TRIG_SOUNDING register
#define MACBYP_TRIG_SOUNDING_INDEX  0x00000047
/// Reset value of the TRIG_SOUNDING register
#define MACBYP_TRIG_SOUNDING_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_SOUNDING register.
 * The TRIG_SOUNDING register will be read and its value returned.
 * @return The current value of the TRIG_SOUNDING register.
 */
__INLINE uint32_t macbyp_trig_sounding_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_SOUNDING_ADDR);
}

/**
 * @brief Sets the TRIG_SOUNDING register to a value.
 * The TRIG_SOUNDING register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_sounding_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_SOUNDING_ADDR, value);
}

// field definitions
/// TRIG_SOUNDING_EN field bit
#define MACBYP_TRIG_SOUNDING_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_SOUNDING_EN field position
#define MACBYP_TRIG_SOUNDING_EN_POS    31
/// TRIG_SOUNDING field bit
#define MACBYP_TRIG_SOUNDING_BIT       ((uint32_t)0x00000001)
/// TRIG_SOUNDING field position
#define MACBYP_TRIG_SOUNDING_POS       0

/// TRIG_SOUNDING_EN field reset value
#define MACBYP_TRIG_SOUNDING_EN_RST    0x0
/// TRIG_SOUNDING field reset value
#define MACBYP_TRIG_SOUNDING_RST       0x0

/**
 * @brief Constructs a value for the TRIG_SOUNDING register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigsoundingen - The value to use for the TRIG_SOUNDING_EN field.
 * @param[in] trigsounding - The value to use for the TRIG_SOUNDING field.
 */
__INLINE void macbyp_trig_sounding_pack(uint8_t trigsoundingen, uint8_t trigsounding)
{
    ASSERT_ERR((((uint32_t)trigsoundingen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigsounding << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_SOUNDING_ADDR,  ((uint32_t)trigsoundingen << 31) | ((uint32_t)trigsounding << 0));
}

/**
 * @brief Unpacks TRIG_SOUNDING's fields from current value of the TRIG_SOUNDING register.
 *
 * Reads the TRIG_SOUNDING register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigsoundingen - Will be populated with the current value of this field from the register.
 * @param[out] trigsounding - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_sounding_unpack(uint8_t* trigsoundingen, uint8_t* trigsounding)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SOUNDING_ADDR);

    *trigsoundingen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigsounding = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_SOUNDING_EN field in the TRIG_SOUNDING register.
 *
 * The TRIG_SOUNDING register will be read and the TRIG_SOUNDING_EN field's value will be returned.
 *
 * @return The current value of the TRIG_SOUNDING_EN field in the TRIG_SOUNDING register.
 */
__INLINE uint8_t macbyp_trig_sounding_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SOUNDING_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_SOUNDING_EN field of the TRIG_SOUNDING register.
 *
 * The TRIG_SOUNDING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigsoundingen - The value to set the field to.
 */
__INLINE void macbyp_trig_sounding_en_setf(uint8_t trigsoundingen)
{
    ASSERT_ERR((((uint32_t)trigsoundingen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_SOUNDING_ADDR, (REG_PL_RD(MACBYP_TRIG_SOUNDING_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigsoundingen << 31));
}

/**
 * @brief Returns the current value of the TRIG_SOUNDING field in the TRIG_SOUNDING register.
 *
 * The TRIG_SOUNDING register will be read and the TRIG_SOUNDING field's value will be returned.
 *
 * @return The current value of the TRIG_SOUNDING field in the TRIG_SOUNDING register.
 */
__INLINE uint8_t macbyp_trig_sounding_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SOUNDING_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the TRIG_SOUNDING field of the TRIG_SOUNDING register.
 *
 * The TRIG_SOUNDING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigsounding - The value to set the field to.
 */
__INLINE void macbyp_trig_sounding_setf(uint8_t trigsounding)
{
    ASSERT_ERR((((uint32_t)trigsounding << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_SOUNDING_ADDR, (REG_PL_RD(MACBYP_TRIG_SOUNDING_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)trigsounding << 0));
}

/// @}

/**
 * @name TRIG_PARTIALAID register definitions
 * <table>
 * <caption id="TRIG_PARTIALAID_BF">TRIG_PARTIALAID bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>TRIG_PARTIALAID_EN <td>R <td>R/W <td>0
 * <tr><td>08:00 <td>   TRIG_PARTIALAID <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_PARTIALAID register
#define MACBYP_TRIG_PARTIALAID_ADDR   0x01060120
/// Offset of the TRIG_PARTIALAID register from the base address
#define MACBYP_TRIG_PARTIALAID_OFFSET 0x00000120
/// Index of the TRIG_PARTIALAID register
#define MACBYP_TRIG_PARTIALAID_INDEX  0x00000048
/// Reset value of the TRIG_PARTIALAID register
#define MACBYP_TRIG_PARTIALAID_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_PARTIALAID register.
 * The TRIG_PARTIALAID register will be read and its value returned.
 * @return The current value of the TRIG_PARTIALAID register.
 */
__INLINE uint32_t macbyp_trig_partialaid_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_PARTIALAID_ADDR);
}

/**
 * @brief Sets the TRIG_PARTIALAID register to a value.
 * The TRIG_PARTIALAID register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_partialaid_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_PARTIALAID_ADDR, value);
}

// field definitions
/// TRIG_PARTIALAID_EN field bit
#define MACBYP_TRIG_PARTIALAID_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_PARTIALAID_EN field position
#define MACBYP_TRIG_PARTIALAID_EN_POS    31
/// TRIG_PARTIALAID field mask
#define MACBYP_TRIG_PARTIALAID_MASK      ((uint32_t)0x000001FF)
/// TRIG_PARTIALAID field LSB position
#define MACBYP_TRIG_PARTIALAID_LSB       0
/// TRIG_PARTIALAID field width
#define MACBYP_TRIG_PARTIALAID_WIDTH     ((uint32_t)0x00000009)

/// TRIG_PARTIALAID_EN field reset value
#define MACBYP_TRIG_PARTIALAID_EN_RST    0x0
/// TRIG_PARTIALAID field reset value
#define MACBYP_TRIG_PARTIALAID_RST       0x0

/**
 * @brief Constructs a value for the TRIG_PARTIALAID register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigpartialaiden - The value to use for the TRIG_PARTIALAID_EN field.
 * @param[in] trigpartialaid - The value to use for the TRIG_PARTIALAID field.
 */
__INLINE void macbyp_trig_partialaid_pack(uint8_t trigpartialaiden, uint16_t trigpartialaid)
{
    ASSERT_ERR((((uint32_t)trigpartialaiden << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigpartialaid << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MACBYP_TRIG_PARTIALAID_ADDR,  ((uint32_t)trigpartialaiden << 31) | ((uint32_t)trigpartialaid << 0));
}

/**
 * @brief Unpacks TRIG_PARTIALAID's fields from current value of the TRIG_PARTIALAID register.
 *
 * Reads the TRIG_PARTIALAID register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigpartialaiden - Will be populated with the current value of this field from the register.
 * @param[out] trigpartialaid - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_partialaid_unpack(uint8_t* trigpartialaiden, uint16_t* trigpartialaid)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_PARTIALAID_ADDR);

    *trigpartialaiden = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigpartialaid = (localVal & ((uint32_t)0x000001FF)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_PARTIALAID_EN field in the TRIG_PARTIALAID register.
 *
 * The TRIG_PARTIALAID register will be read and the TRIG_PARTIALAID_EN field's value will be returned.
 *
 * @return The current value of the TRIG_PARTIALAID_EN field in the TRIG_PARTIALAID register.
 */
__INLINE uint8_t macbyp_trig_partialaid_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_PARTIALAID_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_PARTIALAID_EN field of the TRIG_PARTIALAID register.
 *
 * The TRIG_PARTIALAID register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigpartialaiden - The value to set the field to.
 */
__INLINE void macbyp_trig_partialaid_en_setf(uint8_t trigpartialaiden)
{
    ASSERT_ERR((((uint32_t)trigpartialaiden << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_PARTIALAID_ADDR, (REG_PL_RD(MACBYP_TRIG_PARTIALAID_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigpartialaiden << 31));
}

/**
 * @brief Returns the current value of the TRIG_PARTIALAID field in the TRIG_PARTIALAID register.
 *
 * The TRIG_PARTIALAID register will be read and the TRIG_PARTIALAID field's value will be returned.
 *
 * @return The current value of the TRIG_PARTIALAID field in the TRIG_PARTIALAID register.
 */
__INLINE uint16_t macbyp_trig_partialaid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_PARTIALAID_ADDR);
    return ((localVal & ((uint32_t)0x000001FF)) >> 0);
}

/**
 * @brief Sets the TRIG_PARTIALAID field of the TRIG_PARTIALAID register.
 *
 * The TRIG_PARTIALAID register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigpartialaid - The value to set the field to.
 */
__INLINE void macbyp_trig_partialaid_setf(uint16_t trigpartialaid)
{
    ASSERT_ERR((((uint32_t)trigpartialaid << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MACBYP_TRIG_PARTIALAID_ADDR, (REG_PL_RD(MACBYP_TRIG_PARTIALAID_ADDR) & ~((uint32_t)0x000001FF)) | ((uint32_t)trigpartialaid << 0));
}

/// @}

/**
 * @name TRIG_GROUPID register definitions
 * <table>
 * <caption id="TRIG_GROUPID_BF">TRIG_GROUPID bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td>   TRIG_GROUPID_EN <td>R <td>R/W <td>0
 * <tr><td>05:00 <td>      TRIG_GROUPID <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_GROUPID register
#define MACBYP_TRIG_GROUPID_ADDR   0x01060124
/// Offset of the TRIG_GROUPID register from the base address
#define MACBYP_TRIG_GROUPID_OFFSET 0x00000124
/// Index of the TRIG_GROUPID register
#define MACBYP_TRIG_GROUPID_INDEX  0x00000049
/// Reset value of the TRIG_GROUPID register
#define MACBYP_TRIG_GROUPID_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_GROUPID register.
 * The TRIG_GROUPID register will be read and its value returned.
 * @return The current value of the TRIG_GROUPID register.
 */
__INLINE uint32_t macbyp_trig_groupid_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_GROUPID_ADDR);
}

/**
 * @brief Sets the TRIG_GROUPID register to a value.
 * The TRIG_GROUPID register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_groupid_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_GROUPID_ADDR, value);
}

// field definitions
/// TRIG_GROUPID_EN field bit
#define MACBYP_TRIG_GROUPID_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_GROUPID_EN field position
#define MACBYP_TRIG_GROUPID_EN_POS    31
/// TRIG_GROUPID field mask
#define MACBYP_TRIG_GROUPID_MASK      ((uint32_t)0x0000003F)
/// TRIG_GROUPID field LSB position
#define MACBYP_TRIG_GROUPID_LSB       0
/// TRIG_GROUPID field width
#define MACBYP_TRIG_GROUPID_WIDTH     ((uint32_t)0x00000006)

/// TRIG_GROUPID_EN field reset value
#define MACBYP_TRIG_GROUPID_EN_RST    0x0
/// TRIG_GROUPID field reset value
#define MACBYP_TRIG_GROUPID_RST       0x0

/**
 * @brief Constructs a value for the TRIG_GROUPID register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] triggroupiden - The value to use for the TRIG_GROUPID_EN field.
 * @param[in] triggroupid - The value to use for the TRIG_GROUPID field.
 */
__INLINE void macbyp_trig_groupid_pack(uint8_t triggroupiden, uint8_t triggroupid)
{
    ASSERT_ERR((((uint32_t)triggroupiden << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)triggroupid << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MACBYP_TRIG_GROUPID_ADDR,  ((uint32_t)triggroupiden << 31) | ((uint32_t)triggroupid << 0));
}

/**
 * @brief Unpacks TRIG_GROUPID's fields from current value of the TRIG_GROUPID register.
 *
 * Reads the TRIG_GROUPID register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] triggroupiden - Will be populated with the current value of this field from the register.
 * @param[out] triggroupid - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_groupid_unpack(uint8_t* triggroupiden, uint8_t* triggroupid)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_GROUPID_ADDR);

    *triggroupiden = (localVal & ((uint32_t)0x80000000)) >> 31;
    *triggroupid = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_GROUPID_EN field in the TRIG_GROUPID register.
 *
 * The TRIG_GROUPID register will be read and the TRIG_GROUPID_EN field's value will be returned.
 *
 * @return The current value of the TRIG_GROUPID_EN field in the TRIG_GROUPID register.
 */
__INLINE uint8_t macbyp_trig_groupid_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_GROUPID_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_GROUPID_EN field of the TRIG_GROUPID register.
 *
 * The TRIG_GROUPID register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggroupiden - The value to set the field to.
 */
__INLINE void macbyp_trig_groupid_en_setf(uint8_t triggroupiden)
{
    ASSERT_ERR((((uint32_t)triggroupiden << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_GROUPID_ADDR, (REG_PL_RD(MACBYP_TRIG_GROUPID_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)triggroupiden << 31));
}

/**
 * @brief Returns the current value of the TRIG_GROUPID field in the TRIG_GROUPID register.
 *
 * The TRIG_GROUPID register will be read and the TRIG_GROUPID field's value will be returned.
 *
 * @return The current value of the TRIG_GROUPID field in the TRIG_GROUPID register.
 */
__INLINE uint8_t macbyp_trig_groupid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_GROUPID_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the TRIG_GROUPID field of the TRIG_GROUPID register.
 *
 * The TRIG_GROUPID register will be read, modified to contain the new field value, and written.
 *
 * @param[in] triggroupid - The value to set the field to.
 */
__INLINE void macbyp_trig_groupid_setf(uint8_t triggroupid)
{
    ASSERT_ERR((((uint32_t)triggroupid << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MACBYP_TRIG_GROUPID_ADDR, (REG_PL_RD(MACBYP_TRIG_GROUPID_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)triggroupid << 0));
}

/// @}

/**
 * @name TRIG_SMOOTHING register definitions
 * <table>
 * <caption id="TRIG_SMOOTHING_BF">TRIG_SMOOTHING bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31 <td> TRIG_SMOOTHING_EN <td>R <td>R/W <td>0
 * <tr><td>00 <td>    TRIG_SMOOTHING <td>R <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the TRIG_SMOOTHING register
#define MACBYP_TRIG_SMOOTHING_ADDR   0x01060128
/// Offset of the TRIG_SMOOTHING register from the base address
#define MACBYP_TRIG_SMOOTHING_OFFSET 0x00000128
/// Index of the TRIG_SMOOTHING register
#define MACBYP_TRIG_SMOOTHING_INDEX  0x0000004A
/// Reset value of the TRIG_SMOOTHING register
#define MACBYP_TRIG_SMOOTHING_RESET  0x00000000

/**
 * @brief Returns the current value of the TRIG_SMOOTHING register.
 * The TRIG_SMOOTHING register will be read and its value returned.
 * @return The current value of the TRIG_SMOOTHING register.
 */
__INLINE uint32_t macbyp_trig_smoothing_get(void)
{
    return REG_PL_RD(MACBYP_TRIG_SMOOTHING_ADDR);
}

/**
 * @brief Sets the TRIG_SMOOTHING register to a value.
 * The TRIG_SMOOTHING register will be written.
 * @param value - The value to write.
 */
__INLINE void macbyp_trig_smoothing_set(uint32_t value)
{
    REG_PL_WR(MACBYP_TRIG_SMOOTHING_ADDR, value);
}

// field definitions
/// TRIG_SMOOTHING_EN field bit
#define MACBYP_TRIG_SMOOTHING_EN_BIT    ((uint32_t)0x80000000)
/// TRIG_SMOOTHING_EN field position
#define MACBYP_TRIG_SMOOTHING_EN_POS    31
/// TRIG_SMOOTHING field bit
#define MACBYP_TRIG_SMOOTHING_BIT       ((uint32_t)0x00000001)
/// TRIG_SMOOTHING field position
#define MACBYP_TRIG_SMOOTHING_POS       0

/// TRIG_SMOOTHING_EN field reset value
#define MACBYP_TRIG_SMOOTHING_EN_RST    0x0
/// TRIG_SMOOTHING field reset value
#define MACBYP_TRIG_SMOOTHING_RST       0x0

/**
 * @brief Constructs a value for the TRIG_SMOOTHING register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] trigsmoothingen - The value to use for the TRIG_SMOOTHING_EN field.
 * @param[in] trigsmoothing - The value to use for the TRIG_SMOOTHING field.
 */
__INLINE void macbyp_trig_smoothing_pack(uint8_t trigsmoothingen, uint8_t trigsmoothing)
{
    ASSERT_ERR((((uint32_t)trigsmoothingen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)trigsmoothing << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_SMOOTHING_ADDR,  ((uint32_t)trigsmoothingen << 31) | ((uint32_t)trigsmoothing << 0));
}

/**
 * @brief Unpacks TRIG_SMOOTHING's fields from current value of the TRIG_SMOOTHING register.
 *
 * Reads the TRIG_SMOOTHING register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] trigsmoothingen - Will be populated with the current value of this field from the register.
 * @param[out] trigsmoothing - Will be populated with the current value of this field from the register.
 */
__INLINE void macbyp_trig_smoothing_unpack(uint8_t* trigsmoothingen, uint8_t* trigsmoothing)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SMOOTHING_ADDR);

    *trigsmoothingen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *trigsmoothing = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TRIG_SMOOTHING_EN field in the TRIG_SMOOTHING register.
 *
 * The TRIG_SMOOTHING register will be read and the TRIG_SMOOTHING_EN field's value will be returned.
 *
 * @return The current value of the TRIG_SMOOTHING_EN field in the TRIG_SMOOTHING register.
 */
__INLINE uint8_t macbyp_trig_smoothing_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SMOOTHING_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TRIG_SMOOTHING_EN field of the TRIG_SMOOTHING register.
 *
 * The TRIG_SMOOTHING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigsmoothingen - The value to set the field to.
 */
__INLINE void macbyp_trig_smoothing_en_setf(uint8_t trigsmoothingen)
{
    ASSERT_ERR((((uint32_t)trigsmoothingen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MACBYP_TRIG_SMOOTHING_ADDR, (REG_PL_RD(MACBYP_TRIG_SMOOTHING_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)trigsmoothingen << 31));
}

/**
 * @brief Returns the current value of the TRIG_SMOOTHING field in the TRIG_SMOOTHING register.
 *
 * The TRIG_SMOOTHING register will be read and the TRIG_SMOOTHING field's value will be returned.
 *
 * @return The current value of the TRIG_SMOOTHING field in the TRIG_SMOOTHING register.
 */
__INLINE uint8_t macbyp_trig_smoothing_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_TRIG_SMOOTHING_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the TRIG_SMOOTHING field of the TRIG_SMOOTHING register.
 *
 * The TRIG_SMOOTHING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] trigsmoothing - The value to set the field to.
 */
__INLINE void macbyp_trig_smoothing_setf(uint8_t trigsmoothing)
{
    ASSERT_ERR((((uint32_t)trigsmoothing << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MACBYP_TRIG_SMOOTHING_ADDR, (REG_PL_RD(MACBYP_TRIG_SMOOTHING_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)trigsmoothing << 0));
}

/// @}

/**
 * @name TXV register definitions
 * <table>
 * <caption id="TXV_BF">TXV bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>07:00 <td>               TXV <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the TXV register
#define MACBYP_TXV_ADDR   0x01060200
/// Offset of the TXV register from the base address
#define MACBYP_TXV_OFFSET 0x00000200
/// Index of the TXV register
#define MACBYP_TXV_INDEX  0x00000080
/// Reset value of the TXV register
#define MACBYP_TXV_RESET  0x00000000
/// Number of elements of the TXV register array
#define MACBYP_TXV_COUNT  16

/**
 * @brief Returns the current value of the TXV register.
 * The TXV register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the TXV register.
 */
__INLINE uint32_t macbyp_txv_get(int reg_idx)
{
    //ASSERT_ERR(reg_idx <= 15);
    return REG_PL_RD(MACBYP_TXV_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the TXV register to a value.
 * The TXV register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void macbyp_txv_set(int reg_idx, uint32_t value)
{
    //ASSERT_ERR(reg_idx <= 15);
    REG_PL_WR(MACBYP_TXV_ADDR + reg_idx * 4, value);
}

// field definitions
/// TXV field mask
#define MACBYP_TXV_MASK   ((uint32_t)0x000000FF)
/// TXV field LSB position
#define MACBYP_TXV_LSB    0
/// TXV field width
#define MACBYP_TXV_WIDTH  ((uint32_t)0x00000008)

/// TXV field reset value
#define MACBYP_TXV_RST    0x0

/**
 * @brief Returns the current value of the TXV field in the TXV register.
 *
 * The TXV register will be read and the TXV field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the TXV field in the TXV register.
 */
__INLINE uint8_t macbyp_txv_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 15);
    uint32_t localVal = REG_PL_RD(MACBYP_TXV_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the TXV field of the TXV register.
 *
 * The TXV register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] txv - The value to set the field to.
 */
__INLINE void macbyp_txv_setf(int reg_idx, uint8_t txv)
{
    ASSERT_ERR(reg_idx <= 15);
    ASSERT_ERR((((uint32_t)txv << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MACBYP_TXV_ADDR + reg_idx * 4, (uint32_t)txv << 0);
}

/// @}

/**
 * @name VERSION register definitions
 * <table>
 * <caption id="VERSION_BF">VERSION bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>15:00 <td>           VERSION <td>R <td>R <td>0x2
 * </table>
 *
 * @{
 */

/// Address of the VERSION register
#define MACBYP_VERSION_ADDR   0x010603FC
/// Offset of the VERSION register from the base address
#define MACBYP_VERSION_OFFSET 0x000003FC
/// Index of the VERSION register
#define MACBYP_VERSION_INDEX  0x000000FF
/// Reset value of the VERSION register
#define MACBYP_VERSION_RESET  0x00000002

/**
 * @brief Returns the current value of the VERSION register.
 * The VERSION register will be read and its value returned.
 * @return The current value of the VERSION register.
 */
__INLINE uint32_t macbyp_version_get(void)
{
    return REG_PL_RD(MACBYP_VERSION_ADDR);
}

// field definitions
/// VERSION field mask
#define MACBYP_VERSION_MASK   ((uint32_t)0x0000FFFF)
/// VERSION field LSB position
#define MACBYP_VERSION_LSB    0
/// VERSION field width
#define MACBYP_VERSION_WIDTH  ((uint32_t)0x00000010)

/// VERSION field reset value
#define MACBYP_VERSION_RST    0x2

/**
 * @brief Returns the current value of the VERSION field in the VERSION register.
 *
 * The VERSION register will be read and the VERSION field's value will be returned.
 *
 * @return The current value of the VERSION field in the VERSION register.
 */
__INLINE uint16_t macbyp_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(MACBYP_VERSION_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/// @}


#endif // _REG_MACBYPASS_H_

/// @}

