file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES arduino-esp32
)
