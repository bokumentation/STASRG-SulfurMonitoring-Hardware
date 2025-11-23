#ifndef TB600B_SO2_H
#define TB600B_SO2_H

#include "hal/uart_types.h"

// static const char *TAG_START = "[START]";
// static const char *TAG_TASK_TBxSO2 = "[TASK_TBxS02]";

/* --- SO2 GAS SENSOR TYPE: 0x24--- */
/* --- USED COMMANDS ---- */
// ✅ Cmd 6: to get combined gas concentration, temperature, and humidity:
// [2]=0x87 Return: 13 bytes
static const uint8_t CMD_GET_COMBINED_DATA[] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

// ✅ Command to CHECK LED status
// Return: 0 or 1
static const uint8_t CMD_GET_LED_STATUS[] = {0xFF, 0x01, 0x8A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75};

// ✅ Command to turn OFF LED
// Return: OK
static const uint8_t CMD_TURN_OFF_LED[] = {0xFF, 0x01, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77};

// ✅ Command to turn ON LED
// Return: OK
static const uint8_t CMD_TURN_ON_LED[] = {0xFF, 0x01, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76};

// ✅ Command 2: Switches to PASSIVE UPLOAD (sensor waits for commands)
// Return: ❌
static const uint8_t CMDSET_MODE_PASSIVE_UPLOAD[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};

/* --- ⚠️⚠️ UNUSED COMMANDS ⚠️⚠️ ---- */
/* --- COMMANDS: MODE SWITCHING ---- */
// ✅ Command 1: Switches to active upload mode
// Return: ❌
// static const uint8_t CMDSET_MODE_ACTIVE_UPLOAD[] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};

/* --- COMMAND: DATA QUERY MODE --- */
// Command 3: to get sensor type, max range, and unit: 0xD1
// Return: 9 bytes
// static const uint8_t CMD_GET_SENSOR_INFO[] = {0xFF, 0x01, 0xD1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35};

// Command 4: to get sensor type, max range, unit, and decimal places: [1]=0xD7
// Return: 9 bytes
// static const uint8_t CMD_GET_SENSOR_INFO_2[] = {0xFF, 0xD7, 0x24, 0x00, 0xC8, 0x02, 0x01, 0x00, 0x3A};

// Command 9: to get current version number (ASSUMTION) the return are 5 bytes
// Return: 6 bytes
// static const uint8_t CMD_GET_VERSION[] = {0xFF, 0x01, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* --- COMMAND: ACTIVE UPLOAD MODE --- */
// ✅ Command 5: ACTIVE read gas concentration: [1]=0x01 [2]=0x86
// Return: 9 bytes
// static const uint8_t CMD_GET_GAS_CONCENTRATION[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

/* --- H2S GAS SENSOR TYPE: 0x1C--- */
// Command 4: to get sensor type, max range, unit, and decimal places: [1]=0xD7
// Return: 9 bytes
// static const uint8_t h2s_commandGetSensorInfo2[] = {0xFF, 0xD7, 0x1C, 0x00, 0xC8, 0x02, 0x01, 0x00, 0x3A};

// SENSOR DATA COMMAND
void tb600b_init_uart(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate, const char *tag);
void tb600b_read_confirmation();
void tb600b_read_status_response();
void tb600b_get_combined_data(uart_port_t uart_num, const uint8_t *command, size_t commandSize, const char *tag,
                              float *out_temperature, float *out_humidity, float *out_gasUg);
void tb600b_set_passive_mode(uart_port_t uart_num);

// LED COMMAND
void led_read_confirmation(uart_port_t uart_num);
void led_read_status_response(uart_port_t uart_num);
void led_get_led_status(uart_port_t uart_num);
void led_turn_off_led(uart_port_t uart_num);
void led_turn_on_led(uart_port_t uart_num);
void led_set_passive_mode(uart_port_t uart_num);

#endif // TB600B_SO2_H