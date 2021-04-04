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
#define METRONOME_RECIVER_IP_ADDR             "255.255.255.255"

#define METRONOME_WIFE_AP_SSID                "Metronome AP"
#define METRONOME_WIFE_AP_PASSWD              "Q3ld5xc6!!"
#define METRONOME_WIFI_AP_AUTH                WIFI_AUTH_WPA2_PSK

#define METRONOME_WIFI_STA_SSID               "Test SSID"
#define METRONOME_WIFE_STA_PASSWD             "Test"
#define METRONOME_WIFI_STA_AUTH                WIFI_AUTH_WPA2_PSK

#define METRONOME_MDNS_ENABLE                METRONOME_ENABLE
#define METRONOME_MDNS_NAME                  "Metronome"
#define METRONOME_MDNS_INSTANCE              "Metronome Musik Box"

#define METRONOME_ADF_SAMPLERATE            44100
#define METRONOME_ADF_BITS                  16
#define METRONOME_ADF_CHANNELS              2

#define metro_atoa(x) #x

#endif // CONFIG_H
