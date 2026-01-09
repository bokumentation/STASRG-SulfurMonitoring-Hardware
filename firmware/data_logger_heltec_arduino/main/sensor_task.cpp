// SENSOR_TASK.CPP
/*
 * =====================================
 * === IMPLEMENTASI PEMBACAAN SENSOR ===
 * =====================================
 *
 * Berhasil diuji:
 * TB600B_SO2:  UART    ✅
 * TB600C_H2S:  UART    ✅
 * ANEMOMETER:  ADC     ✅
 * INA219:      I2C     ✅
 *
 * Belum diuji:
 * BME280:      I2C     ❌
 * RTC_DS3231:  I2C     ❌
 * UBLOX NEO:   UART    ❌
 * WIND DIR:    UART    ❌
 */

// --- SYSTEM INCLUDE ---
#include <Arduino.h>
#include <HardwareSerial.h>
#include <cstdio>
#include <esp_log.h>

// --- USER LIBRARIES ---
#include <Adafruit_BME280.h> // BME280 (ATM Pressure)
#include <RTClib.h>          // RTC DS3231 (Realtime Clock)
#include <RadioLib.h>        // SX1262 (L O R A Communication)
#include <SoftwareSerial.h>  // Emulated Software Serial (arduino-esp32 aslinya tidak support SoftwareSerial)
#include <TinyGPSPlus.h>     // GPS uBlox Neo

// --- USER INCLUDE ---
#include "board_pins.h"
#include "sensor/anemometer.h"   // Anemometer
#include "sensor/tb600_sensor.h" // tb600 sensor untuk SO2 dan H2S
#include "shared_data.h"         // Data versi MUTEX supaya tidak terjadi Race Condition

// --- USER DEFINE ---

// Data mutex untuk menghindari race condition dengan task lain
// Mutex digunakan untuk mengamankan akses ke data live_data
// karena data ini diakses oleh beberapa task (sensor_reading, anemometer_task, batteryTask)
SemaphoreHandle_t data_mutex = xSemaphoreCreateMutex();

// Inisialisasi objek untuk menyimpan data dari sensor
sensor_output_t live_data = {.so2_ppm = 0.0,
                             .so2_ugm = 0.0,
                             .h2s_ppm = 0.0,
                             .h2s_ugm = 0.0,
                             .h2s_temp = 0.0,
                             .h2s_hum = 0.0,
                             .wind_speed = 0.0,
                             .bus_voltage_v = 0.0,
                             .current_ma = 0.0};

// --- Task 1: Pembacaan sensor TB6000
#define UART_SENSOR_READ_INTERVAL_MS 2000 // Interval pembacaan sensor dalam milidetik

// Inisialisasi objek untuk menyimpan data dari sensor h2s dan so2
tb600b_combined_data_t h2s_data{}; // Struct h2s
tb600b_combined_data_t so2_data{}; // Struct so2

// Pembacaan sensor TB6000
void sensor_reading(void *pvParameters)
{
    // Inisialisasi UART untuk H2S
    tb600b_init_uart(SENSOR_H2S_UART_PORT, PIN_SENSOR_H2S_TX, PIN_SENSOR_H2S_RX, SENSOR_H2S_TAG);

    // Inisialisasi UART untuk SO2
    tb600b_init_uart(SENSOR_SO2_UART_PORT, PIN_SENSOR_SO2_TX, PIN_SENSOR_SO2_RX, SENSOR_SO2_TAG);

    while (1) {
        // Membaca sensor melalui UART1 dan UART2
        tb600b_read_combined_data(SENSOR_H2S_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA), &h2s_data);
        tb600b_read_combined_data(SENSOR_SO2_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA), &so2_data);

        // Contoh konversi dari ug/m³ ke ppm
        // live_data.h2s_ppm = tb600b_convert_ugm3_to_ppm(h2s_data.gas_ugm3, h2s_data.temperature_c, M_W_H2S); // H2S: 34.08 g/mol
        // live_data.h2s_ugm = h2s_data.gas_ugm3;

        // live_data.so2_ppm = tb600b_convert_ugm3_to_ppm(so2_data.gas_ugm3, so2_data.temperature_c, M_W_SO2); // SO2: 64.06 g/mol
        // live_data.so2_ugm = so2_data.gas_ugm3;

        // UPDATE STRUKTUR DATA SHARED
        if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(100))) {
            live_data.h2s_ugm = h2s_data.gas_ugm3;
            live_data.so2_ugm = so2_data.gas_ugm3;
            live_data.h2s_temp = h2s_data.temperature_c;
            live_data.h2s_hum = h2s_data.humidity_perc;
            xSemaphoreGive(data_mutex);
        }

        // Delay untuk menghindari pembacaan terlalu cepat
        // Interval pembacaan sensor diatur sesuai dengan kebutuhan
        vTaskDelay(pdMS_TO_TICKS(UART_SENSOR_READ_INTERVAL_MS));
    }
}

// --- Task 2: Anemometer Polling (Non-Blocking) ---
#define ANEMOMETER_MEASUREMENT_INTERVAL_SEC 10
#define ANEMOMETER_POLL_MS                  5

anemometer_handle_t g_anemometer_handle = NULL;
anemometer_data_t anem_data{}; // Inisialisasi objek struct untuk menyimpan data nanemometer

// Anemometer Task untuk membaca kecepatan angin
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
            if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(50))) {
                live_data.wind_speed = anem_data.wind_speed_mps;
                xSemaphoreGive(data_mutex);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(ANEMOMETER_POLL_MS));
    }
}

// --- Task 3: Pembaca Tegangan dan Arus Baterai (INA219) ---
#include "Adafruit_INA219.h"
Adafruit_INA219 ina219_device(I2C_HW_ADDR_SENSOR_INA219_2);

// Pembaca Tegangan dan Arus Baterai (INA219)
void batteryTask(void *pvParameters)
{
    // Error handling jika I2C tidak berhasil diinisialisasi
    if (!Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL)) {
        ESP_LOGE(BAT_DEVICE_MONITORING_TAG, "I2C Begin Failed");
        vTaskDelete(NULL);
    }

    // Error handling jika INA219 tidak berhasil diinisialisasi
    if (!ina219_device.begin(&Wire)) {
        ESP_LOGE(BAT_DEVICE_MONITORING_TAG, "Couldn't find INA219 chip");
    }

    ina219_device.setCalibration_32V_2A(); // Kalibrasi INA219 untuk 32V dan 2A

    while (1) {
        // Parameter penting
        float shuntvoltage = ina219_device.getShuntVoltage_mV();
        float busvoltage = ina219_device.getBusVoltage_V();

        // Perhitungan arus dan tegangan
        float current_mA = ina219_device.getCurrent_mA();
        float loadvoltage = busvoltage + (shuntvoltage / 1000);

        // Forward data ke live_data
        if (xSemaphoreTake(data_mutex, pdMS_TO_TICKS(50))) {
            live_data.bus_voltage_v = loadvoltage;
            live_data.current_ma = current_mA;
            xSemaphoreGive(data_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
