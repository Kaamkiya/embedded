#include <stdio.h>
#include <string.h>

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_err.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_CLOCK_SPEED (400*1000)
#define SSD1306_SDA_GPIO 21
#define SSD1306_SCL_GPIO 22

static uint8_t framebuffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = {0};

void app_main(void)
{
	/* Create the I2C bus */
	i2c_master_bus_handle_t i2c_bus = NULL;
	i2c_master_bus_config_t bus_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.i2c_port = 0,
		.sda_io_num = SSD1306_SDA_GPIO,
		.scl_io_num = SSD1306_SCL_GPIO,
		.flags.enable_internal_pullup = true,
	};
	ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

	/* Allocate an LCD IO device handle from the I2C bus */
	esp_lcd_panel_io_handle_t io_handle = NULL;
	esp_lcd_panel_io_i2c_config_t io_config = {
		.dev_addr = 0x3C,
		.scl_speed_hz = SSD1306_CLOCK_SPEED,
		.control_phase_bytes = 1, // According to SSD1306 datasheet
		.lcd_cmd_bits = 8, // According to SSD1306 datasheet
		.lcd_param_bits = 8, // According to SSD1306 datasheet
		.dc_bit_offset = 6, // According to SSD1306 datasheet
	};
	ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

	/* Install the LCD controller driver */
	esp_lcd_panel_handle_t panel_handle = NULL;
	esp_lcd_panel_dev_config_t panel_config = {
		.bits_per_pixel = 1, // 128x64, 8 "pages", 8 bits per page
		.reset_gpio_num = -1,
	};
	esp_lcd_panel_ssd1306_config_t ssd1306_config = {
		.height = 64,
	};
	panel_config.vendor_config = &ssd1306_config;
	ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

	/* Init */
	ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
	ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
	ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
	ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0, 0));

	/* Drawing */
	while (1) {
		memset(framebuffer, 0b11111111, sizeof(framebuffer));
		esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, framebuffer);
		vTaskDelay(pdMS_TO_TICKS(500));

		memset(framebuffer, 0b00000000, sizeof(framebuffer));
		esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, framebuffer);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
