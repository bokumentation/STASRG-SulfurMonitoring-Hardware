idf_component_register(
    SRCS "sensor_task.cpp" "main.cpp" "display/ui_ssd1306.cpp" "sensor/anemometer.cpp" "sensor/tb600_sensor.cpp" "battery/battery_indicator.cpp"
    PRIV_REQUIRES esp_driver_tsens fatfs Adafruit_SSD1306 ra01s arduino-esp32
    INCLUDE_DIRS "." "include"
)
