#ifndef METRONOME_BASIC_WIFI_H
#define METRONOME_BASIC_WIFI_H

#include <esp_err.h>
#include <esp_event.h>
#include <string>
#include <string.h>
#include <mn_eventgroup.hpp>
#undef ERR_TIMEOUT

#include "if_interface.h"

namespace metronome {
    class basic_wifi : public wifi_interface  {
    public:
        enum class auth_mode : int {
            Open = WIFI_AUTH_OPEN,
            WPE = WIFI_AUTH_WEP,
            WPA_PSK = WIFI_AUTH_WPA_PSK,
            WPA2_PSK = WIFI_AUTH_WPA2_PSK,
            WPA3_PSK = WIFI_AUTH_WPA3_PSK,
            WPA2_WPA3_PSK = WIFI_AUTH_WPA2_WPA3_PSK,
            WPA_WPA2_PSK = WIFI_AUTH_WPA_WPA2_PSK
        };
        using auth_mode_t = basic_wifi::auth_mode;

        basic_wifi(esp_interface_t type );
        virtual ~basic_wifi();

        virtual bool stop(bool wifioff = false) { return true; }
        void         setup(wifi_init_config_t* cfg);
        void         desetup();
    protected:
        virtual void on_wifi_event(esp_event_base_t event_base, int32_t event_id, void* event_data) { return; }
        virtual void on_ip_event(esp_event_base_t event_base, int32_t event_id, void* event_data) { return; }
    private:
        static void _wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
        static void _ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    protected:
        mn::event_group_t   m_eGroup;
    };
}


#endif // METRONOME_BASIC_WIFI_H
