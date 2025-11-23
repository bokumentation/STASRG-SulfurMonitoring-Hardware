// tb600b.cpp

#include "tb600b.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"
#include <board_pins.h>

static esp_err_t tb600b_wait_for_data(uart_port_t uart_num, const char *tag);

/**
 * @brief Initialize the UART Port.
 */
void tb600b_init_uart(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate, const char *tag)
{
    const uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    }; // Missing 'flag' initializer but it not needed.

    ESP_ERROR_CHECK(uart_driver_install(uart_num, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(tag, "Initialize...");
    vTaskDelay(pdMS_TO_TICKS(100));
}

/**
 * @brief Waits until data is available in the UART RX buffer, yielding the task if necessary.
 */
static esp_err_t tb600b_wait_for_data(uart_port_t uart_num, const char *tag)
{
    size_t length = 0;
    const int max_wait_ms = 500;
    const int delay_step_ms = 5;

    for (int elapsed = 0; elapsed < max_wait_ms; elapsed += delay_step_ms) {
        if (uart_get_buffered_data_len(uart_num, &length) != ESP_OK) {
            return ESP_FAIL;
        }

        if (length > 0) {
            return ESP_OK;
        }

        vTaskDelay(pdMS_TO_TICKS(delay_step_ms));
    }
    ESP_LOGW(tag, "No response data detected in buffer after %d ms.", max_wait_ms);
    return ESP_ERR_TIMEOUT;
}

/**
 * @brief Sends command and reads combined data from the TB600 sensor (REPLACING get_combined_data).
 */
esp_err_t tb600b_read_combined_data(uart_port_t uart_num, const uint8_t *command, size_t commandSize,
                                    tb600b_combined_data_t *data_out)
{
    const char *tag = (uart_num == UART_NUM_1) ? SENSOR_H2S_TAG : SENSOR_SO2_TAG;
    const int max_sync_attempts = TB600B_RESPONSE_LENGTH + 2;
    uint8_t responseData[TB600B_RESPONSE_LENGTH];
    int bytesRead = 0;

    if (!data_out) {
        ESP_LOGE(tag, "Output data structure pointer is NULL.");
        return ESP_ERR_INVALID_ARG;
    }

    // 1. Flush and Send Command
    uart_flush(uart_num);
    if (uart_write_bytes(uart_num, command, commandSize) != commandSize) {
        return ESP_FAIL;
    }
    vTaskDelay(pdMS_TO_TICKS(50)); // Sensor internal processing delay

    // 2. Adaptive Wait for First Byte
    if (tb600b_wait_for_data(uart_num, tag) != ESP_OK) {
        return ESP_ERR_TIMEOUT; // Exit immediately on total comms failure
    }

    // 3. Synchronization Loop
    for (int sync_count = 0; sync_count < max_sync_attempts; sync_count++) {
        // Read the full expected frame length with a short timeout
        bytesRead = uart_read_bytes(uart_num, responseData, TB600B_RESPONSE_LENGTH, pdMS_TO_TICKS(100));

        if (bytesRead == TB600B_RESPONSE_LENGTH) {
            if (responseData[0] == TB600B_RESPONSE_HEADER_BYTE1 && responseData[1] == TB600B_RESPONSE_HEADER_BYTE2) {
                // SUCCESS: Valid frame read. Log and break loop for parsing.
                ESP_LOG_BUFFER_HEXDUMP(tag, responseData, bytesRead, ESP_LOG_INFO);
                goto parse_data;
            }
            else {
                // MISALIGNMENT: Attempt to re-sync
                ESP_LOGW(tag, "Malformed header found. Attempting re-sync.");

                // Try to find header mid-frame and push remaining data back
                for (int i = 1; i < bytesRead - 1; i++) {
                    if (responseData[i] == 0xFF && responseData[i + 1] == 0x87) {
                        uart_write_bytes(uart_num, (const char *)&responseData[i], TB600B_RESPONSE_LENGTH - i);
                        vTaskDelay(pdMS_TO_TICKS(10));
                        goto continue_sync_loop; // Skip to next loop iteration
                    }
                }

                // If no header found mid-frame, flush and rely on next read
                uart_flush(uart_num);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        else if (bytesRead > 0 && bytesRead < TB600B_RESPONSE_LENGTH) {
            // Partial read: likely noise. Flush and retry.
            ESP_LOGW(tag, "Partial response (%d bytes). Flushing and retrying.", bytesRead);
            uart_flush(uart_num);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        else {
            // Timeout (bytesRead == 0) after waiting.
            ESP_LOGE(tag, "Timeout during frame read.");
            break; // Exit loop, fail the function
        }

    continue_sync_loop:; // Label for continue/retry logic
    }

    // --- 4. FAILURE EXIT ---
    ESP_LOGE(tag, "Failed to synchronize frame header after %d attempts.", max_sync_attempts);
    data_out->temperature_c = data_out->humidity_perc = data_out->gas_ugm3 = 0.0f;
    return ESP_ERR_TIMEOUT;

// --- 5. SUCCESS PARSING ---
parse_data:
    // Parse gas concentration (ug/m³) from bytes 2 and 3
    uint16_t rawGasUg = (uint16_t)((responseData[2] << 8) | responseData[3]);
    data_out->gas_ugm3 = (float)rawGasUg;

    // Parse Temperature from bytes 8 and 9
    int16_t rawTemperature = (int16_t)((responseData[8] << 8) | responseData[9]);
    data_out->temperature_c = (float)rawTemperature / 100.0f;

    // Parse Humidity from bytes 10 and 11
    uint16_t rawHumidity = (uint16_t)((responseData[10] << 8) | responseData[11]);
    data_out->humidity_perc = (float)rawHumidity / 100.0f;

    vTaskDelay(pdMS_TO_TICKS(100));
    return ESP_OK;
}

/**
 * @brief Converts gas concentration from ug/m³ to ppm, corrected for ambient temperature.
 *
 * Formula derived from Ideal Gas Law (PV = nRT) at ambient pressure (assumed 1 atm):
 * ppm = (ug/m³) * [ (22.414 * (T_C + 273.15)) / (273.15 * M) ] * (1 / 1000)
 * Where 22.414 L/mol is the molar volume at 0 C, 1 atm.
 */
float tb600b_convert_ugm3_to_ppm(float ugm3_concentration, float temperature_c, float molecular_weight)
{
    // Constant: Molar Volume (Vm) at Standard Temperature (0°C / 273.15 K) and Pressure (1 atm)
    const float Vm_STP = 22.414f;
    // Constant: Standard Temperature in Kelvin
    const float T_STP = 273.15f;

    // Convert temperature from Celsius to Kelvin
    float temp_k = temperature_c + T_STP;

    if (molecular_weight <= 0.0f) {
        ESP_LOGE(TB600_TAG_UART_SENSOR, "Molecular weight must be positive for ppm conversion.");
        return 0.0f;
    }

    // Calculate the temperature-corrected molar volume (Vm_T)
    // Vm_T = Vm_STP * (T_K / T_STP)
    float Vm_T = Vm_STP * (temp_k / T_STP);

    // Calculate the conversion factor: (Vm_T / M) / 1000
    // The final division by 1000 converts ug to mg (or ug/m³ to mg/m³) to match the standard ppm relationship.
    float conversion_factor = Vm_T / (molecular_weight * 1000.0f);

    return ugm3_concentration * conversion_factor;
}