// main.cpp
#include "HardwareSerial.h"
#include <Arduino.h>
#include <Wire.h>

// User includes
#include "board_pins.h" // Board pins definition
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "tb600b.h" // Tb600 library
#include "ui_ssd1306.cpp"

#define SENSOR_READ_INTERVAL_MS 2000

tb600b_combined_data_t h2s_data{};
tb600b_combined_data_t so2_data{};

void sensor_reading(void *pvParameters);

void setup()
{
    Serial.begin(115200);

    // Init I2C
    Wire.begin(21, 20);
    
    xTaskCreate(ssd1306_tasks, "SSD1306", 4096, NULL, 5, NULL);
    xTaskCreate(sensor_reading, "sensor_reading", 4096, NULL, 5, NULL);
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1));
}

void sensor_reading(void *pvParameters)
{
    tb600b_init_uart(SENSOR_H2S_UART_PORT, PIN_SENSOR_H2S_TX, PIN_SENSOR_H2S_RX, SENSOR_H2S_TAG);
    tb600b_init_uart(SENSOR_SO2_UART_PORT, PIN_SENSOR_SO2_TX, PIN_SENSOR_SO2_RX, SENSOR_SO2_TAG);

    while (1) {
        tb600b_read_combined_data(SENSOR_H2S_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA),
                                  &h2s_data);
        tb600b_read_combined_data(SENSOR_SO2_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA),
                                  &so2_data);

        float h2s_ppm = tb600b_convert_ugm3_to_ppm(h2s_data.gas_ugm3, h2s_data.temperature_c, M_W_H2S);

        float so2_ppm = tb600b_convert_ugm3_to_ppm(so2_data.gas_ugm3, so2_data.temperature_c, M_W_SO2);

        Serial.printf("H2S Gas: %.2f ug/m3 (%.3f ppm) | temp: %.2f | hum:%.2f \n", h2s_data.gas_ugm3, h2s_ppm, h2s_data.temperature_c, h2s_data.humidity_perc);
        Serial.printf("SO2 Gas: %.2f ug/m3 (%.3f ppm) | temp: %.2f | hum:%.2f \n", so2_data.gas_ugm3, so2_ppm, so2_data.temperature_c, so2_data.humidity_perc);

        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}