#include "button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "BUTTON"

static gpio_num_t g_gpio_pin;
static uint16_t g_long_press_time_ms = 0;
static uint16_t g_double_press_times_ms = 0;

static button_cb_t single_cb = NULL;
static button_cb_t double_cb = NULL;
static button_cb_t long_cb = NULL;

/**
 * @brief 按钮单击回调注册
 * @param cb
 */
void button_register_single_cb(button_cb_t callback)
{
    single_cb = callback;
}

/**
 * @brief 按钮双击回调注册
 * @param cb
 */
void button_register_double_cb(button_cb_t callback)
{
    double_cb = callback;
}

/**
 * @brief 按钮长按回调注册
 * @param cb
 */
void button_register_long_cb(button_cb_t callback)
{
    long_cb = callback;
}

/**
 * @brief 按钮线程
 */
void button_task(void *arg)
{
    bool last_level = 1;
    bool pressing = false;

    uint32_t press_tick = 0;
    uint32_t release_tick = 0;

    uint8_t click_count = 0;
    bool long_triggered = false;

    while (1) {
        bool level = gpio_get_level(g_gpio_pin);
        uint32_t now = xTaskGetTickCount();

        // 按下沿(上次1, 当前0)
        if (last_level == 1 && level == 0) {
            pressing = true;
            press_tick = now;
            long_triggered = false;
        }

        // 松开沿(上次0, 当前1)
        if (last_level == 0 && level == 1) {

            if (!long_triggered) {
                click_count++;
                release_tick = now;
            }
            pressing = false;
        }

        // 长按判定 -> 当前按下未松手且没有长按触发
        if (pressing && !long_triggered) {
            if ((now - press_tick) * portTICK_PERIOD_MS >= g_long_press_time_ms) {
                long_triggered = true; // 长按触发
                click_count = 0;
                if (long_cb) {
                    long_cb();
                }
            }
        }

        // 单击和双击判定 -> 未按住且未产生技术, 认为一次单击或双击
        if (!pressing && click_count > 0) {
            if ((now - release_tick) * portTICK_PERIOD_MS >= g_double_press_times_ms) {
                // 单击判定 -> 在判定时间内只产生一次按下
                if (click_count == 1) {
                    if (single_cb) {
                        single_cb();
                    }
                } else if (click_count >= 2) {
                    // 双击判定 -> 在判定时间内产生多次按下
                    if (double_cb) {
                        double_cb();
                    }
                }
                click_count = 0;
            }
        }
        last_level = level;
        vTaskDelay(10);
    }
}

/**
 * @brief 按钮初始化
 * @param gpio_pin              GPIO引脚
 * @param long_press_time_ms    长按判定时间
 * @param double_press_times_ms 双击判定时间
 */
void button_init(gpio_num_t gpio_pin, uint16_t long_press_time_ms, uint16_t double_press_times_ms)
{
    g_gpio_pin = gpio_pin;
    g_long_press_time_ms = long_press_time_ms;
    g_double_press_times_ms = double_press_times_ms;

    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << gpio_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_conf);

    BaseType_t ret = xTaskCreate(button_task, "button_task", 1024, NULL, 1, NULL);
    assert(ret == pdPASS);
}