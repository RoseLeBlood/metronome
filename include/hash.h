#ifndef HASH_H
#define HASH_H

#include <mn_def.hpp>
#include <string>

namespace metronome {
    unsigned char* calc_sha256_hmac_ex(const char* data, mn::size_t data_size,
                                       const char* key, mn::size_t key_size,
                                       unsigned char output[32]);

    unsigned char* calc_sha256_ex(const char* data, mn::size_t data_size, unsigned char output[32]);


    unsigned char* calc_sha256_data(const void* data, unsigned char output[32]);
    unsigned char* calc_sha256_hmac_data(const void* data, const char* key, unsigned char output[32]);
}
#endif // HASH_H
