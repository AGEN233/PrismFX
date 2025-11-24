#ifdef CONFIG_ENABLE_LED_PILOT_LAMP
#ifndef __LED_PILOT_H__
#define __LED_PILOT_H__

typedef enum {
    OFF_MODE,
    BREATH_MODE,
    FAST_BLINK_MODE,
    SLOW_BLINK_MODE,
}led_pilot_mode_et;

void led_pilot_set_mode(led_pilot_mode_et mode);
void led_pilot_init(void);
#endif
#endif