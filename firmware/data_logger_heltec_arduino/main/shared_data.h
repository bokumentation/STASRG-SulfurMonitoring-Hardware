#pragma once

#include <Arduino.h>
#include <freertos/semphr.h>

typedef struct {
    float so2_ppm;
    float so2_ugm;
    float h2s_ppm;
    float h2s_ugm;
    float h2s_temp;
    float h2s_hum;
    float wind_speed;
    float bus_voltage_v;
    float current_ma;
} sensor_output_t;

// Global instance and mutex for thread safety
extern sensor_output_t live_data;
extern SemaphoreHandle_t data_mutex;

