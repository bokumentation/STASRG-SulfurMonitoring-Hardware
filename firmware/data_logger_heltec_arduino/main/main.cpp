// main.cpp

// --- SYSTEM INCLUDE ----
#include <Arduino.h>
#include <Wire.h>
#include <freertos/idf_additions.h>
#include <freertos/projdefs.h>

// --- USER INCLUDE ----
// #include "board_pins.h"           // Board pins definition
// #include "display/ui_ssd1306.cpp" // SSD1306 Implementation
#include "board_pins.h"
#include "esp32-hal.h"
#include "sensor_task.h"

void setup()
{
    // for simple arduino like setup.
}

void loop()
{
    // for simple arduino like loop.
}

#define SENSOR_TASK_PRIORITY 5
#define ANEMOMETER_PIN_CFG   (void *)ANEMOMETER_ADC_PIN

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

// Main function
extern "C" void app_main()
{
    initArduino();
    setup();

    adc_task();

    while (1) {
        loop();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
