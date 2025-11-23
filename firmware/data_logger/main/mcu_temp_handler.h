#pragma once

/**
 * @brief Initializes the internal MCU temperature sensor.
 * @note The parameter temperature_sensor_handle_t *temp_sensor was removed 
 * because the handle is stored in the static global variable 'temp_sensor_handle'.
 */
void mcu_temp_init(void);

/**
 * @brief Reads the temperature from the internal MCU sensor.
 * @note Added a return type (void) and corrected the function signature.
 */
void mcu_temp_read(void);

/**
 * @brief Gets the last recorded MCU temperature value.
 * @note Corrected the function name from tb600b_get_humidity to a more appropriate name.
 */
float mcu_get_temperature(void);