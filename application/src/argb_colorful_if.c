
#include "argb_colorful_if.h"

bool g_argb_mode_refresh_flag = false;

argb_mode_type_et argb_get_mode_type(void)
{
    return prismFX_register_get_ptr()->power_off_save.argb.mode_type;
}

void argb_set_mode_type(argb_mode_type_et mode_type)
{
    prismFX_register_get_ptr()->power_off_save.argb.mode_type = mode_type;
}

uint8_t argb_get_mode(void)
{
    return prismFX_register_get_ptr()->power_off_save.argb.mode;
}

void argb_set_mode(uint8_t mode)
{
    prismFX_register_get_ptr()->power_off_save.argb.mode = mode;
}

argb_static_mode_st *argb_get_static_config(void)
{
    return &prismFX_register_get_ptr()->power_off_save.argb.static_mode;
}

/**
 * @brief 通知模式需要切出
 */
void argb_mode_set_refresh(void)
{
    g_argb_mode_refresh_flag = true;
}

/**
 * @brief 清除模式刷新标志
 */
void argb_mode_clear_refresh(void)
{
    g_argb_mode_refresh_flag = false;
}

/**
 * @brief 查询模式是否需要刷新
 */
bool argb_mode_get_refresh(void)
{
    return g_argb_mode_refresh_flag;
}


/**
 * @brief 设置亮度
 */
void argb_set_unif_br(uint8_t br)
{
    prismFX_register_get_ptr()->power_off_save.argb.brightness = br;
    if (prismFX_register_get_ptr()->power_off_save.argb.power_sw) {
        argb_led_br_fade_start(br, 900);
    }
}

/**
 * @brief  查询当前亮度
 */
uint8_t argb_get_unif_br(void)
{
    return prismFX_register_get_ptr()->power_off_save.argb.brightness;
}

/**
 * @brief 设置开关状态
 * @param sw true:开 false:关
 */
void argb_set_power(argb_power_type_et sw)
{
    prismFX_register_get_ptr()->power_off_save.argb.power_sw = sw;
}

/**
 * @brief 查询设置开关状态
 * @return true
 * @return false
 */
argb_power_type_et argb_get_power(void)
{
    return prismFX_register_get_ptr()->power_off_save.argb.power_sw;
}

/**
 * @brief  查询LED点数
 */
uint16_t argb_get_led_total_len(void)
{
    return prismFX_register_get_ptr()->persistent.led_num;
}