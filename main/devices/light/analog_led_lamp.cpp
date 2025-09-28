#include "analog_led_lamp.h"
#include "led_param.h"
#include <esp_log.h>

#define TAG "AnalogLedLamp"

AnalogLedLamp::AnalogLedLamp()
{
    powerState = led_param::LED_POWER_OFF;
    brightness = 0;
    temperature = 0;
    error_state = configureIndicators();
}

AnalogLedLamp::~AnalogLedLamp()
{

}

esp_err_t AnalogLedLamp::getErrorState()
{
    return error_state;
}

esp_err_t AnalogLedLamp::configureIndicators()
{
    esp_err_t err = ESP_OK;
    led_indicator_ledc_config_t ledc_config_white = {
        .is_active_level_high = 1,
        .timer_inited = false,
        .timer_num = LEDC_TIMER_0,
        .gpio_num = GPIO_NUM_10,
        .channel = LEDC_CHANNEL_0,
    };
    const led_indicator_config_t white_led_config = {
        .mode = LED_LEDC_MODE,
        .led_indicator_ledc_config = &ledc_config_white,
        .blink_lists = led_mode_config,
        .blink_list_num = led_param::LED_STATE_MAX,
    };
    white_led_handle = led_indicator_create(&white_led_config);
    if (white_led_handle == NULL) {
        ESP_LOGE(TAG, "Failed to create white LED indicator");
        err = ESP_FAIL;
    }
    if (err == ESP_OK) {
        led_indicator_ledc_config_t ledc_config_yellow = {
            .is_active_level_high = 1,
                .timer_inited = false,
                .timer_num = LEDC_TIMER_0,
                .gpio_num = GPIO_NUM_11,
                .channel = LEDC_CHANNEL_1,
        };
        const led_indicator_config_t yellow_led_config = {
            .mode = LED_LEDC_MODE,
            .led_indicator_ledc_config = &ledc_config_yellow,
            .blink_lists = led_mode_config,
            .blink_list_num = led_param::LED_STATE_MAX,
        };
        yellow_led_handle = led_indicator_create(&yellow_led_config);
        if (yellow_led_handle == NULL) {
            ESP_LOGE(TAG, "Failed to create yellow LED indicator");
            err = ESP_FAIL;
        }
    }

    return err;
}

esp_err_t AnalogLedLamp::setPower(led_param::led_power_state_t power_state)
{
    esp_err_t err = ESP_OK;
    this->powerState = power_state;
    if (power_state == led_param::LED_POWER_ON) {
        led_indicator_start(white_led_handle, LED_STATE_ON);
        led_indicator_start(yellow_led_handle, LED_STATE_ON);
        setBrightness(brightness);
    } else {
        led_indicator_start(white_led_handle, LED_STATE_OFF);
        led_indicator_start(yellow_led_handle, LED_STATE_OFF);
    }
    return err;
}

esp_err_t AnalogLedLamp::setBrightness(uint8_t brightness)
{
    esp_err_t err = ESP_OK;
    this->brightness = brightness;
    led_indicator_set_brightness(white_led_handle, brightness);
    led_indicator_set_brightness(yellow_led_handle, brightness);
    return ESP_OK;
}