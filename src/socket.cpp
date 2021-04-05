#include "socket.h"
#include <stdint.h>

namespace metronome {
    socket::socket(handle_type& hndl, ip4_endpoint* endp)
        : m_iHandle(hndl) {
            m_pEndPoint = endp;
            m_isBlocking = true;
            m_recvTimeout = m_sendTimeout = 0xffffffff;
            m_iLastError = 0;
    }

    socket::socket(const address_family& fam, const socket_type& type, const protocol_type& protocol) {
            m_iHandle = lwip_socket(static_cast<int>(fam),
                                    static_cast<int>(type),
                                    static_cast<int>(protocol) );
            m_pEndPoint = nullptr;
            m_isBlocking = true;
            m_recvTimeout = m_sendTimeout = 0xffffffff;
            m_iLastError = m_iHandle != -1 ? 0 : m_iHandle;
    }
    int socket::available() {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return 0;
        int cBytes = 0;

        m_iLastError = lwip_ioctl(m_iHandle, FIONREAD, &cBytes);

        return cBytes;
    }
    ip4_endpoint* socket::get_endpoint(bool local) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return nullptr;
        ip4_endpoint *_ret = nullptr;

        struct sockaddr_in name;
        unsigned int name_len = sizeof(name);

        if (local) {
            m_iLastError = lwip_getsockname(m_iHandle, (struct sockaddr *) &name, &name_len);

        } else {
            m_iLastError = lwip_getpeername(m_iHandle, (struct sockaddr *) &name, &name_len);
        }

        _ret = new ip4_endpoint(ip4_address(name.sin_addr.s_addr), lwip_htons(name.sin_port));
        if(local) m_pEndPoint = _ret;

        return _ret;
    }
    int socket::set_options(const socket_option_level& opt, const socket_option_name& name, int value) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return -1;

        if(name == socket_option_name::send_timeout) m_sendTimeout = value;
        if(name == socket_option_name::recive_timeout) m_recvTimeout = value;

        socklen_t _size = sizeof(value);
        return (m_iLastError = setsockopt(m_iHandle, static_cast<int>(opt), static_cast<int>(name), &value, _size));
    }
    int socket::set_options(const socket_option_level& opt, const socket_option_name& name, bool value) {
        return set_options(opt, name, value ? 1 : 0 );
    }
    int socket::set_options(const socket_option_level& opt, const socket_option_name& name, void* value, uint32_t size) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return -1;
        return (m_iLastError = setsockopt(m_iHandle, static_cast<int>(opt), static_cast<int>(name), value, size));
    }

    int socket::get_option_int(const socket_option_level& opt, const socket_option_name& name) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return -1;

        int value = 0;
        socklen_t _size = sizeof(value);
        m_iLastError = getsockopt(m_iHandle, static_cast<int>(opt), static_cast<int>(name), &value, &_size);

        return value;
    }
    bool socket::get_option_bool(const socket_option_level& opt, const socket_option_name& name) {
        return get_option_int(opt, name) == 1;
    }

    void socket::get_option_ex(const socket_option_level& opt, const socket_option_name& name, void* value, uint32_t size) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) { value = NULL; return; }
        m_iLastError = getsockopt(m_iHandle, static_cast<int>(opt), static_cast<int>(name), &value, &size);
    }
    void socket::set_recv_timeout(int timeout) {
        set_options(socket_option_level::socket, socket_option_name::recive_timeout, timeout);
    }
    void socket::set_send_timeout(int timeout) {
        set_options(socket_option_level::socket, socket_option_name::send_timeout, timeout);
    }
    bool socket::bind(ip4_endpoint local_ep) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return false;

        ip4_address ip = local_ep.get_ip();
        unsigned int port = local_ep.get_port();

        struct sockaddr_in addr;
        memset((char *) &addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = (in_addr_t)ip;
        m_iLastError = lwip_bind(m_iHandle, (struct sockaddr*)&addr, sizeof(addr) );

        if(m_iLastError == -1) {
            ESP_LOGE("socket", "could not bind socket: %d", errno);
            return false;
        }
        fcntl(m_iHandle, F_SETFL, O_NONBLOCK);

        return true;
    }
    bool socket::bind(const unsigned int& port) {
        return bind(ip4_endpoint(METRONOME_IPV4_ADDRESS_ANY, port) );
    }
    bool socket::bind(ip4_address ip, const unsigned int& port) {
        return bind(ip4_endpoint(ip, port) );
    }
    bool socket::get_peername(ip4_address& ipPeerAddress, uint16_t& iPeerPort) {
        bool _ret = false;
        struct sockaddr_in stPeer;
        memset(&stPeer, 0, sizeof(struct sockaddr));
        socklen_t iLen = sizeof(sockaddr);

        m_iLastError = getpeername(m_iHandle, (struct sockaddr *)&stPeer, &iLen);
        if(m_iLastError != 0) {
            ESP_LOGE("socket", "could not getpeername: %d", errno);
        } else {
            ipPeerAddress = ip4_address(stPeer.sin_addr.s_addr);
            iPeerPort = stPeer.sin_port;
            _ret = true;
        }
        return _ret;
    }
    bool socket::get_peername(ip4_endpoint& endpoint) {
        bool _ret = false;
        struct sockaddr_in stPeer;
        memset(&stPeer, 0, sizeof(struct sockaddr));
        socklen_t iLen = sizeof(sockaddr);

        m_iLastError = getpeername(m_iHandle, (struct sockaddr *)&stPeer, &iLen);
        if(m_iLastError != 0) {
            ESP_LOGE("socket", "could not getpeername: %d", errno);
        } else {
            endpoint = ip4_endpoint(ip4_address(stPeer.sin_addr.s_addr), stPeer.sin_port);
            _ret = true;
        }
        return _ret;
    }
//---------------------UDP DataGram ---------------------------------------------------------------
    bool dgram_socket::bind_multicast(ip4_endpoint local_ep) {
        bool _retBool = false;

        if(bind((int)local_ep.get_port())) {
            struct ip_mreq mreq;
            mreq.imr_multiaddr.s_addr = (in_addr_t)local_ep.get_ip();
            mreq.imr_interface.s_addr = INADDR_ANY;

            if (setsockopt(m_iHandle, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
                 ESP_LOGE("dgram_socket", "could not join igmp: %d", errno);
            } else {
                _retBool = true;
                m_ipMultiCast = local_ep.get_ip();
            }
        }
        return _retBool;
    }
    bool dgram_socket::bind_multicast(ip4_address ip, const unsigned int& port) {
        return bind_multicast(ip4_endpoint(ip, port));
    }

    int dgram_socket::send_to(char* buffer, int offset, int size, socket_flags socketFlags, ip4_endpoint* ep) {
        mn::autolock_t lock(m_bLockSend);

        if(m_iHandle == -1) return -1;

        ip4_address ip = ep->get_ip();
        unsigned int port = ep->get_port();

        struct sockaddr_in addr;
        unsigned int addrlen=sizeof(addr);

        memset((char *) &addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = (in_addr_t)ip;

        m_iLastError = lwip_sendto(m_iHandle, &buffer[offset], size-offset, static_cast<int>(socketFlags),
                           (struct sockaddr*)&addr,
                           addrlen );
        return m_iLastError;
    }
    int dgram_socket::recive_from(char* buffer, int offset, int size, socket_flags socketFlags, ip4_endpoint* ep) {
        mn::autolock_t lock(m_bLockRecive);

        if(m_iHandle == -1) return -1;

        ip4_address ip = ep->get_ip();
        unsigned int port = ep->get_port();

        struct sockaddr_in addr;
        unsigned int addrlen=sizeof(addr);

        memset((char *) &addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = (in_addr_t)ip;

        m_iLastError = lwip_recvfrom(m_iHandle, &buffer[offset], size-offset, static_cast<int>(socketFlags),
                      (struct sockaddr*)&addr,
                      &addrlen );
        return m_iLastError;
    }
    void dgram_socket::set_nocheak(bool value) {
        set_options(socket::option_level::socket, socket::option_name::no_check, value);
    }
    bool dgram_socket::get_nocheak() {
        return socket::get_option_bool(socket::option_level::socket, socket::option_name::no_check);
    }

//---------------------DATAGRAM LITE ------------------------------------------------------------

#if LWIP_UDP && LWIP_UDPLITE
    //UDPLITE_SEND_CSCOV
    void dgram_socket_lite::set_send_coverage(uint8_t val) {
        set_options(socket::option_level::udp_lite,
                           socket::option_name::udplite_send_cscov,
                           static_cast<int>(val) );
    }
    uint8_t dgram_socket_lite::get_send_coverage() {
        return get_option_int(socket::option_level::udp_lite, socket::option_name::udplite_send_cscov);
    }

    // UDPLITE_RECV_CSCOV
    void dgram_socket_lite::set_recive_coverage(uint8_t val) {
        set_options(socket::option_level::udp_lite,
                           socket::option_name::udplite_recv_cscov,
                           static_cast<int>(val) );
    }
    uint8_t dgram_socket_lite::get_recive_coverage() {
        return get_option_int(socket::option_level::udp_lite, socket::option_name::udplite_recv_cscov);
    }
#endif // LWIP_UDP
//---------------------TCP STREAM ---------------------------------------------------------------

    int stream_socket::send(char* buffer, int offset, int size, socket_flags socketFlags) {
        mn::autolock_t lock(m_bLockSend);

        if(m_iHandle == -1) return -1;

        m_iLastError = lwip_send(m_iHandle, &buffer[offset], size-offset, static_cast<int>(socketFlags));
        return m_iLastError;
    }
    int stream_socket::recive(char* buffer, int offset, int size, socket_flags socketFlags) {
        mn::autolock_t lock(m_bLockRecive);

        if(m_iHandle == -1) return -1;

        m_iLastError = lwip_recv(m_iHandle, &buffer[offset], size-offset, static_cast<int>(socketFlags));
        return m_iLastError;
    }

    bool stream_socket::listen(int backLog) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return false;

        m_iLastError = lwip_listen(m_iHandle, backLog) ;

        return m_iLastError != -1;
    }
    stream_socket* stream_socket::accept() {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return nullptr;

        int clientfd;
        struct sockaddr_in client_addr;
        unsigned int addrlen=sizeof(client_addr);
        stream_socket* socket_return = nullptr;

        clientfd = lwip_accept(m_iHandle, (struct sockaddr*)&client_addr, &addrlen);
        if(clientfd >= 0) {
            auto port = lwip_ntohs(client_addr.sin_port);
            auto ip = ip4_address( (uint32_t)(client_addr.sin_addr.s_addr) );

            socket_return = new stream_socket(clientfd, new ip4_endpoint( ip, port) );
            mn::autolock_t c_lock(socket_return->m_bLock);

        } else {
            m_iLastError = clientfd;
        }
        return socket_return;
    }

    bool stream_socket::connect(ip4_endpoint remote_ep) {
        mn::autolock_t lock(m_bLock);

        if(m_iHandle == -1) return false;

        ip4_address ip = remote_ep.get_ip();
        unsigned int port = remote_ep.get_port();

        struct sockaddr_in addr;
        memset((char *) &addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = (in_addr_t)ip;

        m_iLastError = lwip_connect(m_iHandle, (struct sockaddr*)&addr, sizeof(addr) ) ;
        return m_iLastError != -1;
    }

}

