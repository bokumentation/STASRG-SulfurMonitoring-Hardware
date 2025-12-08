idf_component_register(
    SRCS "main.cpp" "display/ui_ssd1306.cpp" "sensor/anemometer.cpp" "sensor/tb600_sensor.cpp"
    PRIV_REQUIRES esp_driver_tsens fatfs Adafruit_SSD1306 ra01s arduino-esp32
    INCLUDE_DIRS "." "include"
)
