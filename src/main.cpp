#include <sdkconfig.h>
#include "config.h"

#include <iostream>
#include <string>
#include <nvs_flash.h>
#include <mqtt_client.h>
#include <mdns.h>

#include "mn_version.hpp"
#include "basic_wifi_ap.h"

#include "dram_task.h"

#if METRONOME_WIFI_MODE == METRONOME_WIFI_MODE_AP
metronome::basic_wifi_ap _ap_wifi;
#elif METRONOME_WIFI_MODE == METRONOME_WIFI_MODE_STA
metronome::basic_wifi_sta _sta_wifi;
#endif // METRONOME_WIFI_MODE

#if METRONOME_MDNS_ENABLE == METRONOME_ENABLE


void start_mdns_service() {
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return;
    }
    mdns_hostname_set(METRONOME_MDNS_NAME);
    mdns_instance_name_set(METRONOME_MDNS_INSTANCE);

    //add our services
    mdns_service_add(NULL, "_music", "_udp", METRONOME_RECIVER_UDP_PORT, NULL, 0);
    mdns_service_instance_name_set("_music", "_udp", "music input");

     mdns_txt_item_t serviceTxtData[3] = {
        {"samlerate",metro_atoa(METRONOME_ADF_SAMPLERATE)},
        {"bits",metro_atoa(METRONOME_ADF_BITS)},
        {"channels",metro_atoa(METRONOME_ADF_CHANNELS)}
    };
    //set txt data for service (will free and replace current data)
    mdns_service_txt_set("_music", "_udp", serviceTxtData, 3);
}
#endif // METRONOME_MDNS_ENABLE

#include "audio_out_interface.h"

metronome::audio_network_buffer __gl_buffer;
metronome::dram_task* __gl_main_task;
metronome::ip4_endpoint __gl_main_enpoint(METRONOME_IPV4_ADDRESS_ANY, METRONOME_RECIVER_UDP_PORT);
metronome::audio_out_interface __gl_interface;
extern "C" void app_main() {
    ESP_LOGI("main", "[APP] Metronome 0.4 Startup..");
    ESP_LOGI("main", "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI("main", "[APP] IDF version: %s", esp_get_idf_version());
    ESP_LOGI("main", "[APP] MThread version %d.%d", mn::version::instance().get_major_version(), mn::version::instance().get_minor_version());

    ESP_ERROR_CHECK(nvs_flash_init());

    #if METRONOME_WIFI_MODE == METRONOME_WIFI_MODE_AP
        _ap_wifi.start(METRONOME_WIFE_AP_SSID, METRONOME_WIFI_AP_AUTH, METRONOME_WIFE_AP_PASSWD, false, 13, false, 5);
        _ap_wifi.set_ipinfo(metronome::ip4_address("192.168.100.1"),
                            metronome::ip4_address("192.168.100.1"),
                            metronome::ip4_address("255.255.255.0"));
    #elif METRONOME_WIFI_MODE == METRONOME_WIFI_MODE_STA
        _sta_wifi.start(METRONOME_WIFE_STA_SSID, METRONOME_WIFI_STA_AUTH, METRONOME_WIFE_STA_PASSWD);
    #endif // METRONOME_WIFI_MODE

    #if METRONOME_MDNS_ENABLE == METRONOME_ENABLE
        start_mdns_service();
    #endif // METRONOME_MDNS_ENABLE


    __gl_main_task = new metronome::dram_task(__gl_buffer, __gl_main_enpoint, false, true);
    __gl_main_task->start(PRO_CPU_NUM);

    if(!__gl_interface.create(metronome::audio_out_interface::device::sgtl5000, false)) {
		ESP_LOGE("main", "can create the output devive - halt");
		while(true) ;
    }

    mn::task_t::sleep(3);

    char buffer[METRONOME_ADF_BUFFER_SEND] = {0};
	size_t read = 0;

    while(true) {
		read = __gl_buffer.read_audio(buffer, METRONOME_ADF_BUFFER_SEND);
		__gl_interface.write(buffer, 0, read);

		mn::task_t::usleep(1);
    }

}
