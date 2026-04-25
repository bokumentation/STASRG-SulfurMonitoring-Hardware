/*

an Example for creating ESP-IDF Components.

*/

#include "my_components.h"
#include "esp_log.h"

// Define a tag for this component's logging output
static const char *TAG = "HELLO_LOG_COMP";

void hello_log_init(void)
{
    // Log the "Hello World" message at the INFO level
    ESP_LOGI(TAG, "Hello from my_components.c using ESP_LOGI");
    ESP_LOGW(TAG, "Hello from my_components.c ESP_LOGW");
    ESP_LOGD(
        TAG,
        "Hello from my_components.c ESP_LOGD"); // Only visible if CONFIG_LOG_DEFAULT_LEVEL is set to DEBUG or VERBOSE
}
