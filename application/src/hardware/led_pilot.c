#include "sdkconfig.h"
#ifdef CONFIG_ENABLE_LED_PILOT_LAMP

#include "hardware/led_pilot.h"
#include "dev_public.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_FREQ_HZ        5000
#define LEDC_RESOLUTION     LEDC_TIMER_8_BIT
#define MAX_DUTY            255

#define BREATHE_TIME_MS     2000
#define FAST_BLINK_MS       500
#define SLOW_BLINK_MS       1000

static led_pilot_mode_et g_led_mode = OFF_MODE;
static bool g_blink_state = false;
static uint8_t g_breath_up = 1;

static TaskHandle_t g_led_task_handle = NULL;

/**
 * @brief LED 控制任务
 */
static void led_task(void *arg)
{
    while (1) {
        switch (g_led_mode) {
            case OFF_MODE:
                gpio_set_level(CONFIG_LED_PILOT_LAMP_OUT_PIN, 0);
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;

            case BREATH_MODE: {
                uint32_t next = g_breath_up ? MAX_DUTY : 0;
                g_breath_up = !g_breath_up;
                ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, next, BREATHE_TIME_MS);
                ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
                vTaskDelay(pdMS_TO_TICKS(BREATHE_TIME_MS)); // 等待一次渐变完成
                break;
            }

            case FAST_BLINK_MODE:
            case SLOW_BLINK_MODE: {
                uint32_t delay_ms = (g_led_mode == FAST_BLINK_MODE) ? FAST_BLINK_MS : SLOW_BLINK_MS;
                g_blink_state = !g_blink_state;
                if (g_blink_state) {
                    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MAX_DUTY);
                } else {
                    gpio_set_level(CONFIG_LED_PILOT_LAMP_OUT_PIN, 0);
                    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
                }
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
                vTaskDelay(pdMS_TO_TICKS(delay_ms / 2));
                break;
            }
        }
    }
}

/**
 * @brief 设置指示灯模式
 */
void led_pilot_set_mode(led_pilot_mode_et mode)
{
    if (mode == g_led_mode) return;
    g_led_mode = mode;
}

/**
 * @brief 指示灯初始化
 */
void led_pilot_init(void)
{
    static const ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_RESOLUTION,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQ_HZ,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    if (ledc_timer_config(&ledc_timer) != ESP_OK) return;

    static const ledc_channel_config_t ledc_channel_cfg = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_FADE_END,
        .gpio_num       = CONFIG_LED_PILOT_LAMP_OUT_PIN,
        .duty           = 0,
        .hpoint         = 0,
        .flags.output_invert = 0,
        .deconfigure    = false
    };
    if (ledc_channel_config(&ledc_channel_cfg) != ESP_OK) return;

    if (ledc_fade_func_install(0) != ESP_OK) return;

    if (g_led_task_handle == NULL) {
        xTaskCreate(led_task, "pilot", 512, NULL, 5, &g_led_task_handle);
    }

    led_pilot_set_mode(BREATH_MODE);
}

#endif
