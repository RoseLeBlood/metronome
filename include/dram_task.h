#ifndef METRONOME_DRAM_TASK_H
#define METRONOME_DRAM_TASK_H

#include <mn_task.hpp>
#undef ERR_TIMEOUT

#include "udp_client.h"
#include "audio_network_buffer.h"

namespace metronome {
    class dram_task : public mn::basic_task {
    public:
        dram_task(audio_network_buffer& buffer, ip4_endpoint endpoint,
                  bool use_multicast, bool udpLite);

        virtual void*  on_task() override;
    protected:
        audio_chunk_net_paket* recive_pkg(ip4_endpoint* from, char* buffer);
        void parse_pkg(audio_chunk_net_paket* pkg);
    private:
        ip4_address m_ip;
        audio_network_buffer& m_bufferRef;
        udp_client *m_udpClient;
    };

}
#endif // METRONOME_DRAM_TASK_H
