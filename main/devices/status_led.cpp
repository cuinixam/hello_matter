#include "status_led.h"
#include "bsp/esp-bsp.h"

#define RGB_TO_INT(color) ((color.r << 16) | (color.g << 8) | color.b)

StatusLed::StatusLed() {
    powerState = led_param::LED_POWER_OFF;
    color = LED_COLOR_BLACK;
    setColor(color);
    setPower(powerState);
    led_indicator_handle_t someLedHandle;
    ESP_ERROR_CHECK(bsp_led_indicator_create(&someLedHandle, NULL, 1));
    ledHandle = someLedHandle;
}

StatusLed::~StatusLed() {
    setPower(led_param::LED_POWER_OFF);
}

esp_err_t StatusLed::setPower(led_param::led_power_state_t power_state) {
    esp_err_t err = ESP_OK;
    this->powerState = power_state;
    if (power_state == led_param::LED_POWER_ON) {
        err = led_indicator_start(ledHandle, BSP_LED_ON);
    } else {
        err = led_indicator_start(ledHandle, BSP_LED_OFF);
    }
    return err;
}

esp_err_t StatusLed::setColor(rgb_t color) {
    this->color = color;
    return ESP_OK;
}

esp_err_t StatusLed::setStatus(led_status_t status) {
    this->status = status;
    esp_err_t err = ESP_OK;
    switch (this->status) {
        case INFO:
            err = setColor(LED_COLOR_BLUE);
            break;
        case WARNING:
            err = setColor(LED_COLOR_YELLOW);
            break;
        case ERROR:
            err = setColor(LED_COLOR_RED);
            break;
        case SUCCESS:
            err = setColor(LED_COLOR_GREEN);
            break;
        default:
            err = setColor(LED_COLOR_BLACK);
            break;
    }
    return err;
}
