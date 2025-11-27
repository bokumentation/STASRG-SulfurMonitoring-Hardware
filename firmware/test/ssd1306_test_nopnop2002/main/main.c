#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board_pins.h"
#include "font8x8_basic.h"
#include "ssd1306.h"

void app_main(void)
{
    SSD1306_t dev;
    int top = 2;
    int center = 3;
    int bottom = 8;
    char lineChar[20];
    char textLen = 16;

    ESP_LOGI(DISPLAY_TAG, "INTERFACE is i2c");
    ESP_LOGI(DISPLAY_TAG, "PIN_I2C_SDA=%d", PIN_I2C_SDA);
    ESP_LOGI(DISPLAY_TAG, "PIN_I2C_SCL=%d", PIN_I2C_SCL);
    ESP_LOGI(DISPLAY_TAG, "PIN_RST_GPIO=%d", PIN_RST_GPIO);

    i2c_master_init(&dev, PIN_I2C_SDA, PIN_I2C_SCL, PIN_RST_GPIO);

    ESP_LOGI(DISPLAY_TAG, "Panel is 128x64");
	
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);

    ssd1306_display_text(&dev, 0, "PAGE 1", textLen, false);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
	
    while (1) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
