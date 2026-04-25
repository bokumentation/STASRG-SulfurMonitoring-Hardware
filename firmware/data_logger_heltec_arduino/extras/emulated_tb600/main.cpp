#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "tb600_sensor_cmd.h" // Your header file

#define SENSOR_UART_NUM      UART_NUM_1
#define SENSOR_TX_PIN        0  // Connect to Main ESP32 RX
#define SENSOR_RX_PIN        1  // Connect to Main ESP32 TX
#define BUF_SIZE             1024

static const char *TAG = "SENSOR_EMU";

// Helper to calculate TB600B Checksum
uint8_t calculate_checksum(uint8_t *packet, size_t len) {
    uint8_t sum = 0;
    for (int i = 1; i < len - 1; i++) {
        sum += packet[i];
    }
    return (uint8_t)((~sum) + 1); 
}

void sensor_emulator_task(void *pvParameters) {
    uint8_t data[BUF_SIZE];
    
    while (1) {
        // Read incoming command from Main Controller
        int len = uart_read_bytes(SENSOR_UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        
        if (len > 0) {
            // Check if it matches CMD_GET_COMBINED_DATA (0x87)
            if (data[2] == 0x87) {
                ESP_LOGI(TAG, "Received Request for Combined Data");

                // Prepare Fake 13-byte Response
                uint8_t response[13] = {0};
                response[0] = 0xFF; // Start Bit
                response[1] = 0x87; // Command Echo
                
                // Fake Gas: 500 ug/m3 -> 0x01F4
                response[2] = 0x01; 
                response[3] = 0xF4;

                // Fake Temp: 25.50 C -> 2550 -> 0x09F6
                response[8] = 0x09;
                response[9] = 0xF6;

                // Fake Humidity: 45.00% -> 4500 -> 0x1194
                response[10] = 0x11;
                response[11] = 0x94;

                // Calculate Checksum for bytes 1-11
                response[12] = calculate_checksum(response, 13);

                uart_write_bytes(SENSOR_UART_NUM, (const char *)response, 13);
            }
        }
    }
}

void app_main(void) {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(SENSOR_UART_NUM, &uart_config);
    uart_set_pin(SENSOR_UART_NUM, SENSOR_TX_PIN, SENSOR_RX_PIN, -1, -1);
    uart_driver_install(SENSOR_UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    xTaskCreate(sensor_emulator_task, "sensor_emu", 4096, NULL, 10, NULL);
}