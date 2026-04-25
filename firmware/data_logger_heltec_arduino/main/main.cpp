// main.cpp
// --- SYSTEM INCLUDE ----
#include <Arduino.h>
#include <Wire.h>
#include <esp32-hal.h>
#include <freertos/idf_additions.h>
#include <freertos/projdefs.h>

// --- USER INCLUDE ----
#include "board_pins.h"  // Definisi Pin yang dipakai
#include "sensor_task.h" // Header kode sensor
#include "shared_data.h" // Data sensor bersama

// --- FUNGSI UTAMA ESP-IDF ---
extern "C" void app_main()
{
    initArduino();
    Serial.begin(115200);                 // Memastikan UART USB Serial Monitor terinisialisasi
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL); // Inisialisasi I2C

    // 1. Membuat Task fungsi membaca sensor
    xTaskCreate(sensor_reading, "Gas_Sensor_Task", 4096, NULL, SENSOR_TASK_PRIORITY, NULL);

    // 2. Membuat Task fungsi membaca anemometer
    xTaskCreate(anemometer_task, "Anemometer_Task", 2048, ANEMOMETER_PIN_CFG, SENSOR_TASK_PRIORITY + 1, NULL);

    // 3. Membuat Task fungsi membaca battery
    xTaskCreate(batteryTask, "Battery_Task", 4096, NULL, 2, NULL);

    // 4. Kodingan LoRA di mulai dari sini

    while (1) {
        // Kodingan LoRa untuk mengirim data atau menerima

        // Print ke serial monitor dengan aman
        if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(10))) {
            Serial.printf("%.3f,%.3f,%.2f,%.2f,%.2f,%.2f,%.1f\n", live_data.so2_ugm, live_data.h2s_ugm, live_data.h2s_temp, live_data.h2s_hum,
                          live_data.wind_speed, live_data.bus_voltage_v, live_data.current_ma);

            xSemaphoreGive(data_mutex);
        }

        delay(1000); // Delay 1 detik untuk menghindari flooding serial monitor
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}