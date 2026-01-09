idf_component_register(
    SRCS "Adafruit_BME280.cpp"

    INCLUDE_DIRS "."

    REQUIRES arduino-esp32 Adafruit_Sensor Adafruit_BusIO
)
