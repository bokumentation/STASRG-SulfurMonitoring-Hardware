// main.cpp

// --- SYSTEM INCLUDE ----
#include <Arduino.h>
#include <Wire.h>
#include <freertos/idf_additions.h>
#include <freertos/projdefs.h>

// --- LIBRARIES
#include <Adafruit_BME280.h> // BME280 (ATM Pressure)
#include <RTClib.h>          // RTC DS3231 (Realtime Clock)
#include <RadioLib.h>        // SX1262 (L O R A Communication)
#include <SoftwareSerial.h>  // Emulated Software Serial (arduino-esp32 aslinya tidak support SoftwareSerial)
#include <TinyGPSPlus.h>     // GPS uBlox Neo

// --- USER INCLUDE ----
// #include "board_pins.h"           // Board pins definition
// #include "display/ui_ssd1306.cpp" // SSD1306 Implementation
#include "board_pins.h" // Definisi Pin yang dipakai
#include "esp32-hal.h"
#include "sensor_task.h"
#include "shared_data.h"

void setup()
{
    // for simple arduino like setup.
}

void loop()
{
    // for simple arduino like loop.

    if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(10))) {
        Serial.printf("%.3f,%.3f,%.2f,%.2f,%.2f,%.2f,%.1f\n", 
                      live_data.so2_ugm, live_data.h2s_ugm, 
                      live_data.h2s_temp, live_data.h2s_hum, 
                      live_data.wind_speed, live_data.bus_voltage_v, 
                      live_data.current_ma);
        
                      xSemaphoreGive(data_mutex);
    }
    delay(1000);
}

#define SENSOR_TASK_PRIORITY 5
#define ANEMOMETER_PIN_CFG   (void *)PIN_SENSOR_ANEMOMETER_ADC

// Task untuk sensor UART (TB600, GPS, dan Wind Dir)
void uart_task()
{
    // 1. Create the UART Gas Sensor Task (High stack size for potential UART buffers/complex logic)
    xTaskCreate(sensor_reading,       // Function to implement the task
                "Gas_Sensor_Task",    // Name of the task
                4096,                 // Stack size (4KB)
                NULL,                 // Task parameter (not used here)
                SENSOR_TASK_PRIORITY, // Task priority (e.g., 5)
                NULL                  // Task handle (not used here)
    );
}

// Task untuk sensor ADC (Anemometer)
void adc_task()
{
    // 2. Create the Anemometer Task (Needs to run frequently, so assign high priority)
    xTaskCreate(anemometer_task,          // Function to implement the task
                "Anemometer_Task",        // Name of the task
                2048,                     // Stack size (2KB is usually enough)
                ANEMOMETER_PIN_CFG,       // Task parameter (passing the PIN)
                SENSOR_TASK_PRIORITY + 1, // Slightly higher priority for critical pulse detection
                NULL);
}

void batteryTaskCreate()
{
    xTaskCreate(batteryTask, "Battery_Task", 4096, NULL, 2, NULL);
};

// Panggil fungsi ke Fungsi Main (Utama)
extern "C" void app_main()
{
    initArduino();
    // setup();
    Serial.begin(115200);

    uart_task();
    adc_task();
    batteryTaskCreate();

    while (1) {
        loop();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
