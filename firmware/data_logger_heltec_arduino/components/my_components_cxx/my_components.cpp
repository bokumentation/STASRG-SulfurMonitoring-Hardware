/*

an Example for creating ESP-IDF Components.

*/

#include "my_components.hpp"
#include "esp_log.h"
#include <iostream>

// Use a unique TAG for logging this component
static const char *TAG = "CPP_CLASS";

// Constructor definition
MyCppClass::MyCppClass(const std::string &name) : instance_name(name)
{
    ESP_LOGI(TAG, "Instance '%s' constructed.", instance_name.c_str());
}

// Method definition
void MyCppClass::greet() const
{
    // Use standard C++ output for demonstration
    std::cout << "C++ Component: Hello, world! I am instance '" << instance_name << "'." << std::endl;

    // Also log via ESP-IDF system
    ESP_LOGI(TAG, "Greeting complete.");
}
