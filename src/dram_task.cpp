#include "dram_task.h"
#include <esp_log.h>
#include <esp_system.h>

#include <miniThread.hpp>

namespace metronome {
    dram_task::dram_task(audio_network_buffer& buffer, ip4_endpoint endpoint,
                         bool use_multicast, bool udpLite)

     : mn::basic_task("dram_task", mn::basic_task::PriorityNormal, 4096),
       m_ip(endpoint.get_ip()),
       m_bIsudpLite(udpLite),
       m_bufferRef(buffer) {

       m_udpClient = new udp_client(endpoint);
       m_udpClient->setup_multicast(use_multicast);
    }

    audio_chunk_net_paket* dram_task::recive_pkg(ip4_endpoint* from, char* buffer) {
        m_udpClient->receive(from, buffer, 0, sizeof(buffer) );

        return (audio_chunk_net_paket*)&buffer[0];
    }
    void dram_task::parse_pkg(audio_chunk_net_paket* pkg) {
        if(pkg == NULL) return;

        int written = 0;

        switch(pkg->command) {
        case AUDIO_CHUNK_CMD_NOT:
            written = m_bufferRef.write_audio(pkg->data.buffer, pkg->data.buffer_size);
            ESP_LOGD("dram_task", "buffer written %d", written);
            break;
        case AUDIO_CHUNK_CMD_RESTART:
            ESP_LOGI("dram_task", "esp will be restart");
            esp_restart();
            break;
        case AUDIO_CHUNK_CMD_PLAY: break;
        case AUDIO_CHUNK_CMD_STOP: break;
        case AUDIO_CHUNK_CMD_VOLUP: break;
        case AUDIO_CHUNK_CMD_VOLDOWN: break;
        case AUDIO_CHUNK_CMD_RECONNECT:
            m_udpClient->reconnect();
            if(m_udpClient->is_multicast()) {
                m_udpClient->left_multicast_group(m_ip);
                m_udpClient->join_multicast_group(m_ip);
            }
            break;

        default:
            ESP_LOGI("dram_task", "unsupport pkg %d", pkg->command);
        };
    }

     void*  dram_task::on_task() {
        if(!m_udpClient->connect(m_bIsudpLite)) return NULL;
        if(m_udpClient->is_multicast()) {
            m_udpClient->join_multicast_group(m_ip);
        }
        mn::task_t::sleep(100);

        audio_chunk_net_paket* pkg;
        ip4_endpoint from(METRONOME_IPV4_ADDRESS_ANY, 0);
        char buffer[mn::value2size_raw<audio_chunk_net_paket>::size];

        while(true) {
            while(m_udpClient->available() > 0) {
                from = ip4_endpoint(METRONOME_IPV4_ADDRESS_ANY, 0);
                memset(buffer, 0, sizeof(buffer));

                pkg = recive_pkg(&from, buffer);
                parse_pkg(pkg);

                mn::task_t::sleep(1);
            }
        }


        return NULL;
     }
}
