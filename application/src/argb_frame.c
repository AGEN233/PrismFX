#include "dev_public.h"
#include "argb_drv.h"

static argb_color_type_st g_argb_color_payload[CONFIG_LED_COUNT_MAX] = {0};
static argb_color_type_st argb_color_payload_tx_buf[CONFIG_LED_COUNT_MAX] = {0};
static argb_frame_br_fade_config_st g_argb_br_fade_config = {0};

static const uint8_t gamma_lut[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5,
    5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
    11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
    19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28,
    28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 35, 36, 37, 38, 39, 39,
    40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 73, 74, 75, 76, 77, 78, 79, 81, 82, 83, 84, 85, 87, 88,
    89, 90, 91, 93, 94, 95, 97, 98, 99, 100, 102, 103, 105, 106, 107,
    109, 110, 111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 127,
    129, 130, 132, 133, 135, 137, 138, 140, 141, 143, 145, 146, 148, 149,
    151, 153, 154, 156, 158, 159, 161, 163, 165, 166, 168, 170, 172, 173,
    175, 177, 179, 181, 182, 184, 186, 188, 190, 192, 194, 196, 197, 199,
    201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221, 223, 225, 227,
    229, 231, 234, 236, 238, 240, 242, 244, 246, 248, 251, 253, 255
};

static void inline argb_gamma_handle(argb_color_type_st *payload)
{
    payload->R = gamma_lut[payload->R];
    payload->G = gamma_lut[payload->G];
    payload->B = gamma_lut[payload->B];
}

static void inline argb_led_short_handle(argb_color_type_st *src, argb_color_type_st *dst)
{
    dst->R = src->G;
    dst->G = src->R;
    dst->B = src->B;
}

argb_frame_br_fade_config_st *argb_led_get_br_fade_config(void)
{
    return &g_argb_br_fade_config;
}

// k = (1 - cos(pi * 已渐变时间 / 渐变时间长度)) /2
void argb_led_br_fade_handle(void)
{
    if (!g_argb_br_fade_config.active) {
        return;
    }

    uint32_t now = prismFX_get_timems();
    uint32_t elapsed = now - g_argb_br_fade_config.start_ms;

    if (elapsed >= g_argb_br_fade_config.fade_time_ms) {
        g_argb_br_fade_config.current_br = g_argb_br_fade_config.target_br;
        g_argb_br_fade_config.active = false;
        return;
    }

    float progress = (float)elapsed / g_argb_br_fade_config.fade_time_ms;
    float k = (1.0f - cosf(M_PI * progress)) * 0.5f;

    int32_t diff = (int32_t)g_argb_br_fade_config.target_br - (int32_t)g_argb_br_fade_config.start_br;
    g_argb_br_fade_config.current_br = g_argb_br_fade_config.start_br + diff * k;
}

void argb_led_br_fade_start(uint8_t target_br, uint16_t fade_time_ms)
{
    g_argb_br_fade_config.start_br = g_argb_br_fade_config.current_br;
    g_argb_br_fade_config.target_br = target_br;
    g_argb_br_fade_config.fade_time_ms = fade_time_ms;
    g_argb_br_fade_config.start_ms = prismFX_get_timems();
    g_argb_br_fade_config.active = true;
}

void argb_send_all(void)
{
    uint16_t led_num = argb_get_led_total_len();
    uint8_t br = g_argb_br_fade_config.current_br;
    for (uint16_t i = 0; i < led_num; i++) {

        argb_color_type_st color = g_argb_color_payload[i];

        color.R = (color.R * br) / 100;
        color.G = (color.G * br) / 100;
        color.B = (color.B * br) / 100;

        argb_gamma_handle(&color);
        argb_led_short_handle(&color, &argb_color_payload_tx_buf[i]);
    }
    argb_drv_sendata((const uint8_t *)argb_color_payload_tx_buf, led_num * sizeof(argb_color_type_st));
}

void argb_set_all_color(argb_color_type_st color)
{
    uint16_t led_num = argb_get_led_total_len();

    for (size_t i = 0; i < led_num; i++) {
        g_argb_color_payload[i] = color;
    }
}