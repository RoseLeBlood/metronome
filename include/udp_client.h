#ifndef METRONOME_UDP_CLIENT_H
#define METRONOME_UDP_CLIENT_H

#include "socket.h"

namespace metronome {
    class udp_client {
    public:
        udp_client(ip4_address address, int port);
        udp_client(ip4_endpoint endpoint);

        virtual bool connect(bool use_lite);
        virtual bool disconnect();
        virtual bool reconnect();

        /// This option will enable/disable SO_REUSEADDR
        void set_reuse_address(bool is);

        ip4_endpoint& get_endpoint()        { return m_ipEndpoint; }

        bool is_multicast()                 { return m_bIsMulticast; }
        bool is_reuse_address()             { return m_bIsReuseAddress; }
        bool is_connected()                 { return m_bIsConnected; }

        void setup_multicast(bool is);

        void join_multicast_group(ip4_address& ip);
        void left_multicast_group(ip4_address& ip);

        virtual int send(const ip4_endpoint& endpoint, void* buffer, size_t size);
        virtual int send(const ip4_endpoint& endpoint, void* buffer, int offset, size_t size);

        virtual int receive(ip4_endpoint* endpoint, void* buffer, int offset, size_t size);

        int available() { return m_dramSocket->available(); }
    #ifdef METRONOME_DEBUG
        void reset();

        uint32_t get_bytes_sent()           { return m_bytesSend; }
        uint32_t get_bytes_received()       { return m_bytesReceived;  }
        uint32_t get_datagrams_sent()       { return m_dRamSend; }
        uint32_t get_datagrams_received()   { return m_dRamReceived; }
    #endif
    protected:
        virtual void on_connecting() { }
        virtual void on_connected() { }
        virtual void on_disconnecting() { }
        virtual void on_disconnected() { }

        virtual void on_send(const ip4_endpoint& endpoint, int send) { }
        virtual void on_receive(const ip4_endpoint& endpoint, void* buffer, int offset, size_t size) { }

        virtual void on_joinmulticast_group(const ip4_address& ip) { }
        virtual void on_leftmulticast_group(const ip4_address& ip) { }
    protected:
        dgram_socket* create_socket(bool use_lite = false);
    protected:
        dgram_socket* m_dramSocket;
        ip4_endpoint m_ipEndpoint;
        ip4_endpoint m_ipReciveEndpoint;
        bool m_bIsMulticast;
        bool m_bIsReuseAddress;
        bool m_bIsConnected;
        bool m_bUseLite;
#ifdef METRONOME_DEBUG
    private:
        uint32_t m_bytesPending;
        uint32_t m_bytesSending;
        uint32_t m_bytesSend;
        uint32_t m_bytesReceived;
        uint32_t m_dRamSend;
        uint32_t m_dRamReceived;
#endif
    };
}
#endif // METRONOME_UDP_CLIENT_H
