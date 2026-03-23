#include "dev_public.h"
#include "argb_drv.h"

#define TAG "ARGB(CORE)"

// extern argb_mode_loop_ptr_t dynamic_mode_get_loop(uint8_t mode);
static argb_mode_loop_ptr_t g_mode_loop = NULL;

/**
 * @brief 更新设置模式
 */
static void argb_get_mode_update(void)
{
    uint8_t mode = argb_get_mode();

    switch (argb_get_mode_type()) {
        case STATIC_MODE: {
            g_mode_loop = static_mode_get_loop(mode);
            break;
        }
        case DYNAMIC_MODE: {
            break;
        }
        default: {
            g_mode_loop = NULL;
            break;
        }
    }
}

/**
 * @brief 灯光循环处理loop
 * @param arg
 */
static void argb_main_loop(void *arg)
{
    bool power_last = !argb_get_power();
    while (1) {
        bool power_sw = argb_get_power();
        argb_frame_br_fade_config_st *fade = argb_led_get_br_fade_config();

        if (power_last != power_sw) {
            argb_led_br_fade_start(power_sw ? argb_get_unif_br() : 0, 900);
            power_last = power_sw;
        }

        // mode refresh
        if (g_mode_loop == NULL) {
            argb_get_mode_update();

        } else if (argb_mode_get_refresh()) {
            argb_get_mode_update();
            argb_mode_clear_refresh();
            g_mode_loop();
        }

        // mode loop
        if ((power_sw || fade->current_br > 0) && g_mode_loop) {
            g_mode_loop();
        }

        argb_led_br_fade_handle();

        argb_send_all();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief 灯光初始化
 */
void prismFX_light_init(void)
{
    g_register_st *reg = prismFX_register_get_ptr();

    argb_drv_set_led_len(reg->persistent.led_num);
    argb_driver_init();

    BaseType_t ret = xTaskCreate(argb_main_loop, "argb_main_loop", (16 * 1024), NULL, 5, NULL);
    assert(ret == pdPASS);
}