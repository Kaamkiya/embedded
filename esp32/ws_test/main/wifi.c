#include "main.h"
#include "wifi.h"

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
