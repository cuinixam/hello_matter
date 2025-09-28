/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_err.h>
#include <esp_log.h>

#include <esp_matter.h>
#include "bsp/esp-bsp.h"

#include <common_macros.h>
#include <app_reset.h>

#include "device_driver.h"
#include "shtc3.h"

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;


static const char *TAG = "temp_driver";
uint16_t temperature_sensor_endpoint_id = 0;
uint16_t humidity_sensor_endpoint_id = 0;


#if STATUS_LED_ENABLED
//static StatusLed *status_led  = new StatusLed();
static StatusLed *status_led  = NULL;
#endif


// Application cluster specification, 7.18.2.11. Temperature
// represents a temperature on the Celsius scale with a resolution of 0.01°C.
// temp = (temperature in °C) x 100
static void temp_sensor_notification(uint16_t endpoint_id, float temp, void *user_data)
{
    // schedule the attribute update so that we can report it from matter thread
    chip::DeviceLayer::SystemLayer().ScheduleLambda([endpoint_id, temp]() {
        attribute_t * attribute = attribute::get(endpoint_id,
                                                 TemperatureMeasurement::Id,
                                                 TemperatureMeasurement::Attributes::MeasuredValue::Id);

        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        attribute::get_val(attribute, &val);
        val.val.i16 = static_cast<int16_t>(temp * 100);

        attribute::update(endpoint_id, TemperatureMeasurement::Id, TemperatureMeasurement::Attributes::MeasuredValue::Id, &val);
    });
}

// Application cluster specification, 2.6.4.1. MeasuredValue Attribute
// represents the humidity in percent.
// humidity = (humidity in %) x 100
static void humidity_sensor_notification(uint16_t endpoint_id, float humidity, void *user_data)
{
    // schedule the attribute update so that we can report it from matter thread
    chip::DeviceLayer::SystemLayer().ScheduleLambda([endpoint_id, humidity]() {
        attribute_t * attribute = attribute::get(endpoint_id,
                                                 RelativeHumidityMeasurement::Id,
                                                 RelativeHumidityMeasurement::Attributes::MeasuredValue::Id);

        esp_matter_attr_val_t val = esp_matter_invalid(NULL);
        attribute::get_val(attribute, &val);
        val.val.u16 = static_cast<uint16_t>(humidity * 100);

        attribute::update(endpoint_id, RelativeHumidityMeasurement::Id, RelativeHumidityMeasurement::Attributes::MeasuredValue::Id, &val);
    });
}

static esp_err_t temperature_sensor_driver_create(node_t *node)
{
    esp_err_t err = ESP_OK;
    // add temperature sensor device
    temperature_sensor::config_t temp_sensor_config;
    endpoint_t * temp_sensor_ep = temperature_sensor::create(node, &temp_sensor_config, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(temp_sensor_ep != nullptr, ESP_LOGE(TAG, "Failed to create temperature_sensor endpoint"));
    temperature_sensor_endpoint_id = endpoint::get_id(temp_sensor_ep);
    ESP_LOGI(TAG, "Temperature sensor created with endpoint_id %d", temperature_sensor_endpoint_id);

    // add the humidity sensor device
    humidity_sensor::config_t humidity_sensor_config;
    endpoint_t * humidity_sensor_ep = humidity_sensor::create(node, &humidity_sensor_config, ENDPOINT_FLAG_NONE, NULL);
    ABORT_APP_ON_FAILURE(humidity_sensor_ep != nullptr, ESP_LOGE(TAG, "Failed to create humidity_sensor endpoint"));
    humidity_sensor_endpoint_id = endpoint::get_id(humidity_sensor_ep);
    ESP_LOGI(TAG, "Humidity sensor created with endpoint_id %d", humidity_sensor_endpoint_id);

    // initialize temperature and humidity sensor driver (shtc3)
    static shtc3_sensor_config_t shtc3_config = {
        .temperature = {
            .cb = temp_sensor_notification,
            .endpoint_id = temperature_sensor_endpoint_id,
        },
        .humidity = {
            .cb = humidity_sensor_notification,
            .endpoint_id = humidity_sensor_endpoint_id,
        },
    };
    err = shtc3_sensor_init(&shtc3_config);
    return err;
}

/** Initialize the button driver
 *
 * This initializes the button driver associated with the selected board.
 *
 * @return Handle on success.
 * @return NULL in case of failure.
 */
static device_driver_handle_t device_driver_button_init()
{
if (CONFIG_BSP_BUTTONS_NUM > 0) {
    button_handle_t buttonHandle;
    ESP_ERROR_CHECK(bsp_iot_button_create(&buttonHandle, NULL, 1));
    return (device_driver_handle_t)buttonHandle;
} else {
    return NULL;
}
}

esp_err_t device_driver_attribute_update(device_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    // Since this is just a sensor and we don't expect any writes on our temperature sensor, return success.
    return ESP_OK;
}

/** Set defaults for sensor driver
 *
 * Set the attribute drivers to their default values from the created data model.
 *
 * @param[in] endpoint_id Endpoint ID of the driver.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
static esp_err_t temperature_sensor_set_defaults()
{
    esp_err_t err = ESP_OK;
    // TODO: implement me
    return err;
}

esp_err_t device_driver_init(esp_matter::node_t *node)
{
    esp_err_t err = ESP_OK;
    err = temperature_sensor_driver_create(node);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to initialize temperature sensor driver"));
    device_driver_handle_t button_handle = device_driver_button_init();
    ABORT_APP_ON_FAILURE(button_handle != nullptr, ESP_LOGE(TAG, "Failed to create button handle"));
    app_reset_button_register(button_handle);
    return err;
}

esp_err_t device_driver_set_defaults()
{
    return temperature_sensor_set_defaults();
}
