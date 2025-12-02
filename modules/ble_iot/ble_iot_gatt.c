#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"
#define TAG "BLE(IOT)GATT"

// CONTOR - > C -> 0x43
static const ble_uuid16_t gatt_svr_svc_control          = BLE_UUID16_INIT(0x4300);
static const ble_uuid16_t gatt_svr_chr_control_write    = BLE_UUID16_INIT(0x4301);
static const ble_uuid16_t gatt_svr_chr_control_read     = BLE_UUID16_INIT(0x4302);

// OTA - > O -> 0x4F
static const ble_uuid16_t gatt_svr_svc_ota              = BLE_UUID16_INIT(0x4F00);
static const ble_uuid16_t gatt_svr_chr_ota_write        = BLE_UUID16_INIT(0x4F01);
static const ble_uuid16_t gatt_svr_chr_ota_read         = BLE_UUID16_INIT(0x4F02);

static int dummy_cb(uint16_t conn_handle, uint16_t attr_handle,
                    struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    return 0;
}
/**
 * @brief GATT服务表
 */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    /*** Service: CONTOR. */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_control.u,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = &gatt_svr_chr_control_write.u,
                .access_cb = dummy_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = &gatt_svr_chr_control_read.u,
                .access_cb = dummy_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
            },
            {0}
        }
    },
    /*** Service: OTA. */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_ota.u,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = &gatt_svr_chr_ota_write.u,
                .access_cb = dummy_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = &gatt_svr_chr_ota_read.u,
                .access_cb = dummy_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
            },
            {0}
        }
    },
    {0}
};


/**
 * @brief GATT初始化
 */
void ble_iot_gatt_init(void)
{
    int ret;
    ret = ble_gatts_count_cfg(gatt_svr_svcs);
    if (ret != 0) {
        LOGE(TAG, "ble_gatts_count_cfg failed| %d", ret);
        return;
    }
    
    ret = ble_gatts_add_svcs(gatt_svr_svcs);
    if (ret != 0) {
        LOGE(TAG, "ble_gatts_add_svcs failed| %d", ret);
        return;
    }
    LOGI(TAG, "GATT init success");
}
#endif