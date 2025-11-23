#include "mcu_temp_handler.h"
#include "driver/temperature_sensor.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Define the logging tag
static const char *TAG = "MCU_TEMP";

// Global variable to store the last temperature reading
// NOTE: For thread safety, access to this should ideally be protected by a mutex
// if used by multiple tasks, but for this simple example, we'll keep it as is.
static float mcu_temperature_value = 0.0f;

// Static (file-scope) variable to hold the sensor handle
// This is necessary because the handle is installed in one function (init)
// and needs to be used in another (read).
static temperature_sensor_handle_t temp_sensor_handle = NULL;

/**
 * @brief Initializes the internal MCU temperature sensor.
 * @note The parameter temperature_sensor_handle_t *temp_sensor was removed
 * because the handle is stored in the static global variable 'temp_sensor_handle'.
 */
void mcu_temp_init(void)
{
    ESP_LOGI(TAG, "Install temperature sensor, expected temp range: 10~50 ℃");

    // Check if the handle is already installed (good practice)
    if (temp_sensor_handle != NULL) {
        ESP_LOGW(TAG, "Temperature sensor already initialized.");
        return;
    }

    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);

    // The handle is passed by address to the install function, and the result
    // is stored in the static global variable 'temp_sensor_handle'.
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor_handle));

    ESP_LOGI(TAG, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor_handle));
}

/**
 * @brief Reads the temperature from the internal MCU sensor.
 * @note Added a return type (void) and corrected the function signature.
 */
void mcu_temp_read(void)
{
    ESP_LOGD(TAG, "Reading temperature..."); // Use D for debug or I for info
    float tsens_value;

    if (temp_sensor_handle == NULL) {
        ESP_LOGE(TAG, "Temperature sensor not initialized!");
        return;
    }

    // Use the stored static sensor handle
    esp_err_t err = temperature_sensor_get_celsius(temp_sensor_handle, &tsens_value);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Temperature value %.02f ℃", tsens_value);
        // Store the value in the dedicated temperature variable
        mcu_temperature_value = tsens_value;
    }
    else {
        ESP_LOGE(TAG, "Error reading temperature: %s", esp_err_to_name(err));
    }
}

/**
 * @brief Gets the last recorded MCU temperature value.
 * @note Corrected the function name from tb600b_get_humidity to a more appropriate name.
 */
float mcu_get_temperature(void)
{
    // Return the global temperature value
    return mcu_temperature_value;
}