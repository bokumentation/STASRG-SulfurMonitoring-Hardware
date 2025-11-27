#ifndef ANEMOMETER_H
#define ANEMOMETER_H

#include "driver/gpio.h"
#include <stdbool.h>
#include <stdint.h>

// Define the Anemometer Context Structure (the "object").
// It holds all the state for a single anemometer instance.
typedef struct {
    gpio_num_t pin_num;              // The GPIO pin number for this instance
    volatile unsigned int rpm_count; // The counter for this instance (volatile for ISR access)
    uint64_t time_old_us;            // Last measurement time in microseconds
    float speed_kmph;                // Latest speed result in km/h
    float speed_mps;                 // Latest speed result in m/s
    bool is_initialized;             // Flag to check initialization status
} anemometer_handle_t;

// Public API Functions

/**
 * @brief Initializes a new anemometer instance.
 * @param pin_number The GPIO pin connected to the anemometer's pulse output.
 * @return A pointer to the initialized handle, or NULL on failure.
 */
anemometer_handle_t *anemometer_init(gpio_num_t pin_number);

/**
 * @brief Performs the measurement and updates the speed results for a specific handle.
 * @param handle The pointer to the anemometer instance.
 */
void anemometer_measure_and_update(anemometer_handle_t *handle);

/**
 * @brief Gets the latest wind speed in kilometers per hour.
 * @param handle The pointer to the anemometer instance.
 * @return Speed in km/h. Returns 0.0f if the handle is NULL.
 */
float anemometer_get_speed_kmph(anemometer_handle_t *handle);

/**
 * @brief Gets the latest wind speed in meters per second.
 * @param handle The pointer to the anemometer instance.
 * @return Speed in m/s. Returns 0.0f if the handle is NULL.
 */
float anemometer_get_speed_mps(anemometer_handle_t *handle);

/**
 * @brief Cleans up and frees the resources for the anemometer instance.
 * @param handle The pointer to the anemometer instance.
 */
void anemometer_deinit(anemometer_handle_t *handle);

#endif // ANEMOMETER_H