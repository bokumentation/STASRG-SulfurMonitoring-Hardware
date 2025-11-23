idf_component_register(
    SRCS "main.cpp" "data_format.cpp" "mcu_temp_handler.cpp"
    PRIV_REQUIRES tb600b_v1 tb600b_v2 anemometer_v2 esp_driver_tsens
    INCLUDE_DIRS "." "include"
)
