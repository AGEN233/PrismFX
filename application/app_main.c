#include "dev_public.h"
#include "esp_app_desc.h"
#define TAG "app_main"
static void prismFX_system_print_firmWare_info(void)
{
    #define FW_T "FW" 
    const esp_app_desc_t *fwinfo = esp_app_get_description();
    LOGI(FW_T, "FirmWare info: ");
    LOGI(FW_T, "Device Type: %02X (%s)", DEVICE_TYPE, DEVICE_NAME);
    LOGI(FW_T, "FirmWare Version: %04X", FIRMWARE_VERSION);
    LOGI(FW_T, "BuildTime: %s %s", fwinfo->date, fwinfo->time);
    LOGI(FW_T, "IDF Version  : %s", fwinfo->idf_ver);
    #undef FW_T
}
 
void prismFX_system_init(void)
{

    prismFX_system_print_firmWare_info();
    ble_iot_init();
    prismFX_register_init();
    
    
    led_pilot_init();

    prismFX_light_init();
}