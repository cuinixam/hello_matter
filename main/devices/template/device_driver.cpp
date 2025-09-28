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

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::endpoint;


static const char *TAG = "XYZ_driver";
uint16_t sensor_endpoint_id = 0;

#if STATUS_LED_ENABLED
//static StatusLed *status_led  = new StatusLed();
static StatusLed *status_led  = NULL;
#endif

typedef struct {
    StatusLed *status_led;
} device_driver_xyz_t;
device_driver_xyz_t device_driver_xyz = { status_led, analog_led_lamp };


esp_err_t device_driver_attribute_update(device_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == sensor_endpoint_id) {
        // TODO: implement me
    }
    return err;
}

/** Set defaults for xyz driver
 *
 * Set the attribute drivers to their default values from the created data model.
 *
 * @param[in] endpoint_id Endpoint ID of the driver.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
static esp_err_t device_driver_xyz_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    // TODO: implement me
    return err;
}

esp_err_t device_driver_init(esp_matter::node_t *node)
{
    esp_err_t err = ESP_OK;
    // TODO: implement me
    return err;
}

esp_err_t device_driver_set_defaults()
{
    esp_err_t err = ESP_OK;
    err = device_driver_xyz_set_defaults(sensor_endpoint_id);
    return err;
}
