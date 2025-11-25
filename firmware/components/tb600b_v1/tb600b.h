#ifndef TB600B_SO2_H
#define TB600B_SO2_H

#include "esp_err.h"
#include "hal/uart_types.h"
#include <tb600b_cmd.h>

/**
 * @brief Structure to hold the combined sensor data.
 */
typedef struct {
    float temperature_c; // Temperature in Celsius
    float humidity_perc; // Humidity in percentage
    float gas_ugm3;      // Gas concentration in ug/m³
} tb600b_combined_data_t;

// --- MOLECULAR WEIGHTS (g/mol) for accurate conversions ---
// These values are standard for STP conversions.
#define M_W_SO2 64.06f // Sulfur Dioxide
#define M_W_H2S 34.08f // Hydrogen Sulfide

static const int RX_BUF_SIZE = 128;

#define TB600B_RESPONSE_HEADER_BYTE1 0xFF
#define TB600B_RESPONSE_HEADER_BYTE2 0x87
#define TB600B_RESPONSE_LENGTH 13
#define TB600_TAG_UART_SENSOR "TB600_LIBS"

// SENSOR DATA COMMAND
void tb600b_init_uart(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate, const char *tag);
void tb600b_read_confirmation();
void tb600b_read_status_response();
void tb600b_get_combined_data(uart_port_t uart_num, const uint8_t *command, size_t commandSize, const char *tag,
                              float *out_temperature, float *out_humidity, float *out_gasUg);
esp_err_t tb600b_read_combined_data(uart_port_t uart_num, const uint8_t *command, size_t commandSize,
                                    tb600b_combined_data_t *data_out);
void tb600b_set_passive_mode(uart_port_t uart_num);

// LED COMMAND
void led_read_confirmation(uart_port_t uart_num);
void led_read_status_response(uart_port_t uart_num);
void led_get_led_status(uart_port_t uart_num);
void led_turn_off_led(uart_port_t uart_num);
void led_turn_on_led(uart_port_t uart_num);
void led_set_passive_mode(uart_port_t uart_num);

/**
 * @brief Converts gas concentration from micrograms per cubic meter (ug/m³) to parts per million (ppm),
 * correcting for the ambient temperature.
 * * @param ugm3_concentration Concentration in ug/m³.
 * @param temperature_c Ambient temperature in Celsius (T in the Ideal Gas Law).
 * @param molecular_weight Molecular weight of the gas in g/mol (M).
 * @return float The concentration in ppm.
 */
float tb600b_convert_ugm3_to_ppm(float ugm3_concentration, float temperature_c, float molecular_weight);

#endif // TB600B_SO2_H