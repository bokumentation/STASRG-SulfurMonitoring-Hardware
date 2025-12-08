# Belerang by STAS-RG

Supported Target: ESP32 | ESP32C3 | ESP32S3

## UPDATE NOTE:
In components folder, i added arduino-esp32 as components. Not inside the `main` folder. This is because i wanna fast build times when using `idf.py reconfigure`.


main.cpp

```cpp
// main.cpp
#include "esp_log.h"
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

// Custom headers
#include "board_pins.h"
#include "tb600b.h"

static const char *TAG_MAIN = "MAIN_APP";
const int SENSOR_READ_INTERVAL_MS = 2000; // Read every 5 seconds

void sensor_reading_task(void *pvParameters);

extern "C" void app_main(void)
{
    // Init H2S Sensor (UART1)
    ESP_LOGI(TAG_MAIN, "Initializing H2S Sensor on UART%d (TX:%d, RX:%d)", SENSOR_H2S_UART_PORT, PIN_SENSOR_H2S_TX,
             PIN_SENSOR_H2S_RX);
    tb600b_init_uart(SENSOR_H2S_UART_PORT, PIN_SENSOR_H2S_TX, PIN_SENSOR_H2S_RX, BAUD_RATE, SENSOR_H2S_TAG);

    // Init SO2 Sensor (UART2)
    ESP_LOGI(TAG_MAIN, "Initializing SO2 Sensor on UART%d (TX:%d, RX:%d)", SENSOR_SO2_UART_PORT, PIN_SENSOR_SO2_TX,
             PIN_SENSOR_SO2_RX);
    tb600b_init_uart(SENSOR_SO2_UART_PORT, PIN_SENSOR_SO2_TX, PIN_SENSOR_SO2_RX, BAUD_RATE, SENSOR_SO2_TAG);

    xTaskCreate(sensor_reading_task, "SensorReadTask", 6144, NULL, 5, NULL);
}

void sensor_reading_task(void *pvParameters)
{
    const size_t CMD_SIZE = sizeof(CMD_GET_COMBINED_DATA);
    while (1) {
        // --- 1. Read H2S Sensor (UART1) using the safe wrapper ---
        tb600b_combined_data_t h2s_data = tb600b_get_data_safe(SENSOR_H2S_UART_PORT, CMD_GET_COMBINED_DATA, CMD_SIZE);

        // --- 2. Read SO2 Sensor (UART2) using the safe wrapper ---
        tb600b_combined_data_t so2_data = tb600b_get_data_safe(SENSOR_SO2_UART_PORT, CMD_GET_COMBINED_DATA, CMD_SIZE);

        printf("H2S (T:%.1fC, H:%.1f%%) Gas: %.2f ug/m3\n", h2s_data.temperature_c, h2s_data.humidity_perc,
               h2s_data.gas_ugm3);
        printf("SO2 (T:%.1fC, H:%.1f%%) Gas: %.2f ug/m3\n", so2_data.temperature_c, so2_data.humidity_perc,
               so2_data.gas_ugm3);

        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
}

```