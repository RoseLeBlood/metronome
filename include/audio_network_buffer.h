#ifndef AUDIO_NETWORK_BUFFER_H
#define AUDIO_NETWORK_BUFFER_H


#include "audio_chunk.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include  "ringbuf.h"

namespace metronome {
    class audio_network_buffer {
    public:
        audio_network_buffer();

        mn::size_t read_audio(char* buffer, mn::size_t data_size);
        mn::size_t write_audio(char* buffer, mn::size_t data_size);
    private:
        ringbuf_handle_t m_buffer;
    };
}

#endif // AUDIO_NETWORK_BUFFER_H
