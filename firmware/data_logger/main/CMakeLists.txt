idf_component_register(
    SRCS "main.cpp"
    PRIV_REQUIRES tb600b_v1 esp_driver_tsens fatfs
    INCLUDE_DIRS "." "include"
)
