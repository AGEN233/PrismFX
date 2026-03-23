#include "static_mode/static_mode.h"

/**
 * @brief 全屏静态模式
 */
static void static_full_mode(void)
{
    static argb_color_type_st start_color = {0};
    static argb_color_type_st curent_color = {0};
    static argb_color_type_st last_target = {0};

    static uint32_t start_time = 0;
    static bool inited = false;

    const argb_color_type_st *target = &argb_get_static_config()->color[0];

    uint8_t cur_br = argb_led_get_br_fade_config()->current_br;

    uint32_t now = prismFX_get_timems();

    if (!inited) {

        if (cur_br == 0) {
            // 灯刚开 -> 不做颜色渐变，避免与亮度渐变叠加
            curent_color = *target;
            start_color = *target;
        } else {
            // 从其他模式切入 -> 从当前颜色渐变到目标颜色
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

/**
 * @brief 静态模式类型初始化
 */
void static_mode_type_init(void)
{

}

/**
 * @brief 获取指定静态模式的执行函数
 * @param mode 模式编号
 * @return 模式循环函数指针，若模式不存在则返回 NULL
 */
argb_mode_loop_ptr_t static_mode_get_loop(argb_static_mode_et mode)
{
    switch (mode) {

        case MODE_FULL_STATIC: {
            return static_full_mode;
        }

        default: {
            return NULL;
        }
    }
}