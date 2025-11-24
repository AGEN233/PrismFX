#include "sdkconfig.h"
#ifdef CONFIG_ENABLE_LED_PILOT_LAMP

#include "led_pilot.h"
#include "dev_public.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_FREQ_HZ        5000
#define LEDC_RESOLUTION     LEDC_TIMER_8_BIT
#define MAX_DUTY            255

#define BREATHE_TIME_MS     2500
#define FAST_BLINK_MS       500
#define SLOW_BLINK_MS       1000

static led_pilot_mode_et g_led_mode = OFF_MODE;
static TimerHandle_t g_blink_timer = NULL;
static bool g_blink_state = false;
static uint8_t g_breath_up = 1;
static ledc_cbs_t g_ledc_fade_cb = {0};

static bool IRAM_ATTR led_breath_mode_cb(const ledc_cb_param_t *param, void *arg)
{
    if (g_led_mode != BREATH_MODE) return false;
    uint32_t next = g_breath_up ? MAX_DUTY : 0;
    g_breath_up = !g_breath_up;
    ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, next, BREATHE_TIME_MS);
    ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
    return false;
}

static void led_blink_timer_callback(TimerHandle_t xTimer)
{
    if (g_led_mode != FAST_BLINK_MODE && g_led_mode != SLOW_BLINK_MODE)
        return;

    g_blink_state = !g_blink_state;

    if (g_blink_state) {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, MAX_DUTY);
    } else {
        gpio_set_level(CONFIG_LED_PILOT_LAMP_OUT_PIN, 0); // 直接拉低GPIO
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    }
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void led_pilot_set_mode(led_pilot_mode_et mode)
{
    if (mode == g_led_mode) return;
    g_led_mode = mode;
    if (g_blink_timer) xTimerStop(g_blink_timer, 0);

    switch (mode) {
        case OFF_MODE:
            gpio_set_level(CONFIG_LED_PILOT_LAMP_OUT_PIN, 0);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            break;
        case BREATH_MODE:
            g_breath_up = 1;
            ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, MAX_DUTY, BREATHE_TIME_MS);
            ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
            break;
        case FAST_BLINK_MODE:
            g_blink_state = false;
            gpio_set_level(CONFIG_LED_PILOT_LAMP_OUT_PIN, 0);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            xTimerChangePeriod(g_blink_timer, pdMS_TO_TICKS(FAST_BLINK_MS / 2), 0);
            xTimerStart(g_blink_timer, 0);
            break;
        case SLOW_BLINK_MODE:
            g_blink_state = false;
            gpio_set_level(CONFIG_LED_PILOT_LAMP_OUT_PIN, 0);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            xTimerChangePeriod(g_blink_timer, pdMS_TO_TICKS(SLOW_BLINK_MS / 2), 0);
            xTimerStart(g_blink_timer, 0);
            break;
    }
}

void led_pilot_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_RESOLUTION,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQ_HZ,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    if (ledc_timer_config(&ledc_timer) != ESP_OK) return;

    ledc_channel_config_t ledc_channel_cfg = {
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

    g_ledc_fade_cb.fade_cb = led_breath_mode_cb;
    if (ledc_cb_register(LEDC_MODE, LEDC_CHANNEL, &g_ledc_fade_cb, NULL) != ESP_OK) return;

    if (g_blink_timer == NULL)
        g_blink_timer = xTimerCreate("led_blink", pdMS_TO_TICKS(FAST_BLINK_MS / 2), pdTRUE, NULL, led_blink_timer_callback);

    led_pilot_set_mode(BREATH_MODE);
}

#endif
