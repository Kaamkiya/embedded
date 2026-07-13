#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

// GPIO pins. See L293D datasheet for wiring
#define ENABLE_PIN 18
#define DIR_A_PIN  19
#define DIR_B_PIN  5

void app_main(void)
{
	// Configure all pins as output pins
	gpio_set_direction(ENABLE_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(DIR_A_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(DIR_B_PIN, GPIO_MODE_OUTPUT);

	// Enable the motor controller
	gpio_set_level(ENABLE_PIN, 1);

	while (1) {
		// Turn in direction A for 3 seconds
		gpio_set_level(DIR_A_PIN, 1);
		gpio_set_level(DIR_B_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(3000));

		// Pause for 200ms to prevent motor stress
		gpio_set_level(ENABLE_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(200));
		gpio_set_level(ENABLE_PIN, 1);

		// Turn in direction B for 3 seconds
		gpio_set_level(DIR_A_PIN, 0);
		gpio_set_level(DIR_B_PIN, 1);
		vTaskDelay(pdMS_TO_TICKS(3000));

		// Pause for 200ms to prevent motor stress
		gpio_set_level(ENABLE_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(200));
		gpio_set_level(ENABLE_PIN, 1);
	}
}
