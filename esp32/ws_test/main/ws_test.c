#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <nvs_flash.h>

#define LED_GPIO 23

#define WS_WIFI_SSID            "multilingual_jellyfish" // "RCMP Horse Portable WiFi 452"
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

void set_led_state(uint8_t state) {
	gpio_set_level(LED_GPIO, state == 0 ? 0 : 1);
}

esp_err_t ws_handler(httpd_req_t *req) {
	httpd_ws_frame_t ws_pkt;
	uint8_t *buf = NULL;
	memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
	ws_pkt.type = HTTPD_WS_TYPE_TEXT;
	esp_err_t res = httpd_ws_recv_frame(req, &ws_pkt, 0);
	if (res != ESP_OK) {
		ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame with %d", res);
		return res;
	}

	ESP_LOGI(TAG, "Frame length is %d", ws_pkt.len);
	if (ws_pkt.len) {
		// Calloc one byte extra for null terminator since we're receiving a string
		buf = calloc(1, ws_pkt.len+1);
		if (buf == NULL) {
			ESP_LOGE(TAG, "Failed to calloc for buf");
			return ESP_ERR_NO_MEM;
		}
		ws_pkt.payload = buf;
		res = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
		if (res != ESP_OK) {
			ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", res);
			free(buf);
			return res;
		}
		ESP_LOGI(TAG, "got packet with msg '%s'", ws_pkt.payload);
	}

	ESP_LOGI(TAG, "packet type: %d", ws_pkt.type);
	if (ws_pkt.type == HTTPD_WS_TYPE_TEXT && ws_pkt.payload != NULL) {
		if (strncmp((char *)ws_pkt.payload, "1\n", strlen("1\n")) == 0) {
			set_led_state(1);
		} else if (strncmp((char *)ws_pkt.payload, "0\n", strlen("0\n")) == 0) {
			set_led_state(0);
		}
	}

	free(buf);
	return ESP_OK;
}

const httpd_uri_t ws = {
	.uri = "/ws",
	.method = HTTP_GET,
	.handler = ws_handler,
	.user_ctx = NULL,
	.is_websocket = true,
};

httpd_handle_t start_webserver(void) {
	httpd_handle_t server = NULL;
	httpd_config_t server_cfg = HTTPD_DEFAULT_CONFIG();

	ESP_LOGI(TAG, "starting server on port %d", server_cfg.server_port);
	if (httpd_start(&server, &server_cfg) == ESP_OK) {
		ESP_LOGI(TAG, "registering uri handler");
		httpd_register_uri_handler(server, &ws);
		return server;
	}

	ESP_LOGE(TAG, "error starting server");
	return NULL;
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

	gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

	static httpd_handle_t server = NULL;
	server = start_webserver();
}
