#ifndef METRONOME_IF_INTERFACE_H
#define METRONOME_IF_INTERFACE_H

#include <esp_interface.h>
#include <esp_wifi.h>

#include "ip_address.h"

namespace metronome {

    class if_interface {
    protected:
        if_interface(esp_interface_t type);

        virtual esp_netif_t*    create_netif() = 0;


        virtual std::string     get_mac() = 0;
        virtual uint8_t*        get_mac(uint8_t* mac) = 0;
        virtual std::string     get_hostname() = 0;
        virtual bool            set_hostname(std::string hostname) = 0;
        virtual ip4_address     get_ip() = 0;
        virtual ip4_address     get_broadcast() = 0;
        virtual ip4_address     get_gateway() = 0;
        virtual ip4_address     get_netmask() = 0;
        virtual esp_netif_t*    get_netif() { return m_pNetIf; }
        esp_interface_t&        get()       { return m_ifInterface; }
    protected:
        esp_interface_t m_ifInterface;
        esp_netif_t*    m_pNetIf;
    };

    class ethernet_interface : public if_interface {
    protected:
        ethernet_interface() : if_interface(ESP_IF_ETH) { }

        virtual esp_netif_t* create_netif();
    };

    class wifi_interface : public if_interface {
    protected:
        wifi_interface(esp_interface_t type)
            : if_interface(type) { }

        virtual esp_netif_t*    create_netif();

        virtual wifi_mode_t     get_mode();
        virtual std::string     get_ssid();
    };
}

#endif // METRONOME_IF_INTERFACE_H
