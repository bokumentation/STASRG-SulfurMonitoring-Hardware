#include "anemometer.h"
#include <Arduino.h> // For pinMode, digitalRead, micros, millis, etc.
#include <stdlib.h>
#include <math.h>

// Internal, private structure definition (hidden from the user/header file)
typedef struct anemometer_t {
    int pulse_pin;
    uint32_t measurement_interval_sec;
    const uint32_t debounce_micros; // 5000us = 5ms

    // State variables for pulse counting
    volatile uint32_t pulse_count;
    volatile uint64_t last_pulse_time; // micros() uses uint64_t on ESP32
    int last_pin_state;

    // State variables for measurement window
    uint64_t last_measure_time; // millis()
} anemometer_internal_t;

// --- Private Functions ---

static float calibrate_speed(float rot_per_sec) {
    // Calibration formula: (-0.0181 * R^2) + (1.3859 * R) + 1.4055
    return (-0.0181 * powf(rot_per_sec, 2.0f)) + (1.3859f * rot_per_sec) + 1.4055f;
}

// --- Public API Functions ---

anemometer_handle_t anemometer_create(int pin, uint32_t measurement_interval_sec) {
    // Dynamically allocate memory for the internal structure
    anemometer_internal_t *sensor = (anemometer_internal_t*)malloc(sizeof(anemometer_internal_t));

    if (sensor == NULL) {
        return NULL; // Allocation failed
    }

    // Initialize all members
    sensor->pulse_pin = pin;
    sensor->measurement_interval_sec = measurement_interval_sec;
    // Using a constant member to ensure it can't be accidentally changed
    *(uint32_t*)&sensor->debounce_micros = 5000; 

    sensor->pulse_count = 0;
    sensor->last_pulse_time = 0;
    sensor->last_pin_state = LOW;
    sensor->last_measure_time = 0;

    return (anemometer_handle_t)sensor;
}

void anemometer_begin(anemometer_handle_t handle) {
    if (handle == NULL) return;
    anemometer_internal_t *sensor = (anemometer_internal_t*)handle;
    
    // Set up the input pin
    pinMode(sensor->pulse_pin, INPUT_PULLUP);
    sensor->last_measure_time = millis();
}

bool anemometer_read_speed(anemometer_handle_t handle, 
                           float *rot_per_sec, 
                           float *wind_speed_mps, 
                           float *wind_speed_kph) {
    
    if (handle == NULL) return false;
    anemometer_internal_t *sensor = (anemometer_internal_t*)handle;

    // Use volatile keyword for `now_micros` to prevent compiler optimization issues
    uint64_t now_micros = micros();
    int current_pin_state = digitalRead(sensor->pulse_pin);

    // --- 1. Pulse Polling and Debounce Logic ---
    // Check for a RISING edge (LOW -> HIGH) AND debounce time has passed
    if (current_pin_state == HIGH && sensor->last_pin_state == LOW) {
        if (now_micros - sensor->last_pulse_time >= sensor->debounce_micros) {
            sensor->pulse_count++;
            sensor->last_pulse_time = now_micros;
        }
    }
    sensor->last_pin_state = current_pin_state;

    // --- 2. Measurement Window Check and Calculation ---
    uint64_t now_millis = millis();

    if (now_millis - sensor->last_measure_time >= sensor->measurement_interval_sec * 1000UL) {
        
        // Calculate Rotations Per Second
        *rot_per_sec = (float)sensor->pulse_count / (float)sensor->measurement_interval_sec;

        // Apply Calibration
        *wind_speed_mps = calibrate_speed(*rot_per_sec);

        // Minimum speed check (as per original code)
        if (*wind_speed_mps <= 1.5f) { 
            *wind_speed_mps = 0.0f;
        }

        // Calculate Kilometers Per Hour
        *wind_speed_kph = *wind_speed_mps * 3.6f;

        // Reset for the next measurement
        sensor->pulse_count = 0;
        sensor->last_measure_time = now_millis;

        return true; // A new measurement is available
    }

    return false; // No measurement yet
}

void anemometer_destroy(anemometer_handle_t handle) {
    if (handle != NULL) {
        // Free the dynamically allocated memory
        free((anemometer_internal_t*)handle);
    }
}