#ifndef AUDIO_CHUNK_H
#define AUDIO_CHUNK_H

#include "config.h"
#include "hash.h"

#define AUDIO_CHUNK_CMD_NOT         0
#define AUDIO_CHUNK_CMD_PLAY        1
#define AUDIO_CHUNK_CMD_STOP        2
#define AUDIO_CHUNK_CMD_RESTART     3
#define AUDIO_CHUNK_CMD_VOLUP       4
#define AUDIO_CHUNK_CMD_VOLDOWN     5
#define AUDIO_CHUNK_CMD_RECONNECT   6

namespace metronome {

    /**
     @brief The real paket are send with udp
    */
    struct audio_chunk_net_paket {
        unsigned char command : 4;

        struct {
            unsigned int buffer_size;
            char buffer[512];
        } data;
        unsigned char hash[32]; // SHA256_HMAC - key in config
        bool is_valid();
    };
}
#endif // AUDIO_CHUNK_H
