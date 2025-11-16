#include "dev_public.h"
#include "esp_app_desc.h"
#define TAG "app_main"
static void prismFX_system_print_firmWare_info(void)
{
    #define FW_T "FW" 
    const esp_app_desc_t *fwinfo = esp_app_get_description();
    LOGI(FW_T, "FirmWare info: ");
    LOGI(FW_T, "FirmWare Version: %04X", FIRMWARE_VERSION);
    LOGI(FW_T, "FirmWare ID: %02X", FIRMWARE_ID);
    LOGI(FW_T, "BuildTime: %s %s", fwinfo->date, fwinfo->time);
    LOGI(FW_T, "IDF Version  : %s", fwinfo->idf_ver);
    LOGI(FW_T, "ELF SHA: %s", fwinfo->app_elf_sha256);
    #undef FW_T
}
 
void prismFX_system_init(void)
{
    prismFX_system_print_firmWare_info();
    // prismFX_light_init();
}