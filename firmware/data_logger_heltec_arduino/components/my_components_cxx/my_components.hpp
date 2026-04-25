/**
 * @file my_components.hpp
 * @brief Component to log "hello world" using ESP_LOGI.
 */

#pragma once

#include <string>

// Define a simple C++ class for our component
class MyCppClass {
public:
    /**
     * @brief Constructor for the class.
     * @param name The name to associate with this instance.
     */
    MyCppClass(const std::string &name);

    /**
     * @brief Prints a personalized greeting using C++ features.
     */
    void greet() const;

private:
    std::string instance_name;
};
