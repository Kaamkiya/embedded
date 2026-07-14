#include <string.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "main.h"
#include "wifi.h"
#include "ws.h"

#define LED_GPIO 23

void set_led_state(uint8_t state) {
	gpio_set_level(LED_GPIO, state == 0 ? 0 : 1);
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
