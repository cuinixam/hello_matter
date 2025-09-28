#ifndef ANALOG_LED_LAMP_H
#define ANALOG_LED_LAMP_H

#include <esp_err.h>
#include "led_indicator.h"
#include "led_param.h"

class AnalogLedLamp {
public:
    AnalogLedLamp();
    ~AnalogLedLamp();

    esp_err_t setPower(led_param::led_power_state_t power_state);
    esp_err_t setBrightness(uint8_t brightness);
    esp_err_t setTemperature(uint32_t temperature);
    esp_err_t getErrorState();

private:
    led_param::led_power_state_t powerState;
    uint8_t brightness;
    uint32_t temperature;

    led_indicator_handle_t white_led_handle;
    led_indicator_handle_t yellow_led_handle;

    esp_err_t error_state;

    esp_err_t configureIndicators();
};

#endif // ANALOG_LED_LAMP_H
