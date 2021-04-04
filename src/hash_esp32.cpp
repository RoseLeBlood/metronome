#include "hash.h"
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"

#include <string.h>
#include <memory.h>

namespace metronome {
    unsigned char* calc_sha256_hmac(const char* data, mn::size_t data_size, const char* key, mn::size_t key_size,  unsigned char output[32]) {
        mbedtls_md_context_t ctx;

        mbedtls_md_init(&ctx);
        mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);

        mbedtls_md_hmac_starts(&ctx, (const unsigned char *) key, key_size);
        mbedtls_md_hmac_update(&ctx, (const unsigned char *) data, data_size);
        mbedtls_md_hmac_finish(&ctx, output);

        mbedtls_md_free(&ctx);

        return output;
    }

    unsigned char* calc_sha256_hmac_data(const void* data, const char* key, unsigned char output[32]) {
        return calc_sha256_hmac_ex((const char*)data, sizeof(data), key, strlen(key), output);

    }

    unsigned char* calc_sha256_ex(const char* data, mn::size_t data_size, unsigned char output[32]) {
        mbedtls_sha256_context ctx;
        mbedtls_sha256_init(&ctx);
        if(mbedtls_sha256_starts_ret(&ctx, 0) != 0) return 0;
        if(mbedtls_sha256_update_ret(&ctx, (const unsigned char *) data, data_size) != 0) return 0;
        if(mbedtls_sha256_finish_ret(&ctx, output) != 0) return 0;

        mbedtls_sha256_free(&ctx);

        return output;
    }
    unsigned char* calc_sha256_data(const void* data, unsigned char output[32]) {
        return calc_sha256_ex((const char*)data, sizeof(data), output);
    }
}
