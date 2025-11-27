// tb600b_cmd.cpp
#include "tb600b_cmd.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "hal/uart_types.h"

void tb600b_set_passive_mode(uart_port_t uart_num, const char *tag)
{
    ESP_LOGI(tag, "SEND_CMD: Switching to Passive Mode");
    uart_write_bytes(uart_num, CMDSET_MODE_PASSIVE_UPLOAD, sizeof(CMDSET_MODE_PASSIVE_UPLOAD));
    vTaskDelay(pdMS_TO_TICKS(100));
};