#ifndef METRONOME_SOCKET_H
#define METRONOME_SOCKET_H

#include <sdkconfig.h>
#include "ip_address.h"
#include "ip_endpoint.h"

#include <lwip/api.h>
#include <lwip/sockets.h>

#include <mn_config.hpp>
#include <mn_autolock.hpp>

namespace metronome {
    class socket {
    public:
        enum class protocol : int {
            unspec = 0,
            ip = IPPROTO_IP,
            icmp = IPPROTO_ICMP,
            tcp = IPPROTO_TCP,
            udp = IPPROTO_UDP,
#if LWIP_IPV6 == 1
            ipv6 = IPPROTO_IPV6,
            icmpv6 = IPPROTO_ICMPV6,
#endif // LWIP_IPV6
            udp_lite = IPPROTO_UDPLITE,
            raw = IPPROTO_RAW
        };
        enum class type : int {
            unspec = -1,
            stream = SOCK_STREAM,
            dgram = SOCK_DGRAM,
            raw = SOCK_RAW,
            MAX
        };
        enum class flags : int {
            None = 0x00,
            out_of_band = MSG_OOB,
            peek = MSG_PEEK,
            wait_all = MSG_WAITALL,
            nonblocking = MSG_DONTWAIT,
            more = MSG_MORE,
            nosignal = MSG_NOSIGNAL
        };
        enum class selectmode : int {
            read = SHUT_RD,
            write = SHUT_WR,
            rw = SHUT_RDWR
        };
        enum class option_level : int {
            socket = 0xffff,
            ip = (int)protocol::ip,
        #if LWIP_IPV6 == 1
            ipv6 = (int)protocol::ipv6,
        #endif // LWIP_IPV6
            tcp = (int)protocol::tcp,
            udp = (int)protocol::udp,
            udp_lite = (int)protocol::udp_lite,
        };
        enum class option_name : int {
            debug = SO_DEBUG,
            acceptconn = SO_ACCEPTCONN,
            dont_route = SO_DONTROUTE,
            use_loopback = SO_USELOOPBACK,
            linger = SO_LINGER,
            nolinger = SO_DONTLINGER,
            oob_inline = SO_OOBINLINE,
            reuse_port = SO_REUSEPORT,
            reuse_addr = SO_REUSEADDR,
            sendbuffer = SO_SNDBUF,
            recivebuffer = SO_RCVBUF,
            send_lowat = SO_SNDLOWAT,
            recive_lowat = SO_RCVLOWAT,
            send_timeout = SO_SNDTIMEO,
            recive_timeout = SO_RCVTIMEO,
            error = SO_ERROR,
            get_type = SO_TYPE,
            commected_timeout = SO_CONTIMEO,
            no_check = SO_NO_CHECK,
            bind_device = SO_BINDTODEVICE,

            add_membership = IP_ADD_MEMBERSHIP, //  3
            drop_membership = IP_DROP_MEMBERSHIP,
        };
    public:
        using protocol_type = protocol;
        using protocol_family = address_family;
        using socket_flags = flags;
        using socket_type = type;
        using select_mode = selectmode;
        using socket_option_level = option_level;
        using socket_option_name = option_name;
        using handle_type = int;
    protected:
        socket(handle_type& hndl, ip4_endpoint* endp = nullptr);
    public:
        socket(const address_family& fam, const socket_type& type, const protocol_type& protocol);
        virtual ~socket() { close(); }

        void close() { lwip_close(m_iHandle); }
        int available();

        ip4_endpoint* get_local()   { return get_endpoint(true); }
        ip4_endpoint* get_remote()  { return get_endpoint(false); }

        int get_recv_timeout()      { return m_recvTimeout; }
        int get_send_timeout()      { return m_sendTimeout; }

        void set_recv_timeout(int timeout);
        void set_send_timeout(int timeout);

        bool bind(ip4_endpoint local_ep);
        bool bind(int port);
        bool bind(ip4_address ip, unsigned int port);

        //bool poll(int msec, select_mode mode );

        int get_last_error() { return m_iLastError; }
    public:
        int set_options(const socket_option_level& opt, const socket_option_name& name, int value);
        int set_options(const socket_option_level& opt, const socket_option_name& name, bool value);
        int set_options(const socket_option_level& opt, const socket_option_name& name, void* value, uint32_t size);

        int get_option_int(const socket_option_level& opt, const socket_option_name& name);
        bool get_option_bool(const socket_option_level& opt, const socket_option_name& name);
        void get_option_ex(const socket_option_level& opt, const socket_option_name& name, void* value, uint32_t size);
    protected:
        int lock(TickType_t timeout = portMAX_DELAY) { return m_bLock.lock(timeout); }
        int unlock() { return m_bLock.unlock(); }
    protected:
        ip4_endpoint* get_endpoint(bool local);
    protected:
        handle_type m_iHandle;
        bool m_isBlocking; // true
        ip4_endpoint* m_pEndPoint;
        int m_recvTimeout;
        int m_sendTimeout;
        int m_iLastError;
    protected:
        mn::LockType_t m_bLock;
        mn::LockType_t m_bLockSend;
        mn::LockType_t m_bLockRecive;
    };

    class stream_socket : public socket {
    public:
        using handle_type = int;
        using socket_flags = typename socket::socket_flags;
        using socket_type = typename socket::socket_type;
        using protocol_type = typename socket::protocol_type;

        stream_socket(const address_family& fam = address_family::InterNetworkV4)
            : socket(fam, socket_type::stream, protocol_type::unspec) { }

        int recive(char* buffer, int size, socket_flags socketFlags = socket_flags::None)
            { return recive(buffer, 0, size, socketFlags); }
        int recive(char* buffer, int offset, int size, socket_flags socketFlags);

        int send(char* buffer, int size, socket_flags socketFlags = socket_flags::None)
            { return send(buffer, 0, size, socketFlags); }

        int send(char* buffer, int offset, int size, socket_flags socketFlags);

        bool connect(ip4_endpoint remote_ep);
        bool listen(int backLog);

        stream_socket* accept();
    protected:
        stream_socket(handle_type& hndl, ip4_endpoint* endp = nullptr)
            : socket(hndl, endp) { }
    };

    class dgram_socket : public socket {
    public:
        using handle_type = int;
        using socket_flags = typename socket::socket_flags;
        using socket_type = typename socket::socket_type;
        using protocol_type = typename socket::protocol_type;

        dgram_socket(const address_family& fam = address_family::InterNetworkV4, bool lite = false)
            : socket(fam,  socket_type::dgram, lite ? protocol_type::udp_lite :  protocol_type::unspec) { }

        int send_to(char* buffer, int size, ip4_endpoint* ep, socket_flags socketFlags  = socket_flags::None)
            { return send_to(buffer, 0, size, socketFlags, ep); }
        int send_to(char* buffer, int offset, int size, socket_flags socketFlags, ip4_endpoint* ep);

        int recive_from(char* buffer, int size, ip4_endpoint* ep, socket_flags socketFlags  = socket_flags::None)
            { return recive_from(buffer, 0, size, socketFlags, ep); }
        int recive_from(char* buffer, int offset, int size, socket_flags socketFlags, ip4_endpoint* ep);

        bool bind_multicast(ip4_endpoint local_ep);
        bool bind_multicast(ip4_address ip, unsigned int port);
    protected:
        dgram_socket(handle_type& hndl, ip4_endpoint* endp = nullptr)
            : socket(hndl, endp) { }
    private:
        ip4_address m_ipMultiCast;
    };
}
#endif // METRONOME_SOCKET_H
