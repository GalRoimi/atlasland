#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <stdint.h>
#include <stdio.h>

#define HMAC256_SIZE (32)
#define SHA256_SIZE (32)

/**
 * Generates a sequence of random bytes using a hardware random number generator.
 *
 * @brief Generates random bytes.
 *
 * @param output Pointer to the buffer where the random bytes will be stored.
 * @param len    Number of random bytes to generate and store in the buffer.
 *
 * @return 0 if successful, -1 otherwise.
 */
int rand(uint8_t* output, size_t len);

/**
 * Computes the HMAC-SHA-256 message authentication code for the provided key and message.
 *
 * @brief Computes the HMAC-SHA-256.
 *
 * @param key       Pointer to the key used to compute the HMAC-SHA-256.
 * @param klen      Length of the key in bytes.
 * @param msg       Pointer to the message to authenticate.
 * @param len       Length of the message in bytes.
 * @param output    Pointer to the buffer where the computed HMAC-SHA-256 will be stored.
 * @param olen      Length of the output signature buffer in bytes.
 *
 * @return 0 if successful, -1 otherwise.
 */
int hmac256(const uint8_t* key, size_t klen, const uint8_t* msg, size_t len, uint8_t* output, size_t olen);

/**
 * Computes the HMAC-SHA-256 message authentication code for the provided key and message.
 *
 * @brief Computes the HMAC-SHA-256.
 *
 * @param key       Pointer to the key used to compute the HMAC-SHA-256.
 * @param klen      Length of the key in bytes.
 * @param msg       Pointer to the message to authenticate.
 * @param len       Length of the message in bytes.
 * @param output    Pointer to the buffer where the computed HMAC-SHA-256 will be stored.
 * @param olen      Length of the output signature buffer in bytes.
 *
 * @return 0 if successful, -1 otherwise.
 */
int sha256(const uint8_t* data, size_t len, uint8_t* output, size_t olen);

#endif /* _CRYPTO_H_ */
