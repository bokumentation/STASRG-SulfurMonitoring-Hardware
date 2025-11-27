#include "anemometer.h"
#include "esp_log.h"
// #include "driver/gpio.h"
#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <new>      // For std::nothrow and new
#include <stdlib.h> // For free in deinit

static const char *pTAG_ANEMOMETER = "ANEMOMETER";
// 10 seconds in microseconds
const uint64_t TIMEMEASURE_US = 10000000ULL;

// --- ISR (Interrupt Service Routine) ---
// Must be static and IRAM_ATTR. arg is the pointer to the handle.
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    // Cast the void* argument back to the specific handle struct
    anemometer_handle_t *handle = (anemometer_handle_t *)arg;

    // Use atomic operation on the instance's rpm_count for thread safety
    __atomic_add_fetch(&(handle->rpm_count), 1, __ATOMIC_RELAXED);
}

// --- Setup Function ---
anemometer_handle_t *anemometer_init(gpio_num_t pin_number)
{
    // 1. Allocate memory for the new instance using C++ new
    anemometer_handle_t *handle = new (std::nothrow) anemometer_handle_t;
    if (handle == nullptr) {
        ESP_LOGE(pTAG_ANEMOMETER, "Failed to allocate memory for handle.");
        return nullptr;
    }

    // 2. Initialize the struct members
    handle->pin_num = pin_number;
    handle->rpm_count = 0;
    handle->speed_kmph = 0.0f;
    handle->speed_mps = 0.0f;
    handle->is_initialized = false; // Set to true only upon successful config

    // 3. Configure the GPIO pin
    gpio_config_t io_conf = {};                   // Zero-initialize the struct
    io_conf.intr_type = GPIO_INTR_POSEDGE;        // RISING edge interrupt
    io_conf.mode = GPIO_MODE_INPUT;               // Set as input
    io_conf.pin_bit_mask = (1ULL << pin_number);  // Only for the specified pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // No pull-down
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      // Enable internal pull-up resistor

    if (gpio_config(&io_conf) != ESP_OK) {
        ESP_LOGE(pTAG_ANEMOMETER, "GPIO config failed for pin %d.", pin_number);
        delete handle; // C++ cleanup
        return nullptr;
    }

    // 4. Install the global GPIO ISR handler (safe to call multiple times)
    gpio_install_isr_service(0);

    // 5. Hook the specific ISR function to the pin, passing the HANDLE as the argument
    // This connects the ISR to the specific instance's data.
    if (gpio_isr_handler_add(pin_number, gpio_isr_handler, (void *)handle) != ESP_OK) {
        ESP_LOGE(pTAG_ANEMOMETER, "ISR add failed for pin %d.", pin_number);
        delete handle;
        return nullptr;
    }

    // 6. Finalize initialization
    handle->time_old_us = esp_timer_get_time();
    handle->is_initialized = true;

    ESP_LOGI(pTAG_ANEMOMETER, "Anemometer initialized on GPIO %d. Handle: %p", pin_number, (void *)handle);
    return handle;
}

// --- Deinitialization Function ---
void anemometer_deinit(anemometer_handle_t *handle)
{
    if (handle == nullptr || !handle->is_initialized)
        return;

    // 1. Remove the ISR hook
    gpio_isr_handler_remove(handle->pin_num);

    // 2. Reset the pin mode (optional, but good practice)
    gpio_reset_pin(handle->pin_num);

    // 3. Free the memory using C++ delete
    delete handle;

    ESP_LOGI(pTAG_ANEMOMETER, "Anemometer on GPIO %d deinitialized.", handle->pin_num);
}

// --- Loop/Measurement Function ---
void anemometer_measure_and_update(anemometer_handle_t *handle)
{
    if (handle == nullptr || !handle->is_initialized)
        return;

    uint64_t current_time_us = esp_timer_get_time();

    // Check if the measurement interval has passed (10 seconds)
    if ((current_time_us - handle->time_old_us) >= TIMEMEASURE_US) {
        // 1. Disable interrupt to safely read and reset rpm_count.
        gpio_intr_disable(handle->pin_num);

        // 2. Read count and reset
        unsigned int current_rpmcount = handle->rpm_count;
        handle->rpm_count = 0;
        handle->time_old_us = current_time_us; // Reset time

        // 3. Re-enable the interrupt.
        gpio_intr_enable(handle->pin_num);

        // 4. Perform calculations
        // Time interval in seconds (1000000.0f is 1 second in microseconds)
        const float time_interval_s = (float)TIMEMEASURE_US / 1000000.0f;
        float rotations_per_second = (float)current_rpmcount / time_interval_s;

        // Anemometer-specific conversion formula
        float speed_m_per_s =
            (-0.0181f * (rotations_per_second * rotations_per_second)) + (1.3859f * rotations_per_second) + 1.4055f;

        // Apply low-speed threshold
        if (speed_m_per_s <= 1.5f) {
            speed_m_per_s = 0.0f;
        }

        float speed_km_per_h = speed_m_per_s * 3.6f;

        // Store the calculated values back into the handle
        handle->speed_kmph = speed_km_per_h;
        handle->speed_mps = speed_m_per_s;

        ESP_LOGI(pTAG_ANEMOMETER, "[GPIO %d] RPS: %.2f | Speed (m/s): %.2f | Speed (km/h): %.2f", handle->pin_num,
                 rotations_per_second, handle->speed_mps, handle->speed_kmph);
    }
}

// --- Getter Functions ---
float anemometer_get_speed_kmph(anemometer_handle_t *handle)
{
    // Read protection is often implemented here, but simple return for efficiency
    if (handle == nullptr)
        return 0.0f;
    return handle->speed_kmph;
}

float anemometer_get_speed_mps(anemometer_handle_t *handle)
{
    if (handle == nullptr)
        return 0.0f;
    return handle->speed_mps;
}