# components/my_components/CMakeLists.txt
# Define the component and its properties

# Search all ".c" and ".cpp" file
file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

idf_component_register(
    # Source files to compile
    SRCS ${SOURCES}

    # Directory for public header files
    INCLUDE_DIRS "."

    # Required Dependencies: We use esp_log, which is part of the 'log' component.
    PRIV_REQUIRES log
)
