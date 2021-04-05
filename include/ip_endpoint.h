#ifndef METRONOME_IP_ENDPOINT_H
#define METRONOME_IP_ENDPOINT_H

#include "ip_address.h"
namespace metronome {

    class ip4_endpoint {
    public:
        ip4_endpoint(const uint16_t& port = 0)
            : ip4_endpoint(METRONOME_IPV4_ADDRESS_ANY, port) { }

        ip4_endpoint(const ip4_address& ip, const uint16_t& port)
            :  m_iPort(8289), m_ipAdress(ip)  {
            if(port < 9999) m_iPort = port;
        }
        ip4_endpoint(const ip4_endpoint& pOther)
            : m_iPort(pOther.m_iPort), m_ipAdress(pOther.m_ipAdress) { }

        uint16_t get_port()             { return m_iPort; }
        ip4_address get_ip()             { return m_ipAdress; }

        void set_port(const uint16_t& port) {
            if(port < 9999) m_iPort = port;
        }

        bool operator == (const ip4_endpoint& pOther) {
            if(m_iPort != pOther.m_iPort) return false;
            return m_ipAdress == pOther.m_ipAdress;
        }
        bool operator != (const ip4_endpoint& pOther) {
            if(m_iPort == pOther.m_iPort) return false;
            return m_ipAdress != pOther.m_ipAdress;
        }
        ip4_endpoint& operator = (const ip4_endpoint& pOther) {
            m_iPort = pOther.m_iPort;
            m_ipAdress = pOther.m_ipAdress;
            return *this;
        }
    private:
        uint16_t m_iPort;
        ip4_address m_ipAdress;
    };
}
#endif // METRONOME_IP_ENDPOINT_H
