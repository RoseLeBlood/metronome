#ifndef METRONOME_IP_ADDRESS_H
#define METRONOME_IP_ADDRESS_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#include <array>

#define METRONOME_IPV4_ADDRESS_BYTES        4
#define METRONOME_IPV6_ADDRESS_BYTES        16
#define METRONOME_NUMBER_OF_LABELS          8

#define METRONOME_IPV4_ADDRESS_ANY          metronome::ip4_address( 0,0,0,0 )
#define METRONOME_IPV4_ADDRESS_LOOPBACK     metronome::ip4_address( 127,0,0,1 )
#define METRONOME_IPV4_ADDRESS_BROADCAST    metronome::ip4_address( 255,255,255,255 )
#define METRONOME_IPV4_ADDRESS_NONE         METRONOME_IP_ADDRESS_BROADCAST

#define METRONOME_LOOPBACK_MASK             0x00000000000000FF

#define METRONOME_IPV6_ADDRESS_ANY          metronome::ip6_address( { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 })
#define METRONOME_IPV6_ADDRESS_LOOPBACK     metronome::ip6_address( { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 })
#define METRONOME_IPV6_ADDRESS_NONE         metronome::ip6_address( { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 })

namespace metronome {
    enum class address_family {
            Unknown             = -1,   // Unknown
            Unspecified         = AF_UNSPEC,    // unspecified
            InterNetworkV4      = AF_INET,    // internetwork: UDP, TCP, etc.
            InterNetworkV6      = AF_INET6,   // Internetwork Version 6
            Max,   // Max
    }; // enum AddressFamily


    class ip_address {
    public:
        ip_address(address_family fam) : m_aFamily(fam) { }

        virtual std::string     to_string() = 0;
        virtual uint8_t*        get_bytes() = 0;
        virtual bool            is_loopback() = 0;
        virtual bool            is_broadcast() = 0;
        virtual address_family  get_family()    { return m_aFamily; }


    protected:
        address_family  m_aFamily;
    };

    class ip4_address : public ip_address {
    public:
        union {
            uint8_t  as_array[METRONOME_IPV4_ADDRESS_BYTES];
            uint32_t as_int32;
        };

        ip4_address();
        explicit ip4_address(uint32_t newAddress);
        ip4_address(uint8_t adress[METRONOME_IPV4_ADDRESS_BYTES]);
        ip4_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d );
        ip4_address(std::string str_ip);

        uint8_t*       get_bytes();
        virtual bool   is_equel(const ip4_address& ipOther);

        ip4_address& operator=(uint32_t address);

        bool operator == (const ip4_address& ipOther){ return  is_equel(ipOther); }
        bool operator != (const ip4_address& ipOther){ return !is_equel(ipOther); }
        bool operator == (const uint8_t* addr);

        bool is_loopback()  { return (*this == METRONOME_IPV4_ADDRESS_LOOPBACK); }
        bool is_broadcast() { return (*this == METRONOME_IPV4_ADDRESS_BROADCAST); }

        uint8_t operator[](int index) const {
            return as_array[index];
        }
        uint8_t& operator[](int index) {
            return as_array[index];
        }
        operator uint32_t() const { return as_int32; }


        virtual std::string     to_string();

        ip4_address calc_broadcast(const ip4_address& subnet);
    };

    class ip6_address : public ip_address {
    public:
        union {
            uint8_t         m_Numbers[METRONOME_IPV6_ADDRESS_BYTES];
            uint16_t        as_short[8];
            uint32_t        as_int[4];
        };
        ip6_address();
        ip6_address(uint8_t adress[METRONOME_IPV6_ADDRESS_BYTES], int scopid = 0);
        ip6_address(std::string str_ip);
        ip6_address(const ip6_address& ipOther);

        ip6_address(const ip4_address& ip4);

        long                    get_scopeid()   { return m_ScopeId; }
        void                    set_scopeid(long);

        virtual std::string     to_string();
        virtual uint8_t*        get_bytes();
        virtual bool            is_loopback() { return (*this == METRONOME_IPV6_ADDRESS_LOOPBACK); }
        virtual bool            is_broadcast() { return false; }
        virtual bool            is_multicast() { return ( (as_short[0] & 0xFF00 ) == 0xFF00 );  }
        virtual bool            is_linklocal() { return ( (as_short[0] & 0xFFC0 ) == 0xFE80 );  }
        virtual bool            is_sitelocal() { return ( (as_short[0] & 0xFFC0 ) == 0xFEC0 );  }
        virtual bool            is_toredo()    { return (  as_short[0] == 0x2001 && as_short[1] == 0  );  }
        virtual bool            is_ip4mapped();

        virtual bool            is_equel(const ip6_address& ipOther);

        bool operator == (const ip6_address& ipOther){ return  is_equel(ipOther); }
        bool operator != (const ip6_address& ipOther){ return !is_equel(ipOther); }

        ip6_address& operator = (const ip6_address& ip);

        uint8_t operator[](int index) const { return m_Numbers[index]; }
        uint8_t& operator[](int index) { return m_Numbers[index]; }

        ip4_address as_ip4();
    private:
        long            m_ScopeId;
    };
}

#endif // METRONOME_IP_ADDRESS_H
