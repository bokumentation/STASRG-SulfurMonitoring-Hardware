#ifndef LED_FUNC_H
#define LED_FUNC_H

#include <tb600b_cmd.h>
#include "hal/uart_types.h"

// Define
#define TB600_TAG_LED "TB600_LED"

// LED COMMANDs
void led_read_confirmation(uart_port_t uart_num, const char *tag);
void led_read_status_response(uart_port_t uart_num, const char *tag);
void led_get_led_status(uart_port_t uart_num, const char *tag);
void led_turn_off_led(uart_port_t uart_num, const char *tag);
void led_turn_on_led(uart_port_t uart_num, const char *tag);
void led_set_passive_mode(uart_port_t uart_num, const char *tag);

#endif // LED_FUNC_H