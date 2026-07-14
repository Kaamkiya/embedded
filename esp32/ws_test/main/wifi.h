#include <esp_event.h>
#include <esp_log.h>
#include <esp_wifi.h>

#ifndef PROJ_WIFI_H
#define PROJ_WIFI_H

#define WS_WIFI_SSID            "multilingual_jellyfish" // "RCMP Horse Portable WiFi 452"
#define WS_WIFI_PASSWD          "oh-idk-ig-this-works"
#define WS_WIFI_CHANNEL         1
#define WS_WIFI_MAX_CONNECTIONS 4

void wifi_event_handler(void *arg, esp_event_base_t evt, int32_t evt_type, void *evt_data);
void wifi_init_softap(void);

#endif
