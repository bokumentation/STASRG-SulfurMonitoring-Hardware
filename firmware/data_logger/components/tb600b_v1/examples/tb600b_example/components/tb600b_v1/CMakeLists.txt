# Define the component and its properties
idf_component_register(
    # Source files to compile
    SRCS "tb600b.cpp"
    
    # Directory for public header files (must be 'include')
    INCLUDE_DIRS "include"
    
    # Dependencies: We use esp_log, which is part of the 'log' component.
    # We explicitly depend on it to be safe, although it's often included globally.
    REQUIRES log cxx esp_driver_uart main
)