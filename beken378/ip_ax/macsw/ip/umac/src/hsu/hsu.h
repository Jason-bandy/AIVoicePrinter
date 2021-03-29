/**
 ******************************************************************************
 * @file hsu.h
 *
 * @brief Hardware Security Unit module definitions.
 *
 * Copyright (C) RivieraWaves 2017-2020
 *
 ******************************************************************************
 */
#ifndef _HSU_H_
#define _HSU_H_

#include "me_mic.h"

/**
 * @defgroup HSU HSU
 * @ingroup UMAC
 * @brief Structures and Helper functions to use the Hardware Security Unit.
 */

/**
 * @addtogroup HSU
 * @{
 */
/**
 ****************************************************************************************
 * @brief HSU interrupt service routine.
 *
 * HSU interrupt is used when fw is compiled to run with a RTOS (i.e. NX_FULLY_HOSTED)
 * for crypto mode.
 ****************************************************************************************
 */
void hsu_isr(void);

/**
 ******************************************************************************
 * @brief Checks if HSU is available
 *
 * It updates internal variable @ref hsu_status.
 * If HSU is not available and @ref NX_HSU is defined to 1 then hsu functions
 * will immediately return and software implementation will be used instead.
 * If HSU is not available and @ref NX_HSU is set to 2, then an error is
 * generated.
 ******************************************************************************
 */
void hsu_init(void);

/**
 ******************************************************************************
 * @brief Computes AES-CMAC on several vectors using HSU
 *
 * All vectors MUST be located in SHARED MEMORY.
 *
 * @param[in] key      Key (16 bytes)
 * @param[in] nb_elem  Number of element in @p addr and @p len tables.
 * @param[in] addr     Table of input vector addresses (HW address)
 * @param[in] len      Table of input vector length (in bytes)
 *
 * @return AES_CMAC value of all vectors concatenated.
 ******************************************************************************
 */
uint64_t hsu_aes_cmac(uint32_t *key, int nb_elem, uint32_t addr[], int len[]);

/**
 ******************************************************************************
 * @brief Initializes TKIP MIC computation using HSU
 *
 * @param[out] mic_calc Pointer to Mic structure that will be initialize
 * @param[in]  mic_key  Key to use for MIC computation
 * @param[in]  aad      Additional Authentication Data vector
 *                      (CPU address, 16 bytes long)
 *                      It must be located in SHARED RAM.
 *
 * @return true if computation has been done by HSU, false otherwise
 ******************************************************************************
 */
bool hsu_michael_init(struct mic_calc *mic_calc, uint32_t *mic_key,
                      uint32_t *aad);

/**
 ******************************************************************************
 * @brief Continues TKIP MIC computation using HSU
 *
 * Continue MIC computation with the provided data.
 *
 * @param[in,out] mic_calc Pointer to Mic structure
 * @param[in]     data     Address of the data buffer (HW address)
 *                         It must be located in SHARED RAM.
 * @param[in]     data_len Length, in bytes, of the data buffer
 *
 * @return true if computation has been done by HSU, false otherwise
 ******************************************************************************
 */
bool hsu_michael_calc(struct mic_calc *mic_calc, uint32_t data,
                      uint32_t data_len);

/**
 ******************************************************************************
 * @brief Ends TKIP MIC computation using HSU
 *
 * Ends TKIP computation by adding padding data
 *
 * @param[in,out] mic_calc Pointer to Mic structure that will be initialize
 *
 * @return true if computation has been done by HSU, false otherwise
 ******************************************************************************
 */
bool hsu_michael_end(struct mic_calc *mic_calc);

/**
 ******************************************************************************
 * @brief Computes the IP checksum on the buffer provided.
 *
 * @param[in]  addr     The address of the data buffer on which the checksum is computed
 * @param[in]  len      Length on which the checksum is computed
 * @param[out] checksum The computed checksum
 *
 * @return true if the checksum was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_ip_checksum(uint32_t addr, uint16_t len, uint16_t *checksum);

/**
 ******************************************************************************
 * @brief Computes the SHA1 on the buffers provided.
 *
 * For this function buffers doesn't have to be located in Shared memory but the
 * total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the hash is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] sha      The computed hash. MUST be, at least, 20 bytes long
 *
 * @return true if the hash was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_sha1(int nb_elem, const uint8_t *addr[], const size_t *len, uint32_t *sha);

/**
 ******************************************************************************
 * @brief Computes the SHA224 on the buffers provided.
 *
 * For this function buffers doesn't have to be located in Shared memory but the
 * total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the hash is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] sha      The computed hash. MUST be, at least, 28 bytes long
 *
 * @return true if the hash was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_sha224(int nb_elem, const uint8_t *addr[], const size_t *len, uint32_t *sha);

/**
 ******************************************************************************
 * @brief Computes the SHA256 on the buffers provided.
 *
 * For this function buffers doesn't have to be located in Shared memory but the
 * total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the hash is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] sha      The computed hash. MUST be, at least, 32 bytes long
 *
 * @return true if the hash was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_sha256(int nb_elem, const uint8_t *addr[], const size_t *len, uint32_t *sha);

/**
 ******************************************************************************
 * @brief Computes the SHA384 on the buffers provided.
 *
 * For this function buffers doesn't have to be located in Shared memory but the
 * total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the hash is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] sha      The computed hash. MUST be, at least, 48 bytes long
 *
 * @return true if the hash was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_sha384(int nb_elem, const uint8_t *addr[], const size_t *len, uint32_t *sha);

/**
 ******************************************************************************
 * @brief Computes the SHA512 on the buffers provided.
 *
 * For this function buffers doesn't have to be located in Shared memory but the
 * total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the hash is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] sha      The computed hash. MUST be, at least, 64 bytes long
 *
 * @return true if the hash was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_sha512(int nb_elem, const uint8_t *addr[], const size_t *len, uint32_t *sha);

/**
 ******************************************************************************
 * @brief Computes the HMAC-SHA1 on the buffers provided.
 *
 * For this function key and data buffers doesn't have to be located in Shared
 * memory but the total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  key      Key to use to compute the MAC.
 *                      CPU address is expected.
 * @param[in]  key_len  Length, in bytes, of key buffer.
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the MAC is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] mac      The computed MAC. MUST be, at least, 20 bytes long
 *
 * @return true if the MAC was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_hmac_sha1(const uint8_t key[], const size_t key_len, int nb_elem,
                   const uint8_t *addr[], const size_t *len, uint32_t *mac);

/**
 ******************************************************************************
 * @brief Computes the HMAC-SHA224 on the buffers provided.
 *
 * For this function key and data buffers doesn't have to be located in Shared
 * memory but the total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  key      Key to use to compute the MAC.
 *                      CPU address is expected.
 * @param[in]  key_len  Length, in bytes, of key buffer.
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the MAC is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] mac      The computed MAC. MUST be, at least, 28 bytes long
 *
 * @return true if the MAC was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_hmac_sha224(const uint8_t key[], const size_t key_len, int nb_elem,
                     const uint8_t *addr[], const size_t *len, uint32_t *mac);

/**
 ******************************************************************************
 * @brief Computes the HMAC-SHA256 on the buffers provided.
 *
 * For this function key and data buffers doesn't have to be located in Shared
 * memory but the total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  key      Key to use to compute the MAC.
 *                      CPU address is expected.
 * @param[in]  key_len  Length, in bytes, of key buffer.
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the MAC is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] mac      The computed MAC. MUST be, at least, 32 bytes long
 *
 * @return true if the MAC was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_hmac_sha256(const uint8_t key[], const size_t key_len, int nb_elem,
                     const uint8_t *addr[], const size_t *len, uint32_t *mac);

/**
 ******************************************************************************
 * @brief Computes the HMAC-SHA384 on the buffers provided.
 *
 * For this function key and data buffers doesn't have to be located in Shared
 * memory but the total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  key      Key to use to compute the MAC.
 *                      CPU address is expected.
 * @param[in]  key_len  Length, in bytes, of key buffer.
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the MAC is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] mac      The computed MAC. MUST be, at least, 48 bytes long
 *
 * @return true if the MAC was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_hmac_sha384(const uint8_t key[], const size_t key_len, int nb_elem,
                     const uint8_t *addr[], const size_t *len, uint32_t *mac);

/**
 ******************************************************************************
 * @brief Computes the HMAC-SHA512 on the buffers provided.
 *
 * For this function key and data buffers doesn't have to be located in Shared
 * memory but the total size of buffers must be lower than @ref HSU_SHA_MAX_LEN.
 *
 * @param[in]  key      Key to use to compute the MAC.
 *                      CPU address is expected.
 * @param[in]  key_len  Length, in bytes, of key buffer.
 * @param[in]  nb_elem  Number of buffer in \p addr.
 * @param[in]  addr     The address of the data buffer on which the MAC is computed
 *                      CPU address is expected.
 * @param[in]  len      Length, in bytes, of each element of \p addr.
 * @param[out] mac      The computed MAC. MUST be, at least, 64 bytes long
 *
 * @return true if the MAC was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_hmac_sha512(const uint8_t key[], const size_t key_len, int nb_elem,
                     const uint8_t *addr[], const size_t *len, uint32_t *mac);

/**
 ******************************************************************************
 * @brief Computes RSA-1024
 *
 * For this function input buffers don't have to be located in Shared memory,
 * but all buffers must be 128 bytes long (padded with 0 on MSB if needed)
 *
 * @param[in]  message        Message to encrypt/decrypt
 * @param[in]  exponent       Public exponent for encryption or
 *                            Private exponent for decryption
 * @param[in]  modulus        Public modulus
 * @param[in]  little_endian  Whether input/output buffers are in little
 *                            endian format or not
 * @param[out] res            Output buffer. Must also be 128 bytes long
 * @return true if the RSA was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_rsa1024(const uint8_t *message, const uint8_t *exponent,
                 const uint8_t *modulus, bool little_endian, uint8_t *res);

/**
 ******************************************************************************
 * @brief Computes RSA-2048
 *
 * For this function input buffers don't have to be located in Shared memory,
 * but all buffers must be 256 bytes long (padded with 0 on MSB if needed)
 *
 * @param[in]  message        Message to encrypt/decrypt
 * @param[in]  exponent       Public exponent for encryption or
 *                            Private exponent for decryption
 * @param[in]  modulus        Public modulus
 * @param[in]  little_endian  Whether input/output buffers are in little
 *                            endian format or not
 * @param[out] res            Output buffer. Must also be 256 bytes long
 * @return true if the RSA was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_rsa2048(const uint8_t *message, const uint8_t *exponent,
                 const uint8_t *modulus, bool little_endian, uint8_t *res);

/**
 ******************************************************************************
 * @brief Computes RSA-4096
 *
 * For this function input buffers don't have to be located in Shared memory,
 * but all buffers must be 512 bytes long (padded with 0 on MSB if needed)
 *
 * @param[in]  message        Message to encrypt/decrypt
 * @param[in]  exponent       Public exponent for encryption or
 *                            Private exponent for decryption
 * @param[in]  modulus        Public modulus
 * @param[in]  little_endian  Whether input/output buffers are in little
 *                            endian format or not
 * @param[out] res            Output buffer. Must also be 512 bytes long
 * @return true if the RSA was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_rsa4096(const uint8_t *message, const uint8_t *exponent,
                 const uint8_t *modulus, bool little_endian, uint8_t *res);

/**
 ******************************************************************************
 * @brief Computes generic modular exponentiation
 *
 * For this function input buffers don't have to be located in Shared memory,
 * and all buffers can have different size.
 * The output buffer must be big enough for the RSA computation that will be
 * associated. It depends of the maximum length of the input buffers :
 * - If max_len > 512 bytes => not supported, return false immediately
 * - If max_len > 256 bytes => result will be 512 bytes long
 * - If max_len > 128 bytes => result will be 256 bytes long
 * - If max_len > 96 bytes => result will be 128 bytes long
 * - If max_len > 64 bytes => result will be 96 bytes long
 * - If max_len > 32 bytes => result will be 64 bytes long
 * - Otherwise result will be 32 bytes long
 *
 * @param[in]     val            Value buffer
 * @param[in]     val_len        Value size in bytes
 * @param[in]     exponent       Exponent buffer
 * @param[in]     exponent_len   Exponent size in bytes
 * @param[in]     modulus        Modulus buffer
 * @param[in]     modulus_len    Modulus size in bytes
 * @param[in]     little_endian  Whether input/output buffers are in little
 *                               endian format or not
 * @param[out]    res            Output buffer, must be big enough.
 * @param[in,out] res_len        Use to pass the size of the res buffer and
 *                               updated with the number of bytes written.
 * @return true if the exponentiation was correctly computed, false otherwise
 ******************************************************************************
 */
bool hsu_mod_exp(const uint8_t *val, int val_len, const uint8_t *exponent, int exponent_len,
                 const uint8_t *modulus, int modulus_len, bool little_endian,
                 uint8_t *res, int *res_len);


/**
 * Elliptic Curves supported by HSU
 */
enum hsu_elliptic_curve {
    /// 256-bit random ECP group (aka IANA IKE group 19)
    HSU_EC_256 = 0,
    /// 384-bit random ECP group (aka IANA IKE group 20)
    HSU_EC_384 = 1,
    /// 521-bit random ECP group (aka IANA IKE group 21)
    HSU_EC_521 = 2,
    /// Number of Elliptic Curve supported (keep last)
    HSU_EC_MAX,
};

/**
 ******************************************************************************
 * @brief Multiply an Elliptic Curve Point by an integer
 *
 * R(x,y) = k * P(x,y)
 * For this function input buffers don't have to be located in Shared memory.
 * To simplify implementation input/output buffers are read/write as 32bits
 * words and must as such be aligned on 32bits.
 * Size of @p P coordinates are defined by the selected curve and as such if
 * provided buffers are too big they are simply truncated and no error is
 * reported.
 * The size of the @p k buffer is also truncated to the maximum coordinate size
 * if needed.
 * Buffers of the result point @p R can be the same as the input point @p P
 * (assuming they are big enough).
 *
 * @param[in]  curve  Elliptic Curve
 * @param[in]  k      Integer buffer (in little endian)
 * @param[in]  k_len  Integer buffer length in 32bits words
 * @param[in]  P      Input EC point buffers (index 0=x, index 1=y) (in little endian)
 * @param[in]  P_len  Input EC point buffer lengths in 32bits words
 * @param[out] R      Output EC point buffers. Both must be big enough
 *                    for the selected curve
 *
 * @return true if the multiplication has been done, false otherwise
 ******************************************************************************
 */
bool hsu_ecp_mul(enum hsu_elliptic_curve curve, const uint32_t *k, size_t k_len,
                 const uint32_t *P[2], size_t P_len[2], uint32_t *R[2]);

/**
 * @}
 */
#endif /* _HSU_H_ */
