#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include "sdkconfig.h"
#include "hal/gpio_hal.h"

// #define BOARD_DEVKITC_ESP32S3_V1
// #define BOARD_HELTEC_WSL_ESP32S3_V3

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
    #define ANEMOMETER_ADC_PIN GPIO_NUM_3 // Need to be fix. I dont remember the pin

    #define H2S_UART_PORT UART_NUM_0
    #define H2S_RX_PIN GPIO_NUM_9
    #define H2S_TX_PIN GPIO_NUM_10
    #define H2S_LOG_TAG "TB600B_H2S"

    #define SO2_UART_PORT UART_NUM_1
    #define SO2_RX_PIN GPIO_NUM_20
    #define SO2_TX_PIN GPIO_NUM_21
    #define SO2_LOG_TAG "TB600B_SO2"

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    // Not tested
    #if defined(BOARD_DEVKITC_ESP32S3_V1)
        #define ANEMOMETER_ADC_PIN GPIO_NUM_3

        #define H2S_UART_PORT UART_NUM_0
        #define H2S_RX_PIN GPIO_NUM_9
        #define H2S_TX_PIN GPIO_NUM_10
        #define H2S_LOG_TAG "TB600B_H2S"

        #define SO2_UART_PORT UART_NUM_1
        #define SO2_RX_PIN GPIO_NUM_20
        #define SO2_TX_PIN GPIO_NUM_21
        #define SO2_LOG_TAG "TB600B_SO2"

    // Not Tested
    #elif defined(BOARD_HELTEC_WSL_ESP32S3_V3)
        #define ANEMOMETER_ADC_PIN GPIO_NUM_3

        #define H2S_UART_PORT UART_NUM_0
        #define H2S_RX_PIN GPIO_NUM_9
        #define H2S_TX_PIN GPIO_NUM_10
        #define H2S_LOG_TAG "TB600B_H2S"

        #define SO2_UART_PORT UART_NUM_1
        #define SO2_RX_PIN GPIO_NUM_20
        #define SO2_TX_PIN GPIO_NUM_21
        #define SO2_LOG_TAG "TB600B_SO2"
    #endif

#else
    #error "Unsupported IDF_TARGET! Please set the target using idf.py set-target."

#endif // CONFIG_IDF_TARGET

#define BAUD_RATE 9600

#endif // BOARD_PINS_H