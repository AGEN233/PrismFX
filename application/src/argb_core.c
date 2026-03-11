#include "dev_public.h"
#include "argb_drv.h"

#define TAG "ARGB(CORE)"

static void argb_main_task(void *arg)
{
    // while (1) {

    // }
}

void prismFX_light_init(void)
{
    LOGI(TAG, "prismFX light init\n");

    argb_driver_init();

    color_st buf[] = {
        {0, 255, 0},   // 红
        {255, 0, 0},   // 绿
        {0, 0, 255}    // 蓝
    };

    // 发送数据，长度为 sizeof(buf)，即 3 个字节
    argb_drv_sendata((uint8_t *)buf, 3);
}