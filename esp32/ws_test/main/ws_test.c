#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define WS_WIFI_SSID            "multilingual_jellyfish"
#define WS_WIFI_PASSWD          "oh-idk-ig-this-works"
#define WS_WIFI_CHANNEL         1
#define WS_WIFI_MAX_CONNECTIONS 4

static const char *TAG = "WS_LED_TEST";

void wifi_event_handler(void *arg, esp_event_base_t evt, int32_t evt_type, void *evt_data) {
	if (evt_type == WIFI_EVENT_AP_STACONNECTED) { // if something connected to the ap
		ESP_LOGI(TAG, "received new client connection");
	} else if (evt_type == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(TAG, "client disconnection");
	}
}


void wifi_init_softap(void) {
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
	                                                    ESP_EVENT_ANY_ID,
	                                                    &wifi_event_handler,
	                                                    NULL,
	                                                    NULL));

	wifi_config_t wifi_cfg = {
		.ap = {
			.ssid = WS_WIFI_SSID,
			.ssid_len = strlen(WS_WIFI_SSID),
			.channel = WS_WIFI_CHANNEL,
			.password = WS_WIFI_PASSWD,
			.max_connection = WS_WIFI_MAX_CONNECTIONS,
			.authmode = WIFI_AUTH_WPA2_PSK,
			.pmf_cfg = {
				.required = true,
			},
			.gtk_rekey_interval = 0,
		},
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_cfg));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI(TAG, "wifi_init_softap finished ssid=%s pw=%s channel=%d",
	         WS_WIFI_SSID,
	 	 WS_WIFI_PASSWD,
	 	 WS_WIFI_CHANNEL);
}

void app_main(void) {
	esp_err_t res = nvs_flash_init();
	if (res == ESP_ERR_NVS_NO_FREE_PAGES || res == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		res = nvs_flash_init();
	}
	ESP_ERROR_CHECK(res);

	ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
	wifi_init_softap();
}
