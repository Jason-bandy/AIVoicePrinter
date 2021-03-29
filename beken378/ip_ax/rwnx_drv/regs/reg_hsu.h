/**
 * @file reg_hsu.h
 * @brief Definitions of the HSU HW block registers and register access functions.
 *
 * @defgroup REG_HSU REG_HSU
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the HSU HW block registers and register access functions.
 */
#ifndef _REG_HSU_H_
#define _REG_HSU_H_

#include "co_int.h"
#include "_reg_hsu.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_HSU peripheral.
 */
#define REG_HSU_COUNT 14

/** @brief Decoding mask of the REG_HSU peripheral registers from the CPU point of view.
 */
#define REG_HSU_DECODING_MASK 0x0000003F

/**
 * @name REVISION register definitions
 * <table>
 * <caption id="REVISION_BF">REVISION bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>24 <td>HMAC_SHA512_SHA384 <td>W <td>R <td>0
 * <tr><td>23 <td>HMAC_SHA256_SHA224 <td>W <td>R <td>1
 * <tr><td>22 <td>         HMAC_SHA1 <td>W <td>R <td>1
 * <tr><td>21 <td>       SHA_512_384 <td>W <td>R <td>0
 * <tr><td>20 <td>       SHA_256_224 <td>W <td>R <td>1
 * <tr><td>19 <td>             SHA_1 <td>W <td>R <td>1
 * <tr><td>18 <td>            IP_CHK <td>W <td>R <td>1
 * <tr><td>17 <td>      AES_128_CMAC <td>W <td>R <td>1
 * <tr><td>16 <td>          TKIP_MIC <td>W <td>R <td>1
 * <tr><td>15:00 <td>           VERSION <td>W <td>R <td>0x7
 * </table>
 *
 * @{
 */

/// Address of the REVISION register
#define HSU_REVISION_ADDR   0xC0300000
/// Offset of the REVISION register from the base address
#define HSU_REVISION_OFFSET 0x00000000
/// Index of the REVISION register
#define HSU_REVISION_INDEX  0x00000000
/// Reset value of the REVISION register
#define HSU_REVISION_RESET  0x00DF0007

/**
 * @brief Returns the current value of the REVISION register.
 * The REVISION register will be read and its value returned.
 * @return The current value of the REVISION register.
 */
__INLINE uint32_t hsu_revision_get(void)
{
    return REG_PL_RD(HSU_REVISION_ADDR);
}

/**
 * @brief Sets the REVISION register to a value.
 * The REVISION register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_revision_set(uint32_t value)
{
    REG_PL_WR(HSU_REVISION_ADDR, value);
}

// field definitions
/// HMAC_SHA512_SHA384 field bit
#define HSU_HMAC_SHA512_SHA384_BIT    ((uint32_t)0x01000000)
/// HMAC_SHA512_SHA384 field position
#define HSU_HMAC_SHA512_SHA384_POS    24
/// HMAC_SHA256_SHA224 field bit
#define HSU_HMAC_SHA256_SHA224_BIT    ((uint32_t)0x00800000)
/// HMAC_SHA256_SHA224 field position
#define HSU_HMAC_SHA256_SHA224_POS    23
/// HMAC_SHA1 field bit
#define HSU_HMAC_SHA1_BIT             ((uint32_t)0x00400000)
/// HMAC_SHA1 field position
#define HSU_HMAC_SHA1_POS             22
/// SHA_512_384 field bit
#define HSU_SHA_512_384_BIT           ((uint32_t)0x00200000)
/// SHA_512_384 field position
#define HSU_SHA_512_384_POS           21
/// SHA_256_224 field bit
#define HSU_SHA_256_224_BIT           ((uint32_t)0x00100000)
/// SHA_256_224 field position
#define HSU_SHA_256_224_POS           20
/// SHA_1 field bit
#define HSU_SHA_1_BIT                 ((uint32_t)0x00080000)
/// SHA_1 field position
#define HSU_SHA_1_POS                 19
/// IP_CHK field bit
#define HSU_IP_CHK_BIT                ((uint32_t)0x00040000)
/// IP_CHK field position
#define HSU_IP_CHK_POS                18
/// AES_128_CMAC field bit
#define HSU_AES_128_CMAC_BIT          ((uint32_t)0x00020000)
/// AES_128_CMAC field position
#define HSU_AES_128_CMAC_POS          17
/// TKIP_MIC field bit
#define HSU_TKIP_MIC_BIT              ((uint32_t)0x00010000)
/// TKIP_MIC field position
#define HSU_TKIP_MIC_POS              16
/// VERSION field mask
#define HSU_VERSION_MASK              ((uint32_t)0x0000FFFF)
/// VERSION field LSB position
#define HSU_VERSION_LSB               0
/// VERSION field width
#define HSU_VERSION_WIDTH             ((uint32_t)0x00000010)

/// HMAC_SHA512_SHA384 field reset value
#define HSU_HMAC_SHA512_SHA384_RST    0x0
/// HMAC_SHA256_SHA224 field reset value
#define HSU_HMAC_SHA256_SHA224_RST    0x1
/// HMAC_SHA1 field reset value
#define HSU_HMAC_SHA1_RST             0x1
/// SHA_512_384 field reset value
#define HSU_SHA_512_384_RST           0x0
/// SHA_256_224 field reset value
#define HSU_SHA_256_224_RST           0x1
/// SHA_1 field reset value
#define HSU_SHA_1_RST                 0x1
/// IP_CHK field reset value
#define HSU_IP_CHK_RST                0x1
/// AES_128_CMAC field reset value
#define HSU_AES_128_CMAC_RST          0x1
/// TKIP_MIC field reset value
#define HSU_TKIP_MIC_RST              0x1
/// VERSION field reset value
#define HSU_VERSION_RST               0x7

/**
 * @brief Unpacks REVISION's fields from current value of the REVISION register.
 *
 * Reads the REVISION register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] hmacsha512sha384 - Will be populated with the current value of this field from the register.
 * @param[out] hmacsha256sha224 - Will be populated with the current value of this field from the register.
 * @param[out] hmacsha1 - Will be populated with the current value of this field from the register.
 * @param[out] sha512384 - Will be populated with the current value of this field from the register.
 * @param[out] sha256224 - Will be populated with the current value of this field from the register.
 * @param[out] sha1 - Will be populated with the current value of this field from the register.
 * @param[out] ipchk - Will be populated with the current value of this field from the register.
 * @param[out] aes128cmac - Will be populated with the current value of this field from the register.
 * @param[out] tkipmic - Will be populated with the current value of this field from the register.
 * @param[out] version - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_revision_unpack(uint8_t* hmacsha512sha384, uint8_t* hmacsha256sha224, uint8_t* hmacsha1, uint8_t* sha512384, uint8_t* sha256224, uint8_t* sha1, uint8_t* ipchk, uint8_t* aes128cmac, uint8_t* tkipmic, uint16_t* version)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);

    *hmacsha512sha384 = (localVal & ((uint32_t)0x01000000)) >> 24;
    *hmacsha256sha224 = (localVal & ((uint32_t)0x00800000)) >> 23;
    *hmacsha1 = (localVal & ((uint32_t)0x00400000)) >> 22;
    *sha512384 = (localVal & ((uint32_t)0x00200000)) >> 21;
    *sha256224 = (localVal & ((uint32_t)0x00100000)) >> 20;
    *sha1 = (localVal & ((uint32_t)0x00080000)) >> 19;
    *ipchk = (localVal & ((uint32_t)0x00040000)) >> 18;
    *aes128cmac = (localVal & ((uint32_t)0x00020000)) >> 17;
    *tkipmic = (localVal & ((uint32_t)0x00010000)) >> 16;
    *version = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the HMAC_SHA512_SHA384 field in the REVISION register.
 *
 * The REVISION register will be read and the HMAC_SHA512_SHA384 field's value will be returned.
 *
 * @return The current value of the HMAC_SHA512_SHA384 field in the REVISION register.
 */
__INLINE uint8_t hsu_hmac_sha512_sha384_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Returns the current value of the HMAC_SHA256_SHA224 field in the REVISION register.
 *
 * The REVISION register will be read and the HMAC_SHA256_SHA224 field's value will be returned.
 *
 * @return The current value of the HMAC_SHA256_SHA224 field in the REVISION register.
 */
__INLINE uint8_t hsu_hmac_sha256_sha224_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Returns the current value of the HMAC_SHA1 field in the REVISION register.
 *
 * The REVISION register will be read and the HMAC_SHA1 field's value will be returned.
 *
 * @return The current value of the HMAC_SHA1 field in the REVISION register.
 */
__INLINE uint8_t hsu_hmac_sha1_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Returns the current value of the SHA_512_384 field in the REVISION register.
 *
 * The REVISION register will be read and the SHA_512_384 field's value will be returned.
 *
 * @return The current value of the SHA_512_384 field in the REVISION register.
 */
__INLINE uint8_t hsu_sha_512_384_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Returns the current value of the SHA_256_224 field in the REVISION register.
 *
 * The REVISION register will be read and the SHA_256_224 field's value will be returned.
 *
 * @return The current value of the SHA_256_224 field in the REVISION register.
 */
__INLINE uint8_t hsu_sha_256_224_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the SHA_1 field in the REVISION register.
 *
 * The REVISION register will be read and the SHA_1 field's value will be returned.
 *
 * @return The current value of the SHA_1 field in the REVISION register.
 */
__INLINE uint8_t hsu_sha_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Returns the current value of the IP_CHK field in the REVISION register.
 *
 * The REVISION register will be read and the IP_CHK field's value will be returned.
 *
 * @return The current value of the IP_CHK field in the REVISION register.
 */
__INLINE uint8_t hsu_ip_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Returns the current value of the AES_128_CMAC field in the REVISION register.
 *
 * The REVISION register will be read and the AES_128_CMAC field's value will be returned.
 *
 * @return The current value of the AES_128_CMAC field in the REVISION register.
 */
__INLINE uint8_t hsu_aes_128_cmac_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Returns the current value of the TKIP_MIC field in the REVISION register.
 *
 * The REVISION register will be read and the TKIP_MIC field's value will be returned.
 *
 * @return The current value of the TKIP_MIC field in the REVISION register.
 */
__INLINE uint8_t hsu_tkip_mic_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Returns the current value of the VERSION field in the REVISION register.
 *
 * The REVISION register will be read and the VERSION field's value will be returned.
 *
 * @return The current value of the VERSION field in the REVISION register.
 */
__INLINE uint16_t hsu_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name CONTROL register definitions
 * <table>
 * <caption id="CONTROL_BF">CONTROL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>12:08 <td>              MODE <td>R <td>R/W <td>0x0
 * <tr><td>05 <td>       LAST_BUFFER <td>R <td>R/W <td>0
 * <tr><td>04 <td>      FIRST_BUFFER <td>R <td>R/W <td>0
 * <tr><td>00 <td>             START <td>R/W <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the CONTROL register
#define HSU_CONTROL_ADDR   0xC0300004
/// Offset of the CONTROL register from the base address
#define HSU_CONTROL_OFFSET 0x00000004
/// Index of the CONTROL register
#define HSU_CONTROL_INDEX  0x00000001
/// Reset value of the CONTROL register
#define HSU_CONTROL_RESET  0x00000000

/**
 * @brief Returns the current value of the CONTROL register.
 * The CONTROL register will be read and its value returned.
 * @return The current value of the CONTROL register.
 */
__INLINE uint32_t hsu_control_get(void)
{
    return REG_PL_RD(HSU_CONTROL_ADDR);
}

/**
 * @brief Sets the CONTROL register to a value.
 * The CONTROL register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_control_set(uint32_t value)
{
    REG_PL_WR(HSU_CONTROL_ADDR, value);
}

// field definitions
/// MODE field mask
#define HSU_MODE_MASK           ((uint32_t)0x00001F00)
/// MODE field LSB position
#define HSU_MODE_LSB            8
/// MODE field width
#define HSU_MODE_WIDTH          ((uint32_t)0x00000005)
/// LAST_BUFFER field bit
#define HSU_LAST_BUFFER_BIT     ((uint32_t)0x00000020)
/// LAST_BUFFER field position
#define HSU_LAST_BUFFER_POS     5
/// FIRST_BUFFER field bit
#define HSU_FIRST_BUFFER_BIT    ((uint32_t)0x00000010)
/// FIRST_BUFFER field position
#define HSU_FIRST_BUFFER_POS    4
/// START field bit
#define HSU_START_BIT           ((uint32_t)0x00000001)
/// START field position
#define HSU_START_POS           0

/// MODE field reset value
#define HSU_MODE_RST            0x0
/// LAST_BUFFER field reset value
#define HSU_LAST_BUFFER_RST     0x0
/// FIRST_BUFFER field reset value
#define HSU_FIRST_BUFFER_RST    0x0
/// START field reset value
#define HSU_START_RST           0x0

/**
 * @brief Constructs a value for the CONTROL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mode - The value to use for the MODE field.
 * @param[in] lastbuffer - The value to use for the LAST_BUFFER field.
 * @param[in] firstbuffer - The value to use for the FIRST_BUFFER field.
 * @param[in] start - The value to use for the START field.
 */
__INLINE void hsu_control_pack(uint8_t mode, uint8_t lastbuffer, uint8_t firstbuffer, uint8_t start)
{
    ASSERT_ERR((((uint32_t)mode << 8) & ~((uint32_t)0x00001F00)) == 0);
    ASSERT_ERR((((uint32_t)lastbuffer << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)firstbuffer << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)start << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR,  ((uint32_t)mode << 8) | ((uint32_t)lastbuffer << 5) | ((uint32_t)firstbuffer << 4) | ((uint32_t)start << 0));
}

/**
 * @brief Unpacks CONTROL's fields from current value of the CONTROL register.
 *
 * Reads the CONTROL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mode - Will be populated with the current value of this field from the register.
 * @param[out] lastbuffer - Will be populated with the current value of this field from the register.
 * @param[out] firstbuffer - Will be populated with the current value of this field from the register.
 * @param[out] start - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_control_unpack(uint8_t* mode, uint8_t* lastbuffer, uint8_t* firstbuffer, uint8_t* start)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);

    *mode = (localVal & ((uint32_t)0x00001F00)) >> 8;
    *lastbuffer = (localVal & ((uint32_t)0x00000020)) >> 5;
    *firstbuffer = (localVal & ((uint32_t)0x00000010)) >> 4;
    *start = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the MODE field in the CONTROL register.
 *
 * The CONTROL register will be read and the MODE field's value will be returned.
 *
 * @return The current value of the MODE field in the CONTROL register.
 */
__INLINE uint8_t hsu_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00001F00)) >> 8);
}

/**
 * @brief Sets the MODE field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mode - The value to set the field to.
 */
__INLINE void hsu_mode_setf(uint8_t mode)
{
    ASSERT_ERR((((uint32_t)mode << 8) & ~((uint32_t)0x00001F00)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00001F00)) | ((uint32_t)mode << 8));
}

/**
 * @brief Returns the current value of the LAST_BUFFER field in the CONTROL register.
 *
 * The CONTROL register will be read and the LAST_BUFFER field's value will be returned.
 *
 * @return The current value of the LAST_BUFFER field in the CONTROL register.
 */
__INLINE uint8_t hsu_last_buffer_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the LAST_BUFFER field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lastbuffer - The value to set the field to.
 */
__INLINE void hsu_last_buffer_setf(uint8_t lastbuffer)
{
    ASSERT_ERR((((uint32_t)lastbuffer << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)lastbuffer << 5));
}

/**
 * @brief Returns the current value of the FIRST_BUFFER field in the CONTROL register.
 *
 * The CONTROL register will be read and the FIRST_BUFFER field's value will be returned.
 *
 * @return The current value of the FIRST_BUFFER field in the CONTROL register.
 */
__INLINE uint8_t hsu_first_buffer_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the FIRST_BUFFER field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] firstbuffer - The value to set the field to.
 */
__INLINE void hsu_first_buffer_setf(uint8_t firstbuffer)
{
    ASSERT_ERR((((uint32_t)firstbuffer << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)firstbuffer << 4));
}

/**
 * @brief Returns the current value of the START field in the CONTROL register.
 *
 * The CONTROL register will be read and the START field's value will be returned.
 *
 * @return The current value of the START field in the CONTROL register.
 */
__INLINE uint8_t hsu_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the START field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] start - The value to set the field to.
 */
__INLINE void hsu_start_setf(uint8_t start)
{
    ASSERT_ERR((((uint32_t)start << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)start << 0));
}

/// @}

/**
 * @name STATUS_SET register definitions
 * <table>
 * <caption id="STATUS_SET_BF">STATUS_SET bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>00 <td>          DONE_SET <td>W <td>R <td>0
 * </table>
 *
 * @{
 */

/// Address of the STATUS_SET register
#define HSU_STATUS_SET_ADDR   0xC0300008
/// Offset of the STATUS_SET register from the base address
#define HSU_STATUS_SET_OFFSET 0x00000008
/// Index of the STATUS_SET register
#define HSU_STATUS_SET_INDEX  0x00000002
/// Reset value of the STATUS_SET register
#define HSU_STATUS_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the STATUS_SET register.
 * The STATUS_SET register will be read and its value returned.
 * @return The current value of the STATUS_SET register.
 */
__INLINE uint32_t hsu_status_set_get(void)
{
    return REG_PL_RD(HSU_STATUS_SET_ADDR);
}

/**
 * @brief Sets the STATUS_SET register to a value.
 * The STATUS_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_status_set_set(uint32_t value)
{
    REG_PL_WR(HSU_STATUS_SET_ADDR, value);
}

// field definitions
/// DONE_SET field bit
#define HSU_DONE_SET_BIT    ((uint32_t)0x00000001)
/// DONE_SET field position
#define HSU_DONE_SET_POS    0

/// DONE_SET field reset value
#define HSU_DONE_SET_RST    0x0

/**
 * @brief Returns the current value of the DONE_SET field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET field's value will be returned.
 *
 * @return The current value of the DONE_SET field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name STATUS_CLEAR register definitions
 * <table>
 * <caption id="STATUS_CLEAR_BF">STATUS_CLEAR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>00 <td>        DONE_CLEAR <td>R/W <td>R/W <td>0
 * </table>
 *
 * @{
 */

/// Address of the STATUS_CLEAR register
#define HSU_STATUS_CLEAR_ADDR   0xC030000C
/// Offset of the STATUS_CLEAR register from the base address
#define HSU_STATUS_CLEAR_OFFSET 0x0000000C
/// Index of the STATUS_CLEAR register
#define HSU_STATUS_CLEAR_INDEX  0x00000003
/// Reset value of the STATUS_CLEAR register
#define HSU_STATUS_CLEAR_RESET  0x00000000

/**
 * @brief Returns the current value of the STATUS_CLEAR register.
 * The STATUS_CLEAR register will be read and its value returned.
 * @return The current value of the STATUS_CLEAR register.
 */
__INLINE uint32_t hsu_status_clear_get(void)
{
    return REG_PL_RD(HSU_STATUS_CLEAR_ADDR);
}

/**
 * @brief Sets the STATUS_CLEAR register to a value.
 * The STATUS_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_status_clear_set(uint32_t value)
{
    REG_PL_WR(HSU_STATUS_CLEAR_ADDR, value);
}

// field definitions
/// DONE_CLEAR field bit
#define HSU_DONE_CLEAR_BIT    ((uint32_t)0x00000001)
/// DONE_CLEAR field position
#define HSU_DONE_CLEAR_POS    0

/// DONE_CLEAR field reset value
#define HSU_DONE_CLEAR_RST    0x0

/**
 * @brief Returns the current value of the DONE_CLEAR field in the STATUS_CLEAR register.
 *
 * The STATUS_CLEAR register will be read and the DONE_CLEAR field's value will be returned.
 *
 * @return The current value of the DONE_CLEAR field in the STATUS_CLEAR register.
 */
__INLINE uint8_t hsu_done_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_CLEAR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the DONE_CLEAR field of the STATUS_CLEAR register.
 *
 * The STATUS_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] doneclear - The value to set the field to.
 */
__INLINE void hsu_done_clear_setf(uint8_t doneclear)
{
    ASSERT_ERR((((uint32_t)doneclear << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_STATUS_CLEAR_ADDR, (uint32_t)doneclear << 0);
}

/// @}

/**
 * @name KEY_TAB register definitions
 * <table>
 * <caption id="KEY_TAB_BF">KEY_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>               KEY <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the KEY_TAB register
#define HSU_KEY_TAB_ADDR   0xC0300010
/// Offset of the KEY_TAB register from the base address
#define HSU_KEY_TAB_OFFSET 0x00000010
/// Index of the KEY_TAB register
#define HSU_KEY_TAB_INDEX  0x00000004
/// Reset value of the KEY_TAB register
#define HSU_KEY_TAB_RESET  0x00000000
/// Number of elements of the KEY_TAB register array
#define HSU_KEY_TAB_COUNT  4

/**
 * @brief Returns the current value of the KEY_TAB register.
 * The KEY_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the KEY_TAB register.
 */
__INLINE uint32_t hsu_key_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    return REG_PL_RD(HSU_KEY_TAB_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the KEY_TAB register to a value.
 * The KEY_TAB register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void hsu_key_tab_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 3);
    REG_PL_WR(HSU_KEY_TAB_ADDR + reg_idx * 4, value);
}

// field definitions
/// KEY field mask
#define HSU_KEY_MASK   ((uint32_t)0xFFFFFFFF)
/// KEY field LSB position
#define HSU_KEY_LSB    0
/// KEY field width
#define HSU_KEY_WIDTH  ((uint32_t)0x00000020)

/// KEY field reset value
#define HSU_KEY_RST    0x0

/**
 * @brief Returns the current value of the KEY field in the KEY_TAB register.
 *
 * The KEY_TAB register will be read and the KEY field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the KEY field in the KEY_TAB register.
 */
__INLINE uint32_t hsu_key_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    uint32_t localVal = REG_PL_RD(HSU_KEY_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the KEY field of the KEY_TAB register.
 *
 * The KEY_TAB register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] key - The value to set the field to.
 */
__INLINE void hsu_key_setf(int reg_idx, uint32_t key)
{
    ASSERT_ERR(reg_idx <= 3);
    ASSERT_ERR((((uint32_t)key << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_KEY_TAB_ADDR + reg_idx * 4, (uint32_t)key << 0);
}

/// @}

/**
 * @name SOURCE_ADDR register definitions
 * <table>
 * <caption id="SOURCE_ADDR_BF">SOURCE_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>       SOURCE_ADDR <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the SOURCE_ADDR register
#define HSU_SOURCE_ADDR_ADDR   0xC0300020
/// Offset of the SOURCE_ADDR register from the base address
#define HSU_SOURCE_ADDR_OFFSET 0x00000020
/// Index of the SOURCE_ADDR register
#define HSU_SOURCE_ADDR_INDEX  0x00000008
/// Reset value of the SOURCE_ADDR register
#define HSU_SOURCE_ADDR_RESET  0x00000000

/**
 * @brief Returns the current value of the SOURCE_ADDR register.
 * The SOURCE_ADDR register will be read and its value returned.
 * @return The current value of the SOURCE_ADDR register.
 */
__INLINE uint32_t hsu_source_addr_get(void)
{
    return REG_PL_RD(HSU_SOURCE_ADDR_ADDR);
}

/**
 * @brief Sets the SOURCE_ADDR register to a value.
 * The SOURCE_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_source_addr_set(uint32_t value)
{
    REG_PL_WR(HSU_SOURCE_ADDR_ADDR, value);
}

// field definitions
/// SOURCE_ADDR field mask
#define HSU_SOURCE_ADDR_MASK   ((uint32_t)0xFFFFFFFF)
/// SOURCE_ADDR field LSB position
#define HSU_SOURCE_ADDR_LSB    0
/// SOURCE_ADDR field width
#define HSU_SOURCE_ADDR_WIDTH  ((uint32_t)0x00000020)

/// SOURCE_ADDR field reset value
#define HSU_SOURCE_ADDR_RST    0x0

/**
 * @brief Returns the current value of the SOURCE_ADDR field in the SOURCE_ADDR register.
 *
 * The SOURCE_ADDR register will be read and the SOURCE_ADDR field's value will be returned.
 *
 * @return The current value of the SOURCE_ADDR field in the SOURCE_ADDR register.
 */
__INLINE uint32_t hsu_source_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_SOURCE_ADDR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the SOURCE_ADDR field of the SOURCE_ADDR register.
 *
 * The SOURCE_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sourceaddr - The value to set the field to.
 */
__INLINE void hsu_source_addr_setf(uint32_t sourceaddr)
{
    ASSERT_ERR((((uint32_t)sourceaddr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_SOURCE_ADDR_ADDR, (uint32_t)sourceaddr << 0);
}

/// @}

/**
 * @name LENGTH register definitions
 * <table>
 * <caption id="LENGTH_BF">LENGTH bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>15:00 <td>            LENGTH <td>R <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the LENGTH register
#define HSU_LENGTH_ADDR   0xC0300024
/// Offset of the LENGTH register from the base address
#define HSU_LENGTH_OFFSET 0x00000024
/// Index of the LENGTH register
#define HSU_LENGTH_INDEX  0x00000009
/// Reset value of the LENGTH register
#define HSU_LENGTH_RESET  0x00000000

/**
 * @brief Returns the current value of the LENGTH register.
 * The LENGTH register will be read and its value returned.
 * @return The current value of the LENGTH register.
 */
__INLINE uint32_t hsu_length_get(void)
{
    return REG_PL_RD(HSU_LENGTH_ADDR);
}

/**
 * @brief Sets the LENGTH register to a value.
 * The LENGTH register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_length_set(uint32_t value)
{
    REG_PL_WR(HSU_LENGTH_ADDR, value);
}

// field definitions
/// LENGTH field mask
#define HSU_LENGTH_MASK   ((uint32_t)0x0000FFFF)
/// LENGTH field LSB position
#define HSU_LENGTH_LSB    0
/// LENGTH field width
#define HSU_LENGTH_WIDTH  ((uint32_t)0x00000010)

/// LENGTH field reset value
#define HSU_LENGTH_RST    0x0

/**
 * @brief Returns the current value of the LENGTH field in the LENGTH register.
 *
 * The LENGTH register will be read and the LENGTH field's value will be returned.
 *
 * @return The current value of the LENGTH field in the LENGTH register.
 */
__INLINE uint16_t hsu_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_LENGTH_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the LENGTH field of the LENGTH register.
 *
 * The LENGTH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] length - The value to set the field to.
 */
__INLINE void hsu_length_setf(uint16_t length)
{
    ASSERT_ERR((((uint32_t)length << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(HSU_LENGTH_ADDR, (uint32_t)length << 0);
}

/// @}

/**
 * @name MIC_TAB register definitions
 * <table>
 * <caption id="MIC_TAB_BF">MIC_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>               MIC <td>R/W <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the MIC_TAB register
#define HSU_MIC_TAB_ADDR   0xC0300028
/// Offset of the MIC_TAB register from the base address
#define HSU_MIC_TAB_OFFSET 0x00000028
/// Index of the MIC_TAB register
#define HSU_MIC_TAB_INDEX  0x0000000A
/// Reset value of the MIC_TAB register
#define HSU_MIC_TAB_RESET  0x00000000
/// Number of elements of the MIC_TAB register array
#define HSU_MIC_TAB_COUNT  2

/**
 * @brief Returns the current value of the MIC_TAB register.
 * The MIC_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the MIC_TAB register.
 */
__INLINE uint32_t hsu_mic_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    return REG_PL_RD(HSU_MIC_TAB_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the MIC_TAB register to a value.
 * The MIC_TAB register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void hsu_mic_tab_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 1);
    REG_PL_WR(HSU_MIC_TAB_ADDR + reg_idx * 4, value);
}

// field definitions
/// MIC field mask
#define HSU_MIC_MASK   ((uint32_t)0xFFFFFFFF)
/// MIC field LSB position
#define HSU_MIC_LSB    0
/// MIC field width
#define HSU_MIC_WIDTH  ((uint32_t)0x00000020)

/// MIC field reset value
#define HSU_MIC_RST    0x0

/**
 * @brief Returns the current value of the MIC field in the MIC_TAB register.
 *
 * The MIC_TAB register will be read and the MIC field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the MIC field in the MIC_TAB register.
 */
__INLINE uint32_t hsu_mic_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    uint32_t localVal = REG_PL_RD(HSU_MIC_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the MIC field of the MIC_TAB register.
 *
 * The MIC_TAB register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] mic - The value to set the field to.
 */
__INLINE void hsu_mic_setf(int reg_idx, uint32_t mic)
{
    ASSERT_ERR(reg_idx <= 1);
    ASSERT_ERR((((uint32_t)mic << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_MIC_TAB_ADDR + reg_idx * 4, (uint32_t)mic << 0);
}

/// @}

/**
 * @name REMAINING register definitions
 * <table>
 * <caption id="REMAINING_BF">REMAINING bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>25:24 <td>  REMAINING_LENGTH <td>R/W <td>R/W <td>0x0
 * <tr><td>23:16 <td>   REMAINING_BYTE2 <td>R/W <td>R/W <td>0x0
 * <tr><td>15:08 <td>   REMAINING_BYTE1 <td>R/W <td>R/W <td>0x0
 * <tr><td>07:00 <td>   REMAINING_BYTE0 <td>R/W <td>R/W <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the REMAINING register
#define HSU_REMAINING_ADDR   0xC0300030
/// Offset of the REMAINING register from the base address
#define HSU_REMAINING_OFFSET 0x00000030
/// Index of the REMAINING register
#define HSU_REMAINING_INDEX  0x0000000C
/// Reset value of the REMAINING register
#define HSU_REMAINING_RESET  0x00000000

/**
 * @brief Returns the current value of the REMAINING register.
 * The REMAINING register will be read and its value returned.
 * @return The current value of the REMAINING register.
 */
__INLINE uint32_t hsu_remaining_get(void)
{
    return REG_PL_RD(HSU_REMAINING_ADDR);
}

/**
 * @brief Sets the REMAINING register to a value.
 * The REMAINING register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_remaining_set(uint32_t value)
{
    REG_PL_WR(HSU_REMAINING_ADDR, value);
}

// field definitions
/// REMAINING_LENGTH field mask
#define HSU_REMAINING_LENGTH_MASK   ((uint32_t)0x03000000)
/// REMAINING_LENGTH field LSB position
#define HSU_REMAINING_LENGTH_LSB    24
/// REMAINING_LENGTH field width
#define HSU_REMAINING_LENGTH_WIDTH  ((uint32_t)0x00000002)
/// REMAINING_BYTE2 field mask
#define HSU_REMAINING_BYTE2_MASK    ((uint32_t)0x00FF0000)
/// REMAINING_BYTE2 field LSB position
#define HSU_REMAINING_BYTE2_LSB     16
/// REMAINING_BYTE2 field width
#define HSU_REMAINING_BYTE2_WIDTH   ((uint32_t)0x00000008)
/// REMAINING_BYTE1 field mask
#define HSU_REMAINING_BYTE1_MASK    ((uint32_t)0x0000FF00)
/// REMAINING_BYTE1 field LSB position
#define HSU_REMAINING_BYTE1_LSB     8
/// REMAINING_BYTE1 field width
#define HSU_REMAINING_BYTE1_WIDTH   ((uint32_t)0x00000008)
/// REMAINING_BYTE0 field mask
#define HSU_REMAINING_BYTE0_MASK    ((uint32_t)0x000000FF)
/// REMAINING_BYTE0 field LSB position
#define HSU_REMAINING_BYTE0_LSB     0
/// REMAINING_BYTE0 field width
#define HSU_REMAINING_BYTE0_WIDTH   ((uint32_t)0x00000008)

/// REMAINING_LENGTH field reset value
#define HSU_REMAINING_LENGTH_RST    0x0
/// REMAINING_BYTE2 field reset value
#define HSU_REMAINING_BYTE2_RST     0x0
/// REMAINING_BYTE1 field reset value
#define HSU_REMAINING_BYTE1_RST     0x0
/// REMAINING_BYTE0 field reset value
#define HSU_REMAINING_BYTE0_RST     0x0

/**
 * @brief Constructs a value for the REMAINING register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] remaininglength - The value to use for the REMAINING_LENGTH field.
 * @param[in] remainingbyte2 - The value to use for the REMAINING_BYTE2 field.
 * @param[in] remainingbyte1 - The value to use for the REMAINING_BYTE1 field.
 * @param[in] remainingbyte0 - The value to use for the REMAINING_BYTE0 field.
 */
__INLINE void hsu_remaining_pack(uint8_t remaininglength, uint8_t remainingbyte2, uint8_t remainingbyte1, uint8_t remainingbyte0)
{
    ASSERT_ERR((((uint32_t)remaininglength << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte2 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte1 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR,  ((uint32_t)remaininglength << 24) | ((uint32_t)remainingbyte2 << 16) | ((uint32_t)remainingbyte1 << 8) | ((uint32_t)remainingbyte0 << 0));
}

/**
 * @brief Unpacks REMAINING's fields from current value of the REMAINING register.
 *
 * Reads the REMAINING register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] remaininglength - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte2 - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte1 - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte0 - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_remaining_unpack(uint8_t* remaininglength, uint8_t* remainingbyte2, uint8_t* remainingbyte1, uint8_t* remainingbyte0)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);

    *remaininglength = (localVal & ((uint32_t)0x03000000)) >> 24;
    *remainingbyte2 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *remainingbyte1 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *remainingbyte0 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the REMAINING_LENGTH field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_LENGTH field's value will be returned.
 *
 * @return The current value of the REMAINING_LENGTH field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the REMAINING_LENGTH field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remaininglength - The value to set the field to.
 */
__INLINE void hsu_remaining_length_setf(uint8_t remaininglength)
{
    ASSERT_ERR((((uint32_t)remaininglength << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)remaininglength << 24));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE2 field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_BYTE2 field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE2 field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_byte2_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the REMAINING_BYTE2 field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte2 - The value to set the field to.
 */
__INLINE void hsu_remaining_byte2_setf(uint8_t remainingbyte2)
{
    ASSERT_ERR((((uint32_t)remainingbyte2 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)remainingbyte2 << 16));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE1 field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_BYTE1 field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE1 field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_byte1_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the REMAINING_BYTE1 field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte1 - The value to set the field to.
 */
__INLINE void hsu_remaining_byte1_setf(uint8_t remainingbyte1)
{
    ASSERT_ERR((((uint32_t)remainingbyte1 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)remainingbyte1 << 8));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE0 field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_BYTE0 field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE0 field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_byte0_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the REMAINING_BYTE0 field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte0 - The value to set the field to.
 */
__INLINE void hsu_remaining_byte0_setf(uint8_t remainingbyte0)
{
    ASSERT_ERR((((uint32_t)remainingbyte0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)remainingbyte0 << 0));
}

/// @}

#if RW_HSU_SHA_EN
/**
 * @name SHA_TAB register definitions
 * <table>
 * <caption id="SHA_TAB_BF">SHA_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td>31:00 <td>               SHA <td>W <td>R <td>0x0
 * </table>
 *
 * @{
 */

/// Address of the SHA_TAB register
#define HSU_SHA_TAB_ADDR   0xC0300034
/// Offset of the SHA_TAB register from the base address
#define HSU_SHA_TAB_OFFSET 0x00000034
/// Index of the SHA_TAB register
#define HSU_SHA_TAB_INDEX  0x0000000D
/// Reset value of the SHA_TAB register
#define HSU_SHA_TAB_RESET  0x00000000
/// Number of elements of the SHA_TAB register array
#define HSU_SHA_TAB_COUNT  16

/**
 * @brief Returns the current value of the SHA_TAB register.
 * The SHA_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the SHA_TAB register.
 */
__INLINE uint32_t hsu_sha_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 15);
    return REG_PL_RD(HSU_SHA_TAB_ADDR + reg_idx * 4);
}

// field definitions
/// SHA field mask
#define HSU_SHA_MASK   ((uint32_t)0xFFFFFFFF)
/// SHA field LSB position
#define HSU_SHA_LSB    0
/// SHA field width
#define HSU_SHA_WIDTH  ((uint32_t)0x00000020)

/// SHA field reset value
#define HSU_SHA_RST    0x0

/**
 * @brief Returns the current value of the SHA field in the SHA_TAB register.
 *
 * The SHA_TAB register will be read and the SHA field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the SHA field in the SHA_TAB register.
 */
__INLINE uint32_t hsu_sha_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 15);
    uint32_t localVal = REG_PL_RD(HSU_SHA_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

#endif // RW_HSU_SHA_EN
/// @}


#endif // _REG_HSU_H_

/// @}

