#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define ENABLE_PIN 18
#define DIR_A_PIN  19
#define DIR_B_PIN  5

void app_main(void)
{
	gpio_reset_pin(ENABLE_PIN);
	gpio_reset_pin(DIR_A_PIN);
	gpio_reset_pin(DIR_B_PIN);
	gpio_set_direction(ENABLE_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(DIR_A_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(DIR_B_PIN, GPIO_MODE_OUTPUT);

	gpio_set_level(ENABLE_PIN, 1);
	while (1) {
		gpio_set_level(DIR_A_PIN, 1);
		gpio_set_level(DIR_B_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(3000));

		gpio_set_level(ENABLE_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(297));
		gpio_set_level(ENABLE_PIN, 1);

		gpio_set_level(DIR_A_PIN, 0);
		gpio_set_level(DIR_B_PIN, 1);
		vTaskDelay(pdMS_TO_TICKS(3000));

		gpio_set_level(ENABLE_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(297));
		gpio_set_level(ENABLE_PIN, 1);
	}
}
