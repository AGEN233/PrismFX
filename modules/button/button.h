#ifndef __BUTTON_H__
#define __BUTTON_H__
#include "driver/gpio.h"
typedef void (*button_cb_t)(void);

typedef enum {
    BUTTON_IDLE = 0,          // 空闲，未按
    BUTTON_PRESS_DEBOUNCE,    // 按下消抖
    BUTTON_PRESS_HOLD,        // 按住中（判断长按）
    BUTTON_RELEASE_DEBOUNCE,  // 松开消抖
    BUTTON_WAIT_DOUBLE        // 等待第二击
} button_state_t;

void button_register_single_cb(button_cb_t callback);
void button_register_double_cb(button_cb_t callback);
void button_register_long_cb(button_cb_t callback);

void button_init(gpio_num_t gpio_pin, uint16_t long_press_time_ms, uint16_t double_press_times_ms);

#endif