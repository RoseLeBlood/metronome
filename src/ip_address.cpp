#include "ip_address.h"

#include <exception>
#include <esp_log.h>
#include <string>

#define TAG "ip_address"
namespace metronome {
    ip4_address::ip4_address()
        : ip_address(address_family::InterNetworkV4), as_int32(0) { }

    ip4_address::ip4_address(uint32_t newAddress)
        : ip_address(address_family::InterNetworkV4), as_int32(newAddress) { }

    ip4_address::ip4_address(uint8_t address[METRONOME_IPV4_ADDRESS_BYTES])
        : ip_address(address_family::InterNetworkV4){

        as_array[0] = address[0];
        as_array[1] = address[1];
        as_array[2] = address[2];
        as_array[3] = address[3];
    }
    ip4_address::ip4_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d )
        : ip_address(address_family::InterNetworkV4) {
        as_array[0] = a;
        as_array[1] = b;
        as_array[2] = c;
        as_array[3] = d;
    }
    ip4_address::ip4_address(std::string address)
        : ip_address(address_family::InterNetworkV4) {

        uint16_t acc = 0; // Accumulator
        uint8_t dots = 0;

        for(int i = 0; i < address.length(); i++) {
            char c = address[i];
            if (c >= '0' && c <= '9') {
                acc = acc * 10 + (c - '0');
                if (acc > 255) { as_int32 = 0; return; }
            } else if (c == '.') {
                if (dots == 3) { as_int32 = 0; return; }
                as_array[dots++] = acc;
                acc = 0;
            }
            else { as_int32 = 0; return; }
        }

        if (dots != 3) { as_int32 = 0; return; }
        as_array[3] = acc;
    }

    uint8_t* ip4_address::get_bytes() {
        return as_array;
    }

    bool ip4_address::is_equel(const ip4_address& ipOther) {
        if(m_aFamily != ipOther.m_aFamily) return false;
        return as_int32 == ipOther.as_int32;
    }
    ip4_address& ip4_address::operator=(uint32_t address) {
        as_int32 = address;
        return *this;
    }
    bool ip4_address::operator == (const uint8_t* addr) {
        return memcmp(addr, as_array, sizeof(as_array) )  == 0;
    }

    std::string ip4_address::to_string() {
        char szRet[16];
        sprintf(szRet,"%u.%u.%u.%u", as_array[0], as_array[1], as_array[2], as_array[3]);
        return std::string(szRet);
    }
    ip4_address ip4_address::calc_broadcast(const ip4_address& subnet) {
        uint8_t broadcast[METRONOME_IPV4_ADDRESS_BYTES];

        for (int i = 0; i < METRONOME_IPV4_ADDRESS_BYTES; i++) {
            broadcast[i] = (~(*this)[i] | subnet[i]);
        }
        return ip4_address(broadcast);
    }

    ip6_address::ip6_address()
        : ip_address(address_family::InterNetworkV6)
        { m_ScopeId = 0; memset(m_Numbers, 0, sizeof(m_Numbers)); }

    ip6_address::ip6_address(uint8_t adress[METRONOME_IPV6_ADDRESS_BYTES], int scopid)
        : ip_address(address_family::InterNetworkV6) {

        for (signed char i = 0; i < METRONOME_NUMBER_OF_LABELS; i++) {
            m_Numbers[i] = (adress[i * 2] * 256 + adress[i * 2 + 1]);
        }
        if ( scopid < 0 || scopid > 0x00000000FFFFFFFF ) {
            ESP_LOGE(TAG, "scopid out of range");
        }

        m_ScopeId = scopid;
    }

    ip6_address::ip6_address(const ip6_address& ip)
        : ip_address(address_family::InterNetworkV6) {
        mempcpy(m_Numbers, ip.m_Numbers, sizeof(m_Numbers));
        m_ScopeId = 0;
    }
    ip6_address::ip6_address(std::string str_ip)
        : ip_address(address_family::InterNetworkV6) {
        char * pos = (char*)str_ip.c_str();

        if(str_ip.length() == 39) {
            for(int8_t i = 0; i < 16; i+=2, pos += 5) {
                if(!sscanf(pos, "%2hhx", &m_Numbers[i]) || !sscanf(pos+2, "%2hhx", &m_Numbers[i+1])) {
                    memset(m_Numbers, 0, sizeof(m_Numbers)); return;
                }
            }
        } else {
            memset(m_Numbers, 0, sizeof(m_Numbers));
        }
        m_ScopeId = 0;
    }
    // IPv4 192.168.1.1 maps as ::FFFF:192.168.1.1
    ip6_address::ip6_address(const ip4_address& ip4)
        : ip_address(address_family::InterNetworkV6) {

       // unsigned short labels[METRONOME_NUMBER_OF_LABELS];
        //labels[5] = 0xFFFF;
        /*labels[6] = (ip4.as_array[0] >> 8) | (ip4.as_array[1] << 8);
        labels[7] = ip4.as_array[2] | ip4.as_array[3];*/
        ESP_LOGE(TAG, "future use - ip6_address::ip6_address(const ip4_address& ip4)");
    }
    ip4_address ip6_address::as_ip4() {
        /*long address = ((((uint32_t)m_Numbers[6] & 0x0000FF00u) >> 8) | (((uint32_t)m_Numbers[6] & 0x000000FFu) << 8)) |
                        (((((uint32_t)m_Numbers[7] & 0x0000FF00u) >> 8) | (((uint32_t)m_Numbers[7] & 0x000000FFu) << 8)) << 16);
        */
        return ip4_address((uint32_t)0);
    }
    uint8_t* ip6_address::get_bytes() {
        uint8_t* _retBytes;

        _retBytes = new uint8_t[METRONOME_IPV6_ADDRESS_BYTES];

        for ( int i = 0, j = 0; i < METRONOME_NUMBER_OF_LABELS; i++) {
            _retBytes[j++] = ((m_Numbers[i] >> 8) & 0xFF);
            _retBytes[j++] = ((m_Numbers[i]     ) & 0xFF);
        }

        return _retBytes;
    }
    bool ip6_address::is_ip4mapped() {
        for (int i = 0; i < 5; i++) {
            if (m_Numbers[i] != 0) return false;
        }
        return (as_short[5] == 0xFFFF);
    }
    void ip6_address::set_scopeid(long id) {
        m_ScopeId = id;
    }
    bool ip6_address::is_equel(const ip6_address& ipOther) {
        if(m_ScopeId != ipOther.m_ScopeId) return false;

        for ( int8_t i = 0; i < METRONOME_NUMBER_OF_LABELS; i++) {
            if (ipOther.m_Numbers[i] != m_Numbers[i]) return false;
        }
        return true;
    }

    ip6_address& ip6_address::operator = (const ip6_address& ip) {
        m_aFamily = ip.m_aFamily;
        m_ScopeId  = ip.m_ScopeId;
        mempcpy(m_Numbers, ip.m_Numbers, sizeof(m_Numbers) );

        return *this;
    }
    std::string ip6_address::to_string() {
        char szRet[40];
        sprintf(szRet,"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                m_Numbers[0], m_Numbers[1], m_Numbers[2], m_Numbers[3],
                m_Numbers[4], m_Numbers[5], m_Numbers[6], m_Numbers[7],
                m_Numbers[8], m_Numbers[9], m_Numbers[10], m_Numbers[11],
                m_Numbers[12], m_Numbers[13], m_Numbers[14], m_Numbers[15]);
        return std::string(szRet);
    }
}


