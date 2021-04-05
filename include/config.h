#ifndef CONFIG_H
#define CONFIG_H

#define METRONOME_ENABLE                     1
#define METRONOME_DISABLE                    0

#define METRONOME_WIFI_MODE_AP               1
#define METRONOME_WIFI_MODE_STA              2

#define METRONOME_WIFI_MODE                   METRONOME_WIFI_MODE_AP

#define METRONOME_HMAC_SHA256_KEY "ineredrdöhiedrnehxöu.ogidpbctaudcreixmrb kjqmwxiikdroeuigeoä.gp5f4785f894ya98"
#define METRONOME_HMAC_SHA256_SIZE strlen(METRONOME_HMAC_SHA256_KEY)

#define METRONOME_RECIVER_UDP_PORT            8182
#define METRONOME_RECIVER_IP_ADDR             METRONOME_IPV4_ADDRESS_BROADCAST

#define METRONOME_WIFE_AP_SSID                "Metronome AP"
#define METRONOME_WIFE_AP_PASSWD              "Q3ld5xc6!!"
#define METRONOME_WIFI_AP_AUTH                WIFI_AUTH_WPA2_PSK

#define METRONOME_WIFI_STA_SSID               "Test SSID"
#define METRONOME_WIFE_STA_PASSWD             "Test"
#define METRONOME_WIFI_STA_AUTH                WIFI_AUTH_WPA2_PSK

#define METRONOME_MDNS_ENABLE                METRONOME_ENABLE
#define METRONOME_MDNS_NAME                  "Metronome"
#define METRONOME_MDNS_INSTANCE              "Metronome Musik Box"

#define METRONOME_ADF_SAMPLERATE            48000
#define METRONOME_ADF_BITS                  16
#define METRONOME_ADF_CHANNELS              2

#define METRONOME_SAMPLE_PER_CYCLE 			(METRONOME_ADF_SAMPLERATE/100)
#define METRONOME_ADF_BUFFER_SEND			((METRONOME_ADF_BITS+8) / 16) * METRONOME_SAMPLE_PER_CYCLE * 4

#define METRONOME_RINGBUFFER_SIZE			4096*2


#define metro_atoa(x) #x

#endif // CONFIG_H
