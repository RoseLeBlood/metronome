#include "audio_chunk.h"
#include <cstring>

namespace metronome {
    bool audio_chunk_net_paket::is_valid() {
        unsigned char _hash[32];
        calc_sha256_hmac_data(&this->data, METRONOME_HMAC_SHA256_KEY, _hash);

        for(unsigned char i = 0; i < 32; i++)
            if(hash[i] != _hash[i]) return false;

        return true;
    }
}

