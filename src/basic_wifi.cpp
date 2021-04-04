#include "basic_wifi.h"
#include <mn_eventgroup.hpp>

#undef ERR_TIMEOUT

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
#include "lwip/sys.h"

/*
    Betreuuerin Bielefeld: 015156942505 email: a.klarmann@freenet.de
*/
namespace metronome {

    basic_wifi::basic_wifi(esp_interface_t type)
        : wifi_interface(type) { }

    basic_wifi::~basic_wifi()
        { stop(); }

    void basic_wifi::setup(wifi_init_config_t* cfg) {
        auto _tMode = get_mode();

        esp_netif_init();
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        /*if(_tMode  == WIFI_MODE_AP) {
            m_pNetIf = esp_netif_create_default_wifi_ap();
        } else if(_tMode  == WIFI_MODE_STA) {
            m_pNetIf = esp_netif_create_default_wifi_sta();
        }*/
        m_pNetIf = create_netif();

        ESP_ERROR_CHECK(esp_wifi_init(cfg));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &_wifi_event_handler, this));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &_ip_event_handler, this));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
        ESP_ERROR_CHECK(esp_wifi_set_mode( _tMode ));


        // WIFI-Config
        // Start

    }
    void basic_wifi::desetup() {
        ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &_wifi_event_handler));
        ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &_ip_event_handler));

    }

    void basic_wifi::_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
        if(arg != NULL) {
            basic_wifi* _ctx = static_cast<basic_wifi*>(arg);
            _ctx->on_wifi_event(event_base, event_id, event_data);
        }
    }

    void basic_wifi::_ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
        if(arg != NULL) {
            basic_wifi* _ctx = static_cast<basic_wifi*>(arg);
            _ctx->on_ip_event(event_base, event_id, event_data);
        }
    }

    //-----------------------------------------------------------------------------------------------------
}
