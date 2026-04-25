idf_component_register(
    SRCS "sensor_task.cpp" "main.cpp" "display/ssd1306.cpp" "sensor/anemometer.cpp" "sensor/tb600_sensor.cpp" "battery/ina219.cpp"
    PRIV_REQUIRES Adafruit-GFX-Library esp_driver_tsens fatfs Adafruit_SSD1306 ra01s RadioLib RTClib TinyGPSPlus Adafruit_BME280 espsoftwareserial Adafruit_INA219
    INCLUDE_DIRS "." "include"
)
