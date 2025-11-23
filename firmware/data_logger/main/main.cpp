// main.cpp
#include "esp_err.h"
#include "esp_log.h"
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

// Custom headers
#include "board_pins.h"
#include "tb600b.h"

static const char *TAG_MAIN = "MAIN_APP";
const int SENSOR_READ_INTERVAL_MS = 500; // Read every 5 seconds

void system_check_status(void *pvParameters);
void sensor_reading_task(void *pvParameters);

extern "C" void app_main(void)
{
    // Init H2S Sensor (UART1)
    ESP_LOGI(TAG_MAIN, "Initializing H2S Sensor on UART%d (TX:%d, RX:%d)", SENSOR_H2S_UART_PORT, PIN_H2S_TX,
             PIN_H2S_RX);
    tb600b_init_uart(SENSOR_H2S_UART_PORT, PIN_H2S_TX, PIN_H2S_RX, BAUD_RATE, SENSOR_H2S_TAG);

    // Init SO2 Sensor (UART2)
    ESP_LOGI(TAG_MAIN, "Initializing SO2 Sensor on UART%d (TX:%d, RX:%d)", SENSOR_SO2_UART_PORT, PIN_SO2_TX,
             PIN_SO2_RX);
    tb600b_init_uart(SENSOR_SO2_UART_PORT, PIN_SO2_TX, PIN_SO2_RX, BAUD_RATE, SENSOR_SO2_TAG);

    // Start the periodic sensor reading task
    xTaskCreate(sensor_reading_task, "SensorReadTask", 4096, NULL, 5, NULL);
}

void sensor_reading_task(void *pvParameters)
{
    tb600b_combined_data_t h2s_data{};
    tb600b_combined_data_t so2_data{};
    esp_err_t err;

    while (1) {
        // --- 1. Read H2S Sensor (UART1) ---
        err = tb600b_read_combined_data(SENSOR_H2S_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA),
                                        &h2s_data);

        if (err == ESP_OK) {
            // ESP_LOGI(SENSOR_H2S_TAG, "H2S | Temp: %.2f C, Hum: %.2f %%RH, Gas: %.2f ug/m3", h2s_data.temperature_c,
            //          h2s_data.humidity_perc, h2s_data.gas_ugm3);
        }
        else {
            ESP_LOGE(SENSOR_H2S_TAG, "Read failed with error: %s", esp_err_to_name(err));
        }

        // --- 2. Read SO2 Sensor (UART2) ---
        err = tb600b_read_combined_data(SENSOR_SO2_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA),
                                        &so2_data);

        if (err == ESP_OK) {
            // ESP_LOGI(SENSOR_SO2_TAG, "SO2 | Temp: %.2f C, Hum: %.2f %%RH, Gas: %.2f ug/m3", so2_data.temperature_c,
            //          so2_data.humidity_perc, so2_data.gas_ugm3);
        }
        else {
            ESP_LOGE(SENSOR_SO2_TAG, "Read failed with error: %s", esp_err_to_name(err));
        }

        float h2s_ppm = tb600b_convert_ugm3_to_ppm(h2s_data.gas_ugm3, h2s_data.temperature_c,
                                                   M_W_H2S // Use the defined constant
        );

        float so2_ppm = tb600b_convert_ugm3_to_ppm(so2_data.gas_ugm3, so2_data.temperature_c,
                                                   M_W_SO2 // Use the defined constant
        );

        // Print to Terminal
        printf("H2S Gas: %.2f ug/m3 (%.3f ppm)", h2s_data.gas_ugm3, h2s_ppm);
        printf("\n");
        printf("SO2 Gas: %.2f ug/m3 (%.3f ppm)", so2_data.gas_ugm3, so2_ppm);
        printf("\n");

        // Delay for the next reading cycle
        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}