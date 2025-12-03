idf_component_register(
    SRCS "main.cpp" "ui_ssd1306.cpp"
    PRIV_REQUIRES tb600b_v1 esp_driver_tsens fatfs Adafruit_SSD1306 ra01s
    INCLUDE_DIRS "." "include"
)
