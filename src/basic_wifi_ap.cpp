#include "basic_wifi_ap.h"


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <lwip/ip_addr.h>
#include <esp_event.h>
#include <esp_log.h>

namespace metronome {
    bool basic_wifi_ap::start(std::string ssid, wifi_auth_mode_t mode, std::string passphrase,
                              bool ipv6, int channel, bool ssid_hidden, int max_connection) {

        wifi_config_t wifi_config;

        if( ssid.length() == 0) return false;
        if( (passphrase.length() < 8 ) && (mode != WIFI_AUTH_OPEN ) ) {
                wifi_config.ap.authmode = WIFI_AUTH_OPEN;
                ESP_LOGI("metronome::wifiAp", "passphrase to small set to auth_mode open");
        }
        else wifi_config.ap.authmode = mode;

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        this->setup(&cfg);


        memcpy(wifi_config.ap.ssid, ssid.c_str(), sizeof(wifi_config.ap.ssid) );
        wifi_config.ap.ssid_len = ssid.length();

        if(wifi_config.ap.authmode != WIFI_AUTH_OPEN) {
            memcpy(wifi_config.ap.password, passphrase.c_str(), sizeof(wifi_config.ap.password) );
        }


        wifi_config.ap.ssid_hidden = ssid_hidden;
        wifi_config.ap.channel = channel;
        wifi_config.ap.max_connection = max_connection;
        wifi_config.ap.beacon_interval = 100;

        ESP_LOGI("metronome::wifiAp", "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        if(ipv6) {
            ESP_ERROR_CHECK(tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_AP));
            ESP_LOGI("metronome::wifiAp", "IPv6 enabled");
        }

        return true;
    }
    bool basic_wifi_ap::set_ipinfo(const ip4_adress_t& local_ip, const ip4_adress_t& gateway, const ip4_adress_t& subnet) {
        tcpip_adapter_ip_info_t info;
        info.ip.addr = (local_ip.as_int32);
        info.gw.addr = (gateway.as_int32);
        info.netmask.addr = (subnet.as_int32);
        tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);

        if(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info) == ESP_OK) {
            dhcps_lease_t lease;
            lease.enable = true;
            lease.start_ip.addr = (local_ip.as_int32) + (1 << 24);
            lease.end_ip.addr = (local_ip.as_int32) + (11 << 24);

            tcpip_adapter_dhcps_option(
                TCPIP_ADAPTER_OP_SET,
                ESP_NETIF_REQUESTED_IP_ADDRESS,
                (void*)&lease, sizeof(dhcps_lease_t)
            );

            return tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP) == ESP_OK;
        }
        return false;
    }
    bool basic_wifi_ap::stop(bool wifioff) {
        if(get_mode() == WIFI_MODE_NULL) return false;

        bool ret;

        wifi_config_t wifi_config;
        *wifi_config.ap.ssid = 0;
        *wifi_config.ap.password = 0;

        wifi_config.ap.ssid_len = 0;
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;

         ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_config) == ESP_OK;
         if(wifioff) esp_wifi_stop();

         desetup();
         return ret;
    }
    std::string basic_wifi_ap::get_hostname() {
        if(get_mode() == WIFI_MODE_NULL) return std::string();
        const char * hostname = NULL;

        tcpip_adapter_get_hostname(TCPIP_ADAPTER_IF_AP, &hostname);
        return std::string(hostname);
    }
    bool basic_wifi_ap::set_hostname(std::string hostname) {
        if(get_mode() == WIFI_MODE_NULL) return false;

        return tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, hostname.c_str() ) == ESP_OK;
    }
    uint8_t* basic_wifi_ap::get_mac(uint8_t* mac) {
        if(get_mode() == WIFI_MODE_NULL) return NULL;

        esp_wifi_get_mac(WIFI_IF_AP, mac);
        return mac;
    }
    uint32_t basic_wifi_ap::get_connected() {
        if(get_mode() == WIFI_MODE_NULL) return 0;
        wifi_sta_list_t clients;

        if(esp_wifi_ap_get_sta_list(&clients) == ESP_OK) {
             return clients.num;
        }
        return 0;
    }
    tcpip_adapter_ip_info_t basic_wifi_ap::get_ip_infos() {
        tcpip_adapter_ip_info_t ip;
        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip);

        return ip;
    }
    ip4_address basic_wifi_ap::get_ip() {
        if(get_mode() == WIFI_MODE_NULL) return ip4_address();
        tcpip_adapter_ip_info_t ip = get_ip_infos();

        return ip4_address(ip.ip.addr);
    }
    ip4_address basic_wifi_ap::get_broadcast() {
        if(get_mode() == WIFI_MODE_NULL) return ip4_address();

        return get_gateway().calc_broadcast( get_netmask() );
    }
    ip4_address basic_wifi_ap::get_gateway() {
        if(get_mode() == WIFI_MODE_NULL) return ip4_address();
        tcpip_adapter_ip_info_t ip = get_ip_infos();

        return ip4_address(ip.gw.addr);
    }
    ip4_address   basic_wifi_ap::get_netmask() {
        if(get_mode() == WIFI_MODE_NULL) return ip4_address();
        tcpip_adapter_ip_info_t ip = get_ip_infos();

        return ip4_address(ip.netmask.addr);
    }

    std::string basic_wifi_ap::get_mac() {
        if(get_mode() == WIFI_MODE_NULL) return std::string();

        uint8_t mac[6];
        char macStr[18] = { 0 };
        esp_wifi_get_mac(WIFI_IF_AP, mac);

        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        return std::string(macStr);
    }
    void basic_wifi_ap::on_start(esp_event_base_t event_base,  void* event_data) {

    }
    void basic_wifi_ap::on_stop(esp_event_base_t event_base,  void* event_data) {

    }
    void basic_wifi_ap::on_connected(esp_event_base_t event_base, void* event_data) {

    }
    void basic_wifi_ap::on_disconnected(esp_event_base_t event_base, void* event_data) {

    }
    void basic_wifi_ap::on_probereqrec(esp_event_base_t event_base, void* event_data) {

    }
    void basic_wifi_ap::on_wifi_event(esp_event_base_t event_base, int32_t event_id, void* event_data) {

        switch(event_id) {
        case WIFI_EVENT_AP_STACONNECTED: on_connected(event_base, event_data); break;
        case WIFI_EVENT_AP_STADISCONNECTED: on_disconnected(event_base,  event_data); break;
        case WIFI_EVENT_AP_START: on_start(event_base, event_data); break;
        case WIFI_EVENT_AP_STOP: on_stop(event_base, event_data); break;
        case WIFI_EVENT_AP_PROBEREQRECVED: on_probereqrec(event_base, event_data); break;
        default: break;
        }
        return;
    }
}

