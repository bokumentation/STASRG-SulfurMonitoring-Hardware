// led_func.cpp

#include "led_func.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "hal/uart_types.h"

void led_read_confirmation(uart_port_t uart_num, const char *tag)
{
    const int responseLength = 2;
    uint8_t responseData[responseLength];
    int bytesRead = uart_read_bytes(uart_num, responseData, responseLength, pdMS_TO_TICKS(1000));

    if (bytesRead == responseLength) {
        if (responseData[0] == 0x4F && responseData[1] == 0x4B) {
            ESP_LOGI(tag, "LED: Received 'OK' confirmation.");
        }
        else {
            ESP_LOGW(tag, "LED: Received unexpected response for confirmation.");
            ESP_LOG_BUFFER_HEXDUMP(tag, responseData, bytesRead, ESP_LOG_INFO);
        }
    }
    else {
        ESP_LOGE(tag, "LED: Failed to receive 'OK' confirmation within timeout.");
    }
}

void led_read_status_response(uart_port_t uart_num, const char *tag)
{
    const int responseLength = 9;
    uint8_t responseData[responseLength];
    int bytesRead = uart_read_bytes(uart_num, responseData, responseLength, pdMS_TO_TICKS(100));

    if (bytesRead == responseLength) {
        ESP_LOGI(tag, "Received Status Response:");
        ESP_LOG_BUFFER_HEXDUMP(tag, responseData, responseLength, ESP_LOG_INFO);

        if (responseData[2] == 0x01) {
            ESP_LOGI(tag, "LED Status: ON (0x01)");
        }
        else if (responseData[2] == 0x00) {
            ESP_LOGI(tag, "LED Status: OFF (0x00)");
        }
        else {
            ESP_LOGW(tag, "LED Status: Unknown");
        }
    }
    else {
        ESP_LOGE(tag, "LED Status: Failed to receive status response within timeout.");
    }
}

void led_get_led_status(uart_port_t uart_num, const char *tag)
{
    ESP_LOGI(tag, "LED: CHECK LED STATUS.");
    uart_write_bytes(uart_num, CMD_GET_LED_STATUS, sizeof(CMD_GET_LED_STATUS));
    led_read_status_response(uart_num, tag);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void led_turn_off_led(uart_port_t uart_num, const char *tag)
{
    ESP_LOGI(tag, "LED: TURN OFF LED");
    uart_write_bytes(uart_num, CMD_TURN_OFF_LED, sizeof(CMD_TURN_OFF_LED));
    led_read_confirmation(uart_num, tag);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void led_turn_on_led(uart_port_t uart_num, const char *tag)
{
    ESP_LOGI(tag, "LED: TURN ON LED");
    uart_write_bytes(uart_num, CMD_TURN_ON_LED, sizeof(CMD_TURN_ON_LED));
    led_read_confirmation(uart_num, tag);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void led_set_passive_mode(uart_port_t uart_num, const char *tag)
{
    ESP_LOGI(tag, "LED: Switching Passive Mode or QnA.");
    uart_write_bytes(uart_num, CMDSET_MODE_PASSIVE_UPLOAD, sizeof(CMDSET_MODE_PASSIVE_UPLOAD));
    vTaskDelay(pdMS_TO_TICKS(100));
}