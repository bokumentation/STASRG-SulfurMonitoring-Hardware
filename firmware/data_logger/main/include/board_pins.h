#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include <sdkconfig.h>
#include <hal/gpio_hal.h>

// Define Board Type. If you use HELTEC Wireless Stick Lite, please uncomment it.
// #define BOARD_DEVKITC_ESP32S3_V1
#define BOARD_HELTEC_WSL_ESP32S3_V3

#ifdef CONFIG_IDF_TARGET_ESP32
    #define ANEMOMETER_ADC_PIN GPIO_NUM_32

    #define H2S_UART_PORT UART_NUM_1
    #define H2S_RX_PIN GPIO_NUM_26
    #define H2S_TX_PIN GPIO_NUM_27
    #define H2S_LOG_TAG "TB600B_H2S"

    #define SO2_UART_PORT UART_NUM_2
    #define SO2_RX_PIN GPIO_NUM_16
    #define SO2_TX_PIN GPIO_NUM_17
    #define SO2_LOG_TAG "TB600B_SO2"

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    #define ANEMOMETER_ADC_PIN GPIO_NUM_3

    #define H2S_UART_PORT UART_NUM_0
    #define H2S_RX_PIN GPIO_NUM_9
    #define H2S_TX_PIN GPIO_NUM_10
    #define H2S_LOG_TAG "TB600B_H2S"

    #define SO2_UART_PORT UART_NUM_1
    #define SO2_RX_PIN GPIO_NUM_20
    #define SO2_TX_PIN GPIO_NUM_21
    #define SO2_LOG_TAG "TB600B_SO2"

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #if defined(BOARD_DEVKITC_ESP32S3_V1)
        #define ANEMOMETER_ADC_PIN GPIO_NUM_3

        #define SO2_UART_PORT UART_NUM_0
        #define SO2_RX_PIN GPIO_NUM_44
        #define SO2_TX_PIN GPIO_NUM_43
        #define SO2_LOG_TAG "TB600B_SO2"

        #define H2S_UART_PORT UART_NUM_1
        #define H2S_RX_PIN GPIO_NUM_17
        #define H2S_TX_PIN GPIO_NUM_18
        #define H2S_LOG_TAG "TB600B_H2S"

    #elif defined(BOARD_HELTEC_WSL_ESP32S3_V3)
        // - 0. LoRa SX1262 PINS (FIXED by Heltec WSL v3) ---
        // These pins are hardwired to the onboard HELTEC LoRa module.
        #define PIN_LORA_NSS        8
        #define PIN_LORA_SCK        9
        #define PIN_LORA_MISO       10
        #define PIN_LORA_MOSI       11
        #define PIN_LORA_RST        12
        #define PIN_LORA_BUSY       13
        #define PIN_LORA_DIO1       14

        // - 1. SPI BUS (SD Card and W5500 Ethernet) ---
        // -- 1.1 Dedicated SPI bus (SPI3/VSPI) using accessible GPIO pins
        #define PIN_SPI3_SCK         3
        #define PIN_SPI3_MOSI        4
        #define PIN_SPI3_MISO        5
        // -- 1.2 Select Pin for SPI3 connected modules
        #define PIN_SD_CS           6     
        #define PIN_W5500_CS        2

        // - 2. I2C BUS (RTC, OLED, INA219 x2) ---
        #define PIN_I2C_SCL         20
        #define PIN_I2C_SDA         21
        // -- 2.1. I2C ADDRESS for the modules ---
        #define ADDR_DS3231_RTC
        #define ADDR_SSD1306_DISPLAY
        #define ADDR_INA219_SENSOR_1
        #define ADDR_INA219_SENSOR_2
        // -- 2.2. Additional Control PIN ---
        #define PIN_RTC_INT_SQW     19

        // - 3. UART PERIPHERALS (Sensors and GPS) ---      
        // -- 3.1. UART 1 (Hardware) - TB600B H2S Sensor: ✅ TESTED
        #define SENSOR_H2S_UART_PORT UART_NUM_1
        #define SENSOR_H2S_TAG "TB600B_H2S"
        #define PIN_H2S_TX          17  // YELLOW
        #define PIN_H2S_RX          18  // WHITE
        // -- 3.2. UART 2 (Hardware) - TB600B SO2 Sensor: ✅ TESTED
        #define SENSOR_SO2_UART_PORT UART_NUM_2
        #define SENSOR_SO2_TAG "TB600B_SO2"
        #define PIN_SO2_TX          40  // YELLO
        #define PIN_SO2_RX          39  // WHITE
        // -- 3.3. UART GPS Module uses Software Serial
        #define PIN_GPS_TX          42
        #define PIN_GPS_RX          41

        // - 4. ANALOG (ADC) Inputs (Wind Speed and Sirection) ---
        #define PIN_ANEMOMETER_ADC  34 // ADC1_CH3
        #define PIN_WIND_DIR_ADC    33 // ADC1_CH4
    #endif
#else
    #error "Unsupported IDF_TARGET! Please set the target using idf.py set-target."

#endif // CONFIG_IDF_TARGET

#define BAUD_RATE 9600

#endif // BOARD_PINS_H