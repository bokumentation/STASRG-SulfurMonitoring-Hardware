idf_component_register(SRCS "main.cpp" "sd_card.cpp" "rtc.cpp"
                    REQUIRES RTClib Adafruit_BusIO TinyGPSPlus espsoftwareserial ghostl
                    INCLUDE_DIRS ".")
