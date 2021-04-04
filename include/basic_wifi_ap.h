#ifndef _METRONOME_BASIC_WIFI_AP_H
#define _METRONOME_BASIC_WIFI_AP_H

#include "basic_wifi.h"
#undef ERR_TIMEOUT
namespace metronome {

    class basic_wifi_ap : public basic_wifi {
    public:
        using ip4_adress_t = ip4_address;

        basic_wifi_ap() : basic_wifi(ESP_IF_WIFI_AP) { }
        basic_wifi_ap(const basic_wifi_ap& other) = delete;
        ~basic_wifi_ap() { stop(true); }

        bool start(std::string ssid, wifi_auth_mode_t mode = WIFI_AUTH_OPEN, std::string passphrase = "",
                   bool ipv6 = false, int channel = 13, bool ssid_hidden = false,
                   int max_connection = 2);


        bool set_ipinfo(const ip4_adress_t& local_ip, const ip4_adress_t& gateway, const ip4_adress_t& subnet);
        bool stop(bool wifioff = false);

        virtual ip4_adress_t      get_ip();
        virtual ip4_adress_t      get_broadcast();
        virtual ip4_adress_t      get_gateway();
        virtual ip4_adress_t      get_netmask();

        virtual std::string       get_hostname();
        virtual bool              set_hostname(std::string hostname);
        virtual uint8_t*          get_mac(uint8_t* mac);
        virtual std::string       get_mac();
        virtual uint32_t          get_connected();

        operator uint32_t()     { return get_connected(); }
        operator ip4_adress_t() { return get_ip(); }
        operator std::string()  { return get_hostname(); }

        basic_wifi_ap& operator = (const basic_wifi_ap& other) = delete;
    protected:
        tcpip_adapter_ip_info_t get_ip_infos();
    protected:
        //virtual void on_event(system_event_t *event) override;

        virtual void on_wifi_event(esp_event_base_t event_base, int32_t event_id, void* event_data);

        virtual void on_start(esp_event_base_t event_base, void* event_data);
        virtual void on_stop(esp_event_base_t event_base, void* event_data);
        virtual void on_connected(esp_event_base_t event_base,void* event_data);
        virtual void on_disconnected(esp_event_base_t event_base, void* event_data);
        virtual void on_probereqrec(esp_event_base_t event_base, void* event_data);
    };
}

#endif // _METRONOME_BASIC_WIFI_AP_H
