#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <esp_matter.h>
#include <esp_err.h>
#include "led_param.h"
#include "led_indicator.h"

#define STATUS_LED_ENABLED (CONFIG_BSP_LEDS_NUM > 0)

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

#define LED_COLOR_BLACK {0, 0, 0}
#define LED_COLOR_WHITE {255, 255, 255}
#define LED_COLOR_RED {255, 0, 0}
#define LED_COLOR_GREEN {0, 255, 0}
#define LED_COLOR_YELLOW {255, 255, 0}
#define LED_COLOR_BLUE {0, 0, 255}

typedef enum {
    OFF = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    SUCCESS = 4
} led_status_t;


class StatusLed {
public:
    StatusLed();
    ~StatusLed();

    esp_err_t setPower(led_param::led_power_state_t power_state);
    esp_err_t setColor(rgb_t color);
    esp_err_t setStatus(led_status_t status);
private:
    led_param::led_power_state_t powerState;
    rgb_t color;
    led_status_t status;
    led_indicator_handle_t ledHandle;
};
#endif // STATUS_LED_H
