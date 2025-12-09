// sensor_reading.cpp
#include <Arduino.h>
#include <HardwareSerial.h>
#include <cstdio>

// User Include
#include "board_pins.h"
#include "esp_log.h"
#include "sensor/anemometer.h"
#include "sensor/tb600_sensor.h"

// --- SENSOR: SO2 and H2S ---
#define UART_SENSOR_READ_INTERVAL_MS 2000

tb600b_combined_data_t h2s_data{};
tb600b_combined_data_t so2_data{};

// Task 1: UART reading SO2 and H2S
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

        printf("H2S Gas: %.2f ug/m3 (%.3f ppm) | temp: %.2f | hum:%.2f \n", h2s_data.gas_ugm3, h2s_ppm,
               h2s_data.temperature_c, h2s_data.humidity_perc);
        printf("SO2 Gas: %.2f ug/m3 (%.3f ppm) | temp: %.2f | hum:%.2f \n", so2_data.gas_ugm3, so2_ppm,
               so2_data.temperature_c, so2_data.humidity_perc);

        vTaskDelay(pdMS_TO_TICKS(UART_SENSOR_READ_INTERVAL_MS));
    }
}

// --- Task 2: Anemometer Polling (Non-Blocking) ---
#define ANEMOMETER_MEASUREMENT_INTERVAL_SEC 10
#define ANEMOMETER_POLL_MS                  5

anemometer_handle_t g_anemometer_handle = NULL;
anemometer_data_t anem_data{};

void anemometer_task(void *pvParameters)
{
    int pin = (int)pvParameters;
    g_anemometer_handle = anemometer_create(pin, ANEMOMETER_MEASUREMENT_INTERVAL_SEC);

    if (g_anemometer_handle == NULL) {
        ESP_LOGE(ANEMOMETER_TAG, "FATAL: Failed to allocate context!");
        vTaskDelete(NULL);
        return;
    }

    anemometer_begin(g_anemometer_handle);
    ESP_LOGI(ANEMOMETER_TAG, "Init OK. Measuring every %u seconds.", ANEMOMETER_MEASUREMENT_INTERVAL_SEC);

    while (1) {
    
        if (anemometer_read_speed(g_anemometer_handle, &anem_data)) {
            printf("Speed: %.2f m/s (%.2f km/h) | RPS: %.2f\n", anem_data.wind_speed_mps, anem_data.wind_speed_kph, anem_data.rot_per_sec);
        }

        vTaskDelay(pdMS_TO_TICKS(ANEMOMETER_POLL_MS));
    }
}