#ifndef LED_PARAM_PRIV_H
#define LED_PARAM_PRIV_H

#include "led_indicator.h"

namespace led_param {

    typedef enum {
        LED_POWER_OFF = 0,
        LED_POWER_ON = 1,
    } led_power_state_t;

    enum {
        LED_STATE_ON = 0,
        LED_STATE_OFF,
        LED_STATE_MAX,
    };

} // namespace led_param

extern blink_step_t const *led_mode_config[];

#endif // LED_PARAM_PRIV_H
