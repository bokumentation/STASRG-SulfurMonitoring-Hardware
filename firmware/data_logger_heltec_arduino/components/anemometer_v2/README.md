# How to use this library
### 1. Include this library
Edit your `main/cmakelists.txt`.

```cmake
idf_component_register(
    SRCS "main.cpp"
    PRIV_REQUIRES tb600b_cxx # Add this line
    INCLUDE_DIRS "." "include"
)
```

### 2. Create variables
Example:
```cpp
// Tag for the TB600B Sensor
static const char *SO2_UART_TAG = "[SO2]";
static const char *H2S_UART_TAG = "[H2S]";

// Global variable to store value of SO2
float g_so2_current_temperature = 0.0;
float g_so2_current_humidity = 0.0;
float g_so2_current_gas_ug = 0.0;

// Global variable to store value H2S
float g_h2s_current_temperature = 0.0;
float g_h2s_current_humidity = 0.0;
float g_h2s_current_gas_ug = 0.0;
```

### 3. Call in the `app_main()` function.
Pass the argument by using `reference` or `&your_Variable` to call the global function.
```cpp
extern "C" void app_main(void) {
  tb600b_init_uart((uart_port_t)SO2_UART_PORT, SO2_UART_TX_PIN, SO2_UART_RX_PIN, UART_BAUD_RATE, SO2_UART_TAG);
  tb600b_init_uart((uart_port_t)H2S_UART_PORT, H2S_UART_TX_PIN, H2S_UART_RX_PIN, UART_BAUD_RATE, H2S_UART_TAG);

  vTaskDelay(pdMS_TO_TICKS(100));

  while (1) {
    tb600b_get_combined_data((uart_port_t)SO2_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA), SO2_UART_TAG,
                             &g_so2_current_temperature, &g_so2_current_humidity, &g_so2_current_gas_ug);
    ESP_LOGI(SO2_UART_TAG, "Temperature: %.2f %%", g_so2_current_temperature);
    ESP_LOGI(SO2_UART_TAG, "Humidity: %.2f %%", g_so2_current_humidity);
    ESP_LOGI(SO2_UART_TAG, "Gas: %.2f %%", g_so2_current_gas_ug);
    vTaskDelay(pdMS_TO_TICKS(100));

    tb600b_get_combined_data((uart_port_t)H2S_UART_PORT, CMD_GET_COMBINED_DATA, sizeof(CMD_GET_COMBINED_DATA), H2S_UART_TAG,
                             &g_h2s_current_temperature, &g_h2s_current_humidity, &g_h2s_current_gas_ug);
    ESP_LOGI(H2S_UART_TAG, "Temperature: %.2f %%", g_h2s_current_temperature);
    ESP_LOGI(H2S_UART_TAG, "Humidity: %.2f %%", g_h2s_current_humidity);
    ESP_LOGI(H2S_UART_TAG, "Gas: %.2f %%", g_h2s_current_gas_ug);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
```

### 4. Build flash and Monitor

---
Pin definiton are in the `tb600b_cxx/uart_user_config.h`. 

```cpp
#define SO2_UART_PORT 1
#define SO2_UART_RX_PIN 16
#define SO2_UART_TX_PIN 17

#define H2S_UART_PORT 2
#define H2S_UART_RX_PIN 26
#define H2S_UART_TX_PIN 27
```