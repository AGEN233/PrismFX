#include "dev_public.h"
#include "static_mode/static_mode.h"

static void static_full_mode(void)
{
    static argb_color_type_st start_color = {0};
    static argb_color_type_st curent_color   = {0};
    static argb_color_type_st last_target = {0};

    static uint32_t start_time = 0;
    static bool inited = false;

    const argb_color_type_st *target = &argb_get_static_config()->color;

    uint8_t cur_br = argb_led_get_br_fade_config()->current_br;

    uint32_t now = prismFX_get_timems();

    if (!inited) {
        if (cur_br == 0) {
            // 灯刚开 -> 不做颜色渐变 防止和亮度渐亮冲突
            curent_color = *target;
            start_color = *target;
        } else {
            // 从别的模式切进来 -> 渐变过去
            start_color = curent_color;
            start_time = now;
        }
        last_target = *target;
        inited = true;
    }

    if (memcmp(target, &last_target, sizeof(*target)) != 0) {

        start_color = curent_color;
        start_time = now;
        last_target = *target;
    }

    uint32_t elapsed_ms = now - start_time;

    float t = elapsed_ms / 900.0f;
    if (t > 1.0f) {
        t = 1.0f;
    }

    curent_color.R = start_color.R + (target->R - start_color.R) * t;
    curent_color.G = start_color.G + (target->G - start_color.G) * t;
    curent_color.B = start_color.B + (target->B - start_color.B) * t;

    argb_set_all_color(curent_color);
}

void static_mode_type_init(void)
{

}

static const argb_mode_loop_ptr_t g_static_mode_table[256] = {
    [MODE_FULL_STATIC] = static_full_mode,
};

argb_mode_loop_ptr_t static_mode_get_loop(uint8_t mode)
{
    return g_static_mode_table[mode];
}