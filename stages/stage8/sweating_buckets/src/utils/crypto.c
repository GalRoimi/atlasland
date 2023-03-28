#include <stdio.h>
#include <stdint.h>

#include <mbedtls/md.h>
#include <mbedtls/entropy.h>
#include <mbedtls/hmac_drbg.h>

#include "crypto.h"

static int entropy_seed(void* data, unsigned char* output, size_t len, size_t* olen)
{
    (void) data;

    volatile uint32_t* const RTC = (uint32_t* const)0x101e8000;

    int retval = 0;

    mbedtls_md_context_t ctx = {};
    const mbedtls_md_info_t* md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA512);

    mbedtls_md_init(&ctx);

    int ret = mbedtls_md_setup(&ctx, md_info, 0);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    ret = mbedtls_md_starts(&ctx);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    ret = mbedtls_md_update(&ctx, (const uint8_t*)RTC, sizeof(*RTC));
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    ret = mbedtls_md_finish(&ctx, output);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    *olen = mbedtls_md_get_size(md_info);

cleanup:
    mbedtls_md_free(&ctx);

    return retval;
}

int rand(uint8_t* output, size_t len)
{
    /* sanity checks */
    if ((output == NULL) || (len == 0))
    {
        return -1;
    }

    int retval = 0;

    const mbedtls_md_info_t* md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_hmac_drbg_context hmac_drbg = {};
    mbedtls_entropy_context entropy = {};

    mbedtls_hmac_drbg_init(&hmac_drbg);
    mbedtls_entropy_init(&entropy);

    int ret = mbedtls_entropy_add_source(&entropy, &entropy_seed, NULL, MBEDTLS_ENTROPY_BLOCK_SIZE, MBEDTLS_ENTROPY_SOURCE_STRONG);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    ret = mbedtls_hmac_drbg_seed(&hmac_drbg, md_info, mbedtls_entropy_func, &entropy, (const unsigned char *) "RANDOM_GEN", 10);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    mbedtls_hmac_drbg_set_prediction_resistance(&hmac_drbg, MBEDTLS_HMAC_DRBG_PR_OFF);

    ret = mbedtls_hmac_drbg_random(&hmac_drbg, output, len);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

cleanup:
    mbedtls_hmac_drbg_free(&hmac_drbg);
    mbedtls_entropy_free(&entropy);

    return retval;
}

int hmac256(const uint8_t* key, size_t klen, const uint8_t* msg, size_t len, uint8_t* output, size_t olen)
{
    /* saniy checks */
    if ((key == NULL) || (klen == 0) || (msg == NULL) || (len == 0) || (output == NULL) || (olen != HMAC256_SIZE))
    {
        return -1;
    }

    int retval = 0;

    mbedtls_md_context_t ctx = {};
    const mbedtls_md_info_t* md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    mbedtls_md_init(&ctx);

    int ret = mbedtls_md_setup(&ctx, md_info, 1);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    ret = mbedtls_md_hmac_starts(&ctx, key, klen);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    ret = mbedtls_md_hmac_update(&ctx, msg, len);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

    ret = mbedtls_md_hmac_finish(&ctx, output);
    if (ret != 0)
    {
        retval = ret;
        goto cleanup;
    }

cleanup:
    mbedtls_md_free(&ctx);

    return retval;
}
