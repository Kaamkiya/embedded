#include "main.h"
#include "ws.h"

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
