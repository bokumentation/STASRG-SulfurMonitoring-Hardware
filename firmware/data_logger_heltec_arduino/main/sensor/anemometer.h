#ifndef ANEMOMETER_H
#define ANEMOMETER_H

#include <stdint.h>
#include <stdbool.h>

// Opaque type for the sensor handle. Users only see this pointer.
typedef struct anemometer_t* anemometer_handle_t;

/**
 * @brief Structure to hold the anemometer measurement results.
 */
typedef struct {
    float rot_per_sec;  ///< Rotations per second (RPS)
    float wind_speed_mps; ///< Wind speed in meters per second (m/s)
    float wind_speed_kph; ///< Wind speed in kilometers per hour (km/h)
} anemometer_data_t;

/**
 * @brief Creates and initializes the anemometer structure (the "object").
 * @param pin The GPIO pin connected to the anemometer pulse output.
 * @param measurement_interval_sec The time window for pulse counting in seconds.
 * @return anemometer_handle_t The handle to the initialized sensor context, or NULL on failure.
 */
anemometer_handle_t anemometer_create(int pin, uint32_t measurement_interval_sec);

/**
 * @brief Sets up the necessary GPIO pin mode.
 * @param handle Pointer to the sensor context created by anemometer_create.
 */
void anemometer_begin(anemometer_handle_t handle);

/**
 * @brief Polls the sensor pin and calculates wind speed if the time window is met.
 *
 * This function handles pulse counting, debouncing, and the measurement timing.
 *
 * @param handle Pointer to the sensor context.
 * @param rot_per_sec Output: Rotations per second.
 * @param wind_speed_mps Output: Wind speed in meters per second (m/s).
 * @param wind_speed_kph Output: Wind speed in kilometers per hour (km/h).
 * @return bool True if a new measurement was calculated and updated, false otherwise.
 */
bool anemometer_read_speed(anemometer_handle_t handle, 
                          anemometer_data_t *data);

/**
 * @brief Cleans up the dynamically allocated anemometer context.
 * @param handle Pointer to the sensor context to be destroyed.
 */
void anemometer_destroy(anemometer_handle_t handle);

#endif // ANEMOMETER_H