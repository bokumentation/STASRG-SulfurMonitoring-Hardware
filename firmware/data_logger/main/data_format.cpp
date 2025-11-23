// data_format.cpp (New functions)

#include "data_format.h"
#include "cJSON.h"
#include "esp_timer.h"

#include <stdio.h>
#include <stdlib.h>

#define JSON_MARKER "GLOBAL_DATA:"

// --- Helper: Creates a JSON object for the Anemometer ---
static cJSON *create_anemometer_json(anemometer_handle_t *handle)
{
    if (handle == nullptr)
        return NULL;

    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL)
        return NULL;

    cJSON_AddStringToObject(obj, "id", "ANEMOMETER");
    cJSON_AddStringToObject(obj, "type", "WIND_SPEED");
    cJSON_AddNumberToObject(obj, "speed_kmph", (double)anemometer_get_speed_kmph(handle));
    cJSON_AddNumberToObject(obj, "speed_mps", (double)anemometer_get_speed_mps(handle));

    return obj;
}

// --- Helper: Creates a JSON object for a TB600B Sensor ---
static cJSON *create_tb600b_json(tb600b_handle_t *handle)
{
    if (handle == nullptr)
        return NULL;

    const char *sensor_name = (handle->sensor_type == TB600B_SENSOR_TYPE_H2S) ? "H2S" : "SO2";

    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL)
        return NULL;

    cJSON_AddStringToObject(obj, "id", handle->tag);
    cJSON_AddStringToObject(obj, "type", sensor_name);

    // Add gas and environmental data
    cJSON_AddNumberToObject(obj, "gas_ug", (double)tb600b_get_gas_ug(handle));
    cJSON_AddNumberToObject(obj, "temp_c", (double)tb600b_get_temperature(handle));
    cJSON_AddNumberToObject(obj, "hum_pct", (double)tb600b_get_humidity(handle));

    return obj;
}

// --- Main Combined Print Function ---
void print_all_data_json(anemometer_handle_t *wind_handle, tb600b_handle_t *h2s_handle, tb600b_handle_t *so2_handle)
{

    // 1. Create a root object that will contain the array and a timestamp
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
        return;

    cJSON_AddNumberToObject(root, "timestamp_ms", (double)esp_timer_get_time() / 1000.0);

    // 2. Create the array that holds all sensor data
    cJSON *data_array = cJSON_CreateArray();
    if (data_array == NULL) {
        cJSON_Delete(root);
        return;
    }

    cJSON_AddItemToObject(root, "sensors", data_array);

    // 3. Populate the array with individual sensor objects
    cJSON_AddItemToArray(data_array, create_anemometer_json(wind_handle));
    cJSON_AddItemToArray(data_array, create_tb600b_json(h2s_handle));
    cJSON_AddItemToArray(data_array, create_tb600b_json(so2_handle));

    // 4. Convert to string and print
    char *json_string = cJSON_PrintUnformatted(root);

    if (json_string != NULL) {
        printf("%s%s\n", JSON_MARKER, json_string);
        free(json_string);
    }

    cJSON_Delete(root);
}

// NOTE: The old print_sensor_data_json is no longer needed,
// but can be kept for backward compatibility or testing.