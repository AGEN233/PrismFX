#include "dev_public.h"
#include "esp_app_desc.h"
#define TAG "app_main"
static void prismFX_system_print_firmWare_info(void)
{
#define FW_T "FW"
    const esp_app_desc_t *fwinfo = esp_app_get_description();
    LOGI(FW_T, "FirmWareInfo: ");
    LOGI(FW_T, "DeviceType: %02X (%s)", DEVICE_TYPE, DEVICE_NAME);
    LOGI(FW_T, "FirmWareVersion: %04X", FIRMWARE_VERSION);
    LOGI(FW_T, "BuildTime: %s %s", fwinfo->date, fwinfo->time);
    LOGI(FW_T, "IDFVersion: %s\n", fwinfo->idf_ver);
#undef FW_T
}

const ble_iot_adv_data_st device_info = {
    .device_type = 0xA0,
    .device_name = "prismFX",
    .device_name_len = 8,
    .fw_version = 0x01,
    .dual_ic = false,
};

static void prismFX_ble_iot_init(void)
{
    ble_iot_init();

    ble_iot_adv_update(&device_info);
    ble_iot_adv_start();

    prismFX_data_register();
}

void prismFX_system_init(void)
{

    prismFX_system_print_firmWare_info();
    prismFX_ble_iot_init();
    prismFX_register_init();


    // led_pilot_init();

    prismFX_light_init();
}