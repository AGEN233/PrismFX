#include "dev_public.h"
#include "argb_drv.h"

#define TAG "ARGB(CORE)"

static void argb_main_task(void *arg)
{
    while (1) {
        
    }
}

void prismFX_light_init(void)
{
    LOGI(TAG, "prismFX light init\n");

    argb_driver_init();
}