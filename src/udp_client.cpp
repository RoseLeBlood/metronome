#include "udp_client.h"
#include <lwip/sockets.h>
#include <esp_log.h>

namespace metronome {

    udp_client::udp_client(ip4_address address, int port, bool use_lite)
            : udp_client(ip4_endpoint(address, port), use_lite) {}

    udp_client::udp_client(ip4_endpoint endpoint, bool use_lite)
        : m_dramSocket(nullptr), m_ipEndpoint(endpoint),
        m_ipReciveEndpoint(endpoint), m_bIsMulticast(false),
        m_bIsReuseAddress(false), m_bIsConnected(false), m_bUseLite(use_lite) { }

    dgram_socket* udp_client::create_socket() {
        dgram_socket* _socket = nullptr;

        if(!m_bUseLite) _socket = new dgram_socket(address_family::InterNetworkV4);
        else _socket = new dgram_socket_lite(address_family::InterNetworkV4);

        return _socket;
    }
    void udp_client::set_reuse_address(bool is) {
        m_bIsReuseAddress = is;
        m_dramSocket->set_options(socket::option_level::socket, socket::option_name::reuse_addr, m_bIsReuseAddress);
    }

    bool udp_client::connect() {
        if(m_bIsConnected ) return false;

        m_dramSocket = create_socket();
        if(m_dramSocket == nullptr) return false;

        m_dramSocket->set_options(socket::option_level::socket, socket::option_name::reuse_addr, m_bIsReuseAddress);

        on_connecting();

        if(m_bIsMulticast) {
            m_bIsConnected = m_dramSocket->bind(ip4_endpoint(METRONOME_IPV4_ADDRESS_ANY, m_ipEndpoint.get_port()) );
        } else {
            m_bIsConnected = m_dramSocket->bind(m_ipEndpoint);
        }

        if(m_bIsConnected) {
            m_ipReciveEndpoint = ip4_endpoint(METRONOME_IPV4_ADDRESS_ANY, 0);

        #ifdef METRONOME_DEBUG
            m_bytesSend = 0;
            m_bytesReceived = 0;
            m_dRamSend = 0;
            m_dRamReceived = 0;
        #endif

            on_connected();
        }
        return m_bIsConnected;
    }

    bool udp_client::disconnect() {
        if(!m_bIsConnected ) return false;

        on_disconnecting();

        if(m_dramSocket) {
            m_dramSocket->close();
            delete m_dramSocket; m_dramSocket = nullptr;
        }
        m_bIsConnected = false;

        on_disconnected();

        return true;
    }
    bool udp_client::reconnect() {
        if (!disconnect()) return false;
        return connect();
    }
    void udp_client::setup_multicast(bool is) {
        m_bIsReuseAddress = is;
        m_bIsMulticast = is;
    }
    void udp_client::join_multicast_group(ip4_address& ip) {
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = (in_addr_t)ip;
        mreq.imr_interface.s_addr = INADDR_ANY;

        if(m_dramSocket->set_options(  socket::option_level::ip,
                                    socket::option_name::add_membership,
                                    &mreq, sizeof(mreq) ) < 0) {

            ESP_LOGE("udp_client", "could not join igmp: %d", errno);
        }

        on_joinmulticast_group(ip);
    }
    void udp_client::left_multicast_group(ip4_address& ip) {
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = (in_addr_t)ip;
        mreq.imr_interface.s_addr = INADDR_ANY;

        if(m_dramSocket->set_options(  socket::option_level::ip,
                                    socket::option_name::drop_membership,
                                    &mreq, sizeof(mreq) ) < 0) {

            ESP_LOGE("udp_client", "could not left igmp: %d", errno);
        }

        on_leftmulticast_group(ip);
    }
    int udp_client::send(const ip4_endpoint& endpoint, void* buffer, int offset, size_t size) {
        if(!m_bIsConnected) return 0;
        if(size == 0) return 0;

        ip4_endpoint _endpoint = endpoint;

        int send = m_dramSocket->send_to((char*)buffer, 0, size, socket::socket_flags::None, &_endpoint);
        if(send > 0) {
        #ifdef METRONOME_DEBUG
            m_dRamSend++;
            m_bytesSend += sent;
        #endif // METRONOME_DEBUG
            on_send(endpoint, send);
        }
        return send;
    }
    int udp_client::send(const ip4_endpoint& endpoint, void* buffer, size_t size) {
        return send(endpoint, buffer, 0, size);
    }
    int udp_client::receive(ip4_endpoint* endpoint, void* buffer, int offset, size_t size) {
        if(!m_bIsConnected) return 0;
        if(size == 0) return 0;

        int recive = m_dramSocket->recive_from((char*)buffer, 0, size, socket::socket_flags::None, endpoint);
        if(recive > 0) {
        #ifdef METRONOME_DEBUG
            m_dRamReceived++;
            m_bytesReceived += recive;
        #endif // METRONOME_DEBUG
            on_receive(*endpoint, buffer, offset, size);
        }
        return recive;
    }
    #ifdef METRONOME_DEBUG
        void udp_client::reset() {
            m_dRamReceived = 0;
            m_bytesReceived = 0;
            m_dRamSend = 0;
            m_bytesSend = 0;
        }
    #endif

}
