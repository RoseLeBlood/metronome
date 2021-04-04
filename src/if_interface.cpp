#include "if_interface.h"
#include "esp_eth.h"
/*

*/
namespace metronome {

    if_interface::if_interface(esp_interface_t type)
        : m_ifInterface(type) { }

    esp_netif_t* wifi_interface::create_netif() {
        if(get_mode()  == WIFI_MODE_AP) {
            m_pNetIf = esp_netif_create_default_wifi_ap();
        } else if(get_mode()  == WIFI_MODE_STA) {
            m_pNetIf = esp_netif_create_default_wifi_sta();
        }
        return m_pNetIf;
    }
    esp_netif_t* ethernet_interface::create_netif() {
        return nullptr;
    }

    std::string wifi_interface::get_ssid() {
        wifi_config_t info;
        auto _tmode = get_mode();

        esp_wifi_get_config((_tmode == WIFI_MODE_AP) ? ESP_IF_WIFI_AP : ESP_IF_WIFI_STA, &info);

        return (_tmode == WIFI_MODE_AP) ? std::string(reinterpret_cast<char*>(info.ap.ssid))
                                        : std::string(reinterpret_cast<char*>(info.sta.ssid));
    }
    wifi_mode_t wifi_interface::get_mode() {
        return m_ifInterface == ESP_IF_WIFI_AP ? WIFI_MODE_AP : WIFI_MODE_STA;
    }
}
