// data_format.h

#ifndef DATA_FORMAT_H
#define DATA_FORMAT_H

#include "anemometer.h"
#include "tb600b_v2.h"

// Prototype for the JSON print function
void print_sensor_data_json(tb600b_handle_t *handle);
void print_all_data_json(anemometer_handle_t *wind_handle, tb600b_handle_t *h2s_handle, tb600b_handle_t *so2_handle);

#endif // DATA_FORMAT_H