#include "argb_drv.h"
#include "dev_public.h"

/**
 * @brief 灯光驱动初始化
 */
static void inline prismFX_light_drv_init(void)
{
    ws2812_spi_init();
}

/**
 * @brief 灯光初始化
 */
void prismFX_light_init(void)
{
    prismFX_light_drv_init();
}