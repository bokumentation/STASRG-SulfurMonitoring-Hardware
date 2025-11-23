#include "tb600b.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"

static const int RX_BUF_SIZE = 128;
static const char *TAG_UART_SENSOR = "TB600B_UART"; // Tag for ESP_LOG

void tb600b_init_uart(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate, const char *tag)
{
    const uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    }; // Missing 'rx_flow_ctrl_thresh' initializer but it not needed.

    ESP_ERROR_CHECK(uart_driver_install(uart_num, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(tag, "Initialize...");
    vTaskDelay(pdMS_TO_TICKS(100));
}

void tb600b_get_combined_data(uart_port_t uart_num, const uint8_t *command, size_t commandSize, const char *tag,
                              float *out_temperature, float *out_humidity, float *out_gasUg)
{
    const int responseLength = 13;
    uint8_t responseData[responseLength];

    if (!out_temperature || !out_humidity || !out_gasUg)
    {
        ESP_LOGE(tag, "One or more output pointers were NULL. Data cannot be stored.");
        return;
    }

    // Sending COMMAND to the sensor.
    uart_write_bytes(uart_num, command, commandSize);

    // Read RESPONSE from the sensor.
    int bytesRead = uart_read_bytes(uart_num, responseData, responseLength, pdMS_TO_TICKS(1000));

    if (bytesRead == responseLength)
    {
        // ESP_LOGI(tag, "Received Combined Data Response:");
        ESP_LOG_BUFFER_HEXDUMP(tag, responseData, responseLength, ESP_LOG_INFO);

        if (responseData[0] == 0xFF && responseData[1] == 0x87)
        {
            // Parse temperature from bytes 8 and 9
            int16_t rawTemperature = (int16_t)((responseData[8] << 8) | responseData[9]);
            float temperature = (float)rawTemperature / 100.0;

            // Parse humidity from bytes 10 and 11
            uint16_t rawHumidity = (uint16_t)((responseData[10] << 8) | responseData[11]);
            float humidity = (float)rawHumidity / 100.0;

            // Parse gas concentration (ug/m³) from bytes 2 and 3
            uint16_t rawGasUg = (uint16_t)((responseData[2] << 8) | responseData[3]);
            float gasUg = (float)rawGasUg;

            // Store the results using the dereference operator
            *out_temperature = temperature;
            *out_humidity = humidity;
            *out_gasUg = gasUg;
        }
        else
        {
            ESP_LOGW(tag, "Received malformed response header.");
        }
    }
    else
    {
        ESP_LOGE(tag, "Failed to receive complete combined data response within timeout.");
    }

    // Add delay
    vTaskDelay(pdMS_TO_TICKS(100));
}

void led_read_confirmation(uart_port_t uart_num)
{
    const int responseLength = 2;
    uint8_t responseData[responseLength];
    int bytesRead = uart_read_bytes(uart_num, responseData, responseLength, pdMS_TO_TICKS(1000));

    if (bytesRead == responseLength)
    {
        if (responseData[0] == 0x4F && responseData[1] == 0x4B)
        {
            ESP_LOGI(TAG_UART_SENSOR, "Received: 'OK' confirmation.");
        }
        else
        {
            ESP_LOGW(TAG_UART_SENSOR, "Received unexpected response for confirmation.");
            ESP_LOG_BUFFER_HEXDUMP(TAG_UART_SENSOR, responseData, bytesRead, ESP_LOG_INFO);
        }
    }
    else
    {
        ESP_LOGE(TAG_UART_SENSOR, "Failed to receive 'OK' confirmation within timeout.");
    }
}

void led_read_status_response(uart_port_t uart_num)
{
    const int responseLength = 9;
    uint8_t responseData[responseLength];
    int bytesRead = uart_read_bytes(uart_num, responseData, responseLength, pdMS_TO_TICKS(1000));

    if (bytesRead == responseLength)
    {
        ESP_LOGI(TAG_UART_SENSOR, "Received Status Response:");
        ESP_LOG_BUFFER_HEXDUMP(TAG_UART_SENSOR, responseData, responseLength, ESP_LOG_INFO);

        if (responseData[2] == 0x01)
        {
            ESP_LOGI(TAG_UART_SENSOR, "Light Status: ON (0x01)");
        }
        else if (responseData[2] == 0x00)
        {
            ESP_LOGI(TAG_UART_SENSOR, "Light Status: OFF (0x00)");
        }
        else
        {
            ESP_LOGW(TAG_UART_SENSOR, "Light Status: Unknown");
        }
    }
    else
    {
        ESP_LOGE(TAG_UART_SENSOR, "Failed to receive status response within timeout.");
    }
}

void led_get_led_status(uart_port_t uart_num)
{
    ESP_LOGI(TAG_UART_SENSOR, "SEND_CMD: CHECK LED STATUS.");
    uart_write_bytes(uart_num, CMD_GET_LED_STATUS, sizeof(CMD_GET_LED_STATUS));
    led_read_status_response(uart_num);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void led_turn_off_led(uart_port_t uart_num)
{
    ESP_LOGI(TAG_UART_SENSOR, "SEND_CMD: TURN OFF LED.");
    uart_write_bytes(uart_num, CMD_TURN_OFF_LED, sizeof(CMD_TURN_OFF_LED));
    led_read_confirmation(uart_num);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void led_turn_on_led(uart_port_t uart_num)
{
    ESP_LOGI(TAG_UART_SENSOR, "SEND_CMD: TURN ON LED.");
    uart_write_bytes(uart_num, CMD_TURN_ON_LED, sizeof(CMD_TURN_ON_LED));
    led_read_confirmation(uart_num);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void led_set_passive_mode(uart_port_t uart_num)
{
    ESP_LOGI(TAG_UART_SENSOR, "SEND_CMD: Switching Passive Mode or QnA.");
    uart_write_bytes(uart_num, CMDSET_MODE_PASSIVE_UPLOAD, sizeof(CMDSET_MODE_PASSIVE_UPLOAD));
    vTaskDelay(pdMS_TO_TICKS(100));
}
