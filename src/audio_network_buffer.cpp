#include "audio_network_buffer.h"
#include <cstring>

namespace metronome {
    audio_network_buffer::audio_network_buffer() {
        m_buffer = rb_create(sizeof(char*), 250);
    }

    mn::size_t audio_network_buffer::read_audio(char* buffer, mn::size_t data_size) {
        if(rb_bytes_available(m_buffer) >= data_size) {
            return rb_read(m_buffer, buffer, data_size, portMAX_DELAY);
        }
        return 0;
    }
    mn::size_t audio_network_buffer::write_audio(char* buffer, mn::size_t data_size) {

        return rb_write(m_buffer, buffer, data_size, portMAX_DELAY);
    }
}

