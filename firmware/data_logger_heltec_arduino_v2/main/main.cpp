#include "esp32-hal.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include <Arduino.h>

void setup()
{
}

void loop()
{
}

extern "C" void app_main()
{
    initArduino();
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}