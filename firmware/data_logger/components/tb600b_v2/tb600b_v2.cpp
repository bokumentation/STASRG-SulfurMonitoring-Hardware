#include "tb600b_v2.h"
#include "esp_log.h"
#include "new"
#include <sys/_intsup.h>

static const int RX_BUF_SIZE = 128;

// --- Initialization ---
tb600b_handle_t *tb600b_init(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate, const char *tag,
                             tb600b_sensor_type_t type)
{
    // 1. Allocate and Check Handle
    tb600b_handle_t *handle = new (std::nothrow) tb600b_handle_t;
    if (handle == nullptr) {
        ESP_LOGE(tag, "Failed to allocate memory for handle.");
        return nullptr;
    }

    // 2. Configure Handle
    handle->uart_num = uart_num;
    handle->tag = tag;
    handle->sensor_type = type; // Store the specific type
    handle->temperature = 0.0f; // Zero-initialize data
    handle->humidity = 0.0f;
    handle->gas_ug = 0.0f;
    handle->is_initialized = false;

    // 3. Configure and Install UART
    const uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    }; // Missing initializer but we need to ignore it.

    ESP_ERROR_CHECK(uart_driver_install(uart_num, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // 4. Set mode
    tb600b_set_passive_mode(handle);

    handle->is_initialized = true;
    ESP_LOGI(tag, "Initialized sensor type %d on UART%d.", type, uart_num);
    return handle;
    // vTaskDelay(pdMS_TO_TICKS(100));
}

// --- Data Acquisition (Refactored from tb600b_get_combined_data) ---
void tb600b_measure_and_update(tb600b_handle_t *handle)
{
    if (handle == nullptr || !handle->is_initialized)
        return;

    const int responseLength = 13;
    uint8_t responseData[responseLength];
    uart_port_t uart_num = handle->uart_num;
    const char *tag = handle->tag;

    // Send command
    uart_write_bytes(uart_num, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA));

    // Read response
    int bytesRead = uart_read_bytes(uart_num, responseData, responseLength, pdMS_TO_TICKS(1000));

    if (bytesRead == responseLength) {
        // Parse and store results directly into the handle
        int16_t rawTemperature = (int16_t)((responseData[8] << 8) | responseData[9]);
        handle->temperature = (float)rawTemperature / 100.0f;

        uint16_t rawHumidity = (uint16_t)((responseData[10] << 8) | responseData[11]);
        handle->humidity = (float)rawHumidity / 100.0f;
        uint16_t rawGasUg = (uint16_t)((responseData[2] << 8) | responseData[3]);
        handle->gas_ug = (float)rawGasUg;

        ESP_LOGI(tag, "Read OK. Gas: %.2f ug/m³", handle->gas_ug);
    }
    else {
        ESP_LOGE(tag, "Failed to receive complete data response (read %d/%d bytes)", bytesRead, responseLength);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}

float tb600b_get_gas_ug(tb600b_handle_t *handle)
{
    if (handle == nullptr)
        return 0.0f;
    return handle->gas_ug;
}

float tb600b_get_temperature(tb600b_handle_t *handle)
{
    if (handle == nullptr)
        return 0.0f;
    return handle->temperature;
}

float tb600b_get_humidity(tb600b_handle_t *handle)
{
    if (handle == nullptr)
        return 0.0f;
    return handle->humidity;
}

// The definition of tb600b_set_passive_mode
void tb600b_set_passive_mode(tb600b_handle_t *handle)
{
    if (handle == nullptr || !handle->is_initialized)
        return;

    uart_port_t uart_num = handle->uart_num;
    const char *tag = handle->tag;

    ESP_LOGI(tag, "SEND_CMD: Switching to Passive Mode (QnA) on UART%d.", uart_num);

    // Send the passive mode command
    uart_write_bytes(uart_num, CMDSET_MODE_PASSIVE_UPLOAD, sizeof(CMDSET_MODE_PASSIVE_UPLOAD));

    // Give the sensor a moment to process the command
    vTaskDelay(pdMS_TO_TICKS(100));
}

void tb600b_set_led_turn_off(tb600b_handle_t *handle)
{
    if (handle == nullptr || !handle->is_initialized)
        return;

    uart_port_t uart_num = handle->uart_num;
    const char *tag = handle->tag;

    ESP_LOGI(tag, "SEND_CMD: turn off lcd%d.", uart_num);

    // Send the passive mode command
    uart_write_bytes(uart_num, CMD_TURN_OFF_LED, sizeof(CMD_TURN_OFF_LED));

    // Give the sensor a moment to process the command
    vTaskDelay(pdMS_TO_TICKS(100));
}

void tb600b_set_led_turn_on(tb600b_handle_t *handle)
{
    if (handle == nullptr || !handle->is_initialized)
        return;

    uart_port_t uart_num = handle->uart_num;
    const char *tag = handle->tag;

    ESP_LOGI(tag, "SEND_CMD: turn on lcd%d.", uart_num);

    // Send the passive mode command
    uart_write_bytes(uart_num, CMD_TURN_OFF_LED, sizeof(CMD_TURN_OFF_LED));

    // Give the sensor a moment to process the command
    vTaskDelay(pdMS_TO_TICKS(100));
}