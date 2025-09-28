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
#include "led_param.h"
#include "status_led.h"
#include "analog_led_lamp.h"

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::endpoint;


/** Standard max values (used for remapping attributes) */
#define STANDARD_BRIGHTNESS 255
#define STANDARD_HUE 360
#define STANDARD_SATURATION 255
#define STANDARD_TEMPERATURE_FACTOR 1000000

/** Matter max values (used for remapping attributes) */
#define MATTER_BRIGHTNESS 254
#define MATTER_HUE 254
#define MATTER_SATURATION 254
#define MATTER_TEMPERATURE_FACTOR 1000000

/** Default attribute values used during initialization */
#define DEFAULT_POWER true
#define DEFAULT_BRIGHTNESS 64
#define DEFAULT_HUE 128
#define DEFAULT_SATURATION 254


static const char *TAG = "light_driver";
uint16_t light_endpoint_id = 0;

#if STATUS_LED_ENABLED
//static StatusLed *status_led  = new StatusLed();
static StatusLed *status_led  = NULL;
#endif
static AnalogLedLamp *analog_led_lamp = new AnalogLedLamp();

typedef struct {
    StatusLed *status_led;
    AnalogLedLamp *analog_led_lamp;
} device_driver_light_t;
device_driver_light_t device_driver_light = { status_led, analog_led_lamp };

static esp_err_t device_driver_light_set_power(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (val->val.b) {
        err = device_driver_light.analog_led_lamp->setPower(led_param::LED_POWER_ON);
    } else {
        err = device_driver_light.analog_led_lamp->setPower(led_param::LED_POWER_OFF);
    }
    return err;
}

static esp_err_t device_driver_light_set_brightness(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    uint8_t value = REMAP_TO_RANGE(val->val.u8, MATTER_BRIGHTNESS, STANDARD_BRIGHTNESS);
    ESP_LOGI(TAG, "LED set brightness: %d", value);
    return device_driver_light.analog_led_lamp->setBrightness(value);
}

static esp_err_t device_driver_light_set_hue(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_HUE, STANDARD_HUE);
#if 0
    led_indicator_ihsv_t hsv;
    hsv.value = led_indicator_get_hsv(handle);
    hsv.h = value;
    return led_indicator_set_hsv(handle, hsv.value);
#else
    ESP_LOGI(TAG, "LED set hue: %d", value);
    return ESP_OK;
#endif
}

static esp_err_t device_driver_light_set_saturation(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    int value = REMAP_TO_RANGE(val->val.u8, MATTER_SATURATION, STANDARD_SATURATION);
#if 0
    led_indicator_ihsv_t hsv;
    hsv.value = led_indicator_get_hsv(handle);
    hsv.s = value;
    return led_indicator_set_hsv(handle, hsv.value);
#else
    ESP_LOGI(TAG, "LED set saturation: %d", value);
    return ESP_OK;
#endif
}

static esp_err_t device_driver_light_set_temperature(led_indicator_handle_t handle, esp_matter_attr_val_t *val)
{
    uint32_t value = REMAP_TO_RANGE_INVERSE(val->val.u16, STANDARD_TEMPERATURE_FACTOR);
    ESP_LOGI(TAG, "LED set temperature: %d", (int)value);
    return ESP_OK;
}

static void device_driver_button_toggle_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Toggle button pressed");
    uint16_t endpoint_id = light_endpoint_id;
    uint32_t cluster_id = OnOff::Id;
    uint32_t attribute_id = OnOff::Attributes::OnOff::Id;

    attribute_t *attribute = attribute::get(endpoint_id, cluster_id, attribute_id);

    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    attribute::get_val(attribute, &val);
    val.val.b = !val.val.b;
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

esp_err_t device_driver_attribute_update(device_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == light_endpoint_id) {
        led_indicator_handle_t handle = (led_indicator_handle_t)driver_handle;
        if (cluster_id == OnOff::Id) {
            if (attribute_id == OnOff::Attributes::OnOff::Id) {
                err = device_driver_light_set_power(handle, val);
            }
        } else if (cluster_id == LevelControl::Id) {
            if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                err = device_driver_light_set_brightness(handle, val);
            }
        } else if (cluster_id == ColorControl::Id) {
            if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
                err = device_driver_light_set_hue(handle, val);
            } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
                err = device_driver_light_set_saturation(handle, val);
            } else if (attribute_id == ColorControl::Attributes::ColorTemperatureMireds::Id) {
                err = device_driver_light_set_temperature(handle, val);
            }
        }
    }
    return err;
}

/** Set defaults for light driver
 *
 * Set the attribute drivers to their default values from the created data model.
 *
 * @param[in] endpoint_id Endpoint ID of the driver.
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t device_driver_light_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    led_indicator_handle_t handle = (led_indicator_handle_t)priv_data;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    /* Setting brightness */
    attribute_t *attribute = attribute::get(endpoint_id, LevelControl::Id, LevelControl::Attributes::CurrentLevel::Id);
    attribute::get_val(attribute, &val);
    err |= device_driver_light_set_brightness(handle, &val);

    /* Setting color */
    attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorMode::Id);
    attribute::get_val(attribute, &val);
    if (val.val.u8 == (uint8_t)ColorControl::ColorMode::kCurrentHueAndCurrentSaturation) {
        /* Setting hue */
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentHue::Id);
        attribute::get_val(attribute, &val);
        err |= device_driver_light_set_hue(handle, &val);
        /* Setting saturation */
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::CurrentSaturation::Id);
        attribute::get_val(attribute, &val);
        err |= device_driver_light_set_saturation(handle, &val);
    } else if (val.val.u8 == (uint8_t)ColorControl::ColorMode::kColorTemperature) {
        /* Setting temperature */
        attribute = attribute::get(endpoint_id, ColorControl::Id, ColorControl::Attributes::ColorTemperatureMireds::Id);
        attribute::get_val(attribute, &val);
        err |= device_driver_light_set_temperature(handle, &val);
    } else {
        ESP_LOGE(TAG, "Color mode not supported");
    }

    /* Setting power */
    attribute = attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);
    err |= device_driver_light_set_power(handle, &val);

    return err;
}

device_driver_handle_t device_driver_light_init()
{
    return (device_driver_handle_t)(&device_driver_light);
}

/** Initialize the button driver
 *
 * This initializes the button driver associated with the selected board.
 *
 * @return Handle on success.
 * @return NULL in case of failure.
 */
device_driver_handle_t device_driver_button_init()
{
if (CONFIG_BSP_BUTTONS_NUM > 0) {
    button_handle_t buttonHandle;
    ESP_ERROR_CHECK(bsp_iot_button_create(&buttonHandle, NULL, 1));
    ESP_ERROR_CHECK(iot_button_register_cb(buttonHandle, BUTTON_PRESS_DOWN, device_driver_button_toggle_cb, NULL));
    
    return (device_driver_handle_t)buttonHandle;
} else {
    return NULL;
}
}

static esp_err_t device_driver_light_create(node_t *node, device_driver_handle_t light_handle)
{
    extended_color_light::config_t light_config;
    light_config.on_off.on_off = DEFAULT_POWER;
    light_config.on_off.lighting.start_up_on_off = nullptr;
    light_config.level_control.current_level = DEFAULT_BRIGHTNESS;
    light_config.level_control.on_level = DEFAULT_BRIGHTNESS;
    light_config.level_control.lighting.start_up_current_level = DEFAULT_BRIGHTNESS;
    light_config.color_control.color_mode = (uint8_t)ColorControl::ColorMode::kColorTemperature;
    light_config.color_control.enhanced_color_mode = (uint8_t)ColorControl::ColorMode::kColorTemperature;
    light_config.color_control.color_temperature.startup_color_temperature_mireds = nullptr;

    // endpoint handles can be used to add/modify clusters.
    endpoint_t *endpoint = extended_color_light::create(node, &light_config, ENDPOINT_FLAG_NONE, light_handle);
    ABORT_APP_ON_FAILURE(endpoint != nullptr, ESP_LOGE(TAG, "Failed to create extended color light endpoint"));

    light_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Light created with endpoint_id %d", light_endpoint_id);

    /* Mark deferred persistence for some attributes that might be changed rapidly */
    cluster_t *level_control_cluster = cluster::get(endpoint, LevelControl::Id);
    attribute_t *current_level_attribute = attribute::get(level_control_cluster, LevelControl::Attributes::CurrentLevel::Id);
    attribute::set_deferred_persistence(current_level_attribute);

    cluster_t *color_control_cluster = cluster::get(endpoint, ColorControl::Id);
    attribute_t *current_x_attribute = attribute::get(color_control_cluster, ColorControl::Attributes::CurrentX::Id);
    attribute::set_deferred_persistence(current_x_attribute);
    attribute_t *current_y_attribute = attribute::get(color_control_cluster, ColorControl::Attributes::CurrentY::Id);
    attribute::set_deferred_persistence(current_y_attribute);
    attribute_t *color_temp_attribute = attribute::get(color_control_cluster, ColorControl::Attributes::ColorTemperatureMireds::Id);
    attribute::set_deferred_persistence(color_temp_attribute);

    return ESP_OK;
}

esp_err_t device_driver_init(esp_matter::node_t *node)
{
    esp_err_t err = ESP_OK;
    device_driver_handle_t light_handle = device_driver_light_init();
    ABORT_APP_ON_FAILURE(light_handle != nullptr, ESP_LOGE(TAG, "Failed to create light handle"));
    device_driver_light_create(node, light_handle);
    device_driver_handle_t button_handle = device_driver_button_init();
    ABORT_APP_ON_FAILURE(button_handle != nullptr, ESP_LOGE(TAG, "Failed to create button handle"));
    app_reset_button_register(button_handle);
    return err;
}

esp_err_t device_driver_set_defaults()
{
    esp_err_t err = ESP_OK;
    err = device_driver_light_set_defaults(light_endpoint_id);
    return err;
}
