#ifndef BOARD_PINS_H
#define BOARD_PINS_H

// BOARDS PIN DEFINITON
// SUPPORTED: ESP32 | ESP32C3 SUPERMINI | ESP32S3 DEVKITC | HELTEC WIRELESS STICK LITE V3

#include <hal/gpio_hal.h>
#include <sdkconfig.h>

// Define Board Type. If you use HELTEC Wireless Stick Lite, please uncomment it.
// #define BOARD_DEVKITC_ESP32S3_V1
#define BOARD_HELTEC_WSL_ESP32S3_V3

#ifdef CONFIG_IDF_TARGET_ESP32
    #define ANEMOMETER_ADC_PIN GPIO_NUM_32

    #define H2S_UART_PORT UART_NUM_1
    #define H2S_RX_PIN    GPIO_NUM_26
    #define H2S_TX_PIN    GPIO_NUM_27
    #define H2S_LOG_TAG   "TB600B_H2S"

    #define SO2_UART_PORT UART_NUM_2
    #define SO2_RX_PIN    GPIO_NUM_16
    #define SO2_TX_PIN    GPIO_NUM_17
    #define SO2_LOG_TAG   "TB600B_SO2"

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    // - 2. I2C BUS (RTC, OLED, INA219 x2) ---
    #define PIN_I2C_SCL 20 // YELLOW
    #define PIN_I2C_SDA 21 // WHITE
    #define I2C_MASTER_SCL_IO           PIN_I2C_SCL
    #define I2C_MASTER_SDA_IO           PIN_I2C_SDA
    #define I2C_MASTER_NUM              I2C_NUM_0
    #define I2C_MASTER_FREQ_HZ          100000
    // -- 2.1. I2C ADDRESS for the modules ---
    // --- 2.1.1. I2C MODULE: REALTIME CLOCK - DS3231 ---
    #define RTC_TAG                     "DS3231_RTC"
    #define PIN_RTC_INT_SQW             5 // Untested, Just place random
    #define I2C_HW_ADDR_RTC_DS3231      0x68
    #define I2C_HW_ADDR_RTC_AT24C32     0x57
    // --- 2.1.2. I2C MODULE: DISPLAY - SSD1306 ---
    #define DISPLAY_TAG                 "SSD1306_DISPLAY"
    #define I2C_HW_ADDR_DISPLAY_SSD1306 0x3C
    // --- 2.1.3. I2C MODULE: DEVICE ENERGY MONITOR - INA219 ---
    #define BAT_DEVICE_MONITORING_TAG   "INA219_DEVICE_MONITOR_1"
    #define I2C_HW_ADDR_SENSOR_INA219_1 0x40
    // --- 2.1.4. I2C MODULE: CHARGING MONITOR - INA219 ---
    #define BAT_CHARGER_MONITORING_TAG  "INA219_CHARGER_MONITOR_2"
    #define I2C_HW_ADDR_SENSOR_INA219_2 0x41

    // - 3. UART PERIPHERALS (Sensors and GPS) ---
    // -- 3.1. UART 1 (Hardware) - TB600B H2S Sensor: ✅ TESTED
    #define SENSOR_H2S_UART_PORT        UART_NUM_0
    #define SENSOR_H2S_TAG              "TB600B_H2S"
    #define PIN_SENSOR_H2S_TX           10 // WHITE
    #define PIN_SENSOR_H2S_RX           9  // YELLOW
    // -- 3.2. UART 2 (Hardware) - TB600B SO2 Sensor: ✅ TESTED
    #define SENSOR_SO2_UART_PORT        UART_NUM_1
    #define SENSOR_SO2_TAG              "TB600B_SO2"
    #define PIN_SENSOR_SO2_TX           21 // WHITE
    #define PIN_SENSOR_SO2_RX           20 // YELLOW
    // -- 3.3. UART GPS Module uses Software Serial: UNTESTED
    #define PIN_SENSOR_GPS_TX           42
    #define PIN_SENSOR_GPS_RX           41

    // - 4. ANALOG (ADC) Inputs (Wind Speed and Sirection) ---
    #define PIN_SENSOR_ANEMOMETER_ADC   3  // ADC1_CH3: UNTESTED
    #define PIN_SENSOR_WIND_DIR_ADC     4  // ADC1_CH4: UNTESTED

    // - 5. ADDITIONAL PINS
    #define PIN_BTN_USER_KEY            0  // GPIO_0: USER KEY [UNTESTED]
    #define PIN_BTN_RST_GPIO            -1 // GPIO_1: RESET BUTTON
    #define PIN_LED_BUILTIN             8  // GPIO_35: UNTESTED

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #if defined(BOARD_DEVKITC_ESP32S3_V1)
        #define ANEMOMETER_ADC_PIN GPIO_NUM_3

        #define SO2_UART_PORT UART_NUM_0
        #define SO2_RX_PIN    GPIO_NUM_44
        #define SO2_TX_PIN    GPIO_NUM_43
        #define SO2_LOG_TAG   "TB600B_SO2"

        #define H2S_UART_PORT UART_NUM_1
        #define H2S_RX_PIN    GPIO_NUM_17
        #define H2S_TX_PIN    GPIO_NUM_18
        #define H2S_LOG_TAG   "TB600B_H2S"

    #elif defined(BOARD_HELTEC_WSL_ESP32S3_V3)
        // - 0. LoRa SX1262 PINS (FIXED by Heltec WSL v3) ---
        // These pins are hardwired to the onboard HELTEC LoRa module.
        #define PIN_LORA_NSS  8
        #define PIN_LORA_SCK  9
        #define PIN_LORA_MISO 10
        #define PIN_LORA_MOSI 11
        #define PIN_LORA_RST  12
        #define PIN_LORA_BUSY 13
        #define PIN_LORA_DIO1 14

        // - 1. SPI BUS (SD Card and W5500 Ethernet) ---
        // -- 1.1 Dedicated SPI bus (SPI3/VSPI) using accessible GPIO pins
        #define PIN_SPI3_SCK  3
        #define PIN_SPI3_MOSI 4
        #define PIN_SPI3_MISO 5
        // -- 1.2 Select Pin for SPI3 connected modules
        #define PIN_SD_CS     6
        #define PIN_W5500_CS  2

        // - 2. I2C BUS (RTC, OLED, INA219 x2) ---
        #define PIN_I2C_SCL   20
        #define PIN_I2C_SDA   21

        #define I2C_MASTER_SCL_IO PIN_I2C_SCL
        #define I2C_MASTER_SDA_IO PIN_I2C_SDA

        #define I2C_MASTER_NUM              I2C_NUM_0
        #define I2C_MASTER_FREQ_HZ          100000

        // -- 2.1. I2C ADDRESS for the modules ---
        // --- 2.1.1. I2C MODULE: REALTIME CLOCK - DS3231 ---
        #define RTC_TAG                     "DS3231_RTC"
        #define PIN_RTC_INT_SQW             19
        #define I2C_HW_ADDR_RTC_DS3231      0x68
        #define I2C_HW_ADDR_RTC_AT24C32     0x57
        // --- 2.1.2. I2C MODULE: DISPLAY - SSD1306 ---
        #define DISPLAY_TAG                 "SSD1306_DISPLAY"
        #define I2C_HW_ADDR_DISPLAY_SSD1306 0x3C
        // --- 2.1.3. I2C MODULE: DEVICE ENERGY MONITOR - INA219 ---
        #define BAT_DEVICE_MONITORING_TAG   "INA219_DEVICE_MONITOR_1"
        #define I2C_HW_ADDR_SENSOR_INA219_1 0x40
        // --- 2.1.4. I2C MODULE: CHARGING MONITOR - INA219 ---
        #define BAT_CHARGER_MONITORING_TAG  "INA219_CHARGER_MONITOR_2"
        #define I2C_HW_ADDR_SENSOR_INA219_2 0x41

        // - 3. UART PERIPHERALS (Sensors and GPS) ---
        // -- 3.1. UART 1 (Hardware) - TB600B H2S Sensor: ✅ TESTED
        #define SENSOR_H2S_UART_PORT        UART_NUM_1
        #define SENSOR_H2S_TAG              "TB600B_H2S"
        #define PIN_SENSOR_H2S_TX           46 // WHITE
        #define PIN_SENSOR_H2S_RX           45 // YELLOW
        // -- 3.2. UART 2 (Hardware) - TB600B SO2 Sensor: ✅ TESTED
        #define SENSOR_SO2_UART_PORT        UART_NUM_2
        #define SENSOR_SO2_TAG              "TB600B_SO2"
        #define PIN_SENSOR_SO2_TX           40 // WHITE
        #define PIN_SENSOR_SO2_RX           39 // YELLOW
        // -- 3.3. UART GPS Module uses Software Serial: UNTESTED
        #define PIN_SENSOR_GPS_TX           42
        #define PIN_SENSOR_GPS_RX           41

        // - 4. ANALOG (ADC) Inputs (Wind Speed and Sirection) ---
        #define PIN_SENSOR_ANEMOMETER_ADC   34 // ADC1_CH3: UNTESTED
        #define PIN_SENSOR_WIND_DIR_ADC     33 // ADC1_CH4: UNTESTED

        // - 5. ADDITIONAL PINS
        #define PIN_BTN_USER_KEY            0  // GPIO_0: USER KEY [UNTESTED]
        #define PIN_BTN_RST_GPIO            -1 // GPIO_1: RESET BUTTON
        #define PIN_LED_BUILTIN             35 // GPIO_35: UNTESTED

    #endif
#else
    #error "Unsupported IDF_TARGET! Please set the target using idf.py set-target."

#endif // CONFIG_IDF_TARGET

#define BAUD_RATE 9600

#endif // BOARD_PINS_H