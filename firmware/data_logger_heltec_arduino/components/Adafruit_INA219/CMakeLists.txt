idf_component_register(
    SRCS "Adafruit_INA219.cpp"

    INCLUDE_DIRS "."

    REQUIRES arduino-esp32 Adafruit-GFX-Library Adafruit_SSD1306 Adafruit_BusIO
)
