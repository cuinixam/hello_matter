#include "led_param.h"

static const blink_step_t led_state_on[] = {
    {LED_BLINK_HOLD, led_param::LED_STATE_ON, 0},
    {LED_BLINK_STOP, 0, 0},
};

static const blink_step_t led_state_off[] = {
    {LED_BLINK_HOLD, led_param::LED_STATE_OFF, 0},
    {LED_BLINK_STOP, 0, 0},
};

blink_step_t const *led_mode_config[] = {
    [led_param::LED_STATE_ON] = led_state_on,
    [led_param::LED_STATE_OFF] = led_state_off,
    [led_param::LED_STATE_MAX] = NULL,
};
