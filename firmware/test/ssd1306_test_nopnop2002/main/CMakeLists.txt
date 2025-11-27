idf_component_register(
    SRCS "main.c"
    PRIV_REQUIRES ssd1306 esp_driver_spi esp_driver_i2c
    INCLUDE_DIRS "."
    )
