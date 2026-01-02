#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "services/gatt/ble_svc_gatt.h"
#define TAG "BLE(IOT)GATT"

// CONTOR - > C -> 0x43
static const ble_uuid16_t gatt_svr_svc_control          = BLE_UUID16_INIT(0x4300);
static const ble_uuid16_t gatt_svr_chr_control_write    = BLE_UUID16_INIT(0x4301);
static const ble_uuid16_t gatt_svr_chr_control_notify     = BLE_UUID16_INIT(0x4302);
uint16_t gatt_contor_notify_handle;
// OTA - > O -> 0x4F
static const ble_uuid16_t gatt_svr_svc_ota              = BLE_UUID16_INIT(0x4F00);
static const ble_uuid16_t gatt_svr_chr_ota_write        = BLE_UUID16_INIT(0x4F01);
static const ble_uuid16_t gatt_svr_chr_ota_read         = BLE_UUID16_INIT(0x4F02);

/**
 * @brief 获取GATT的UUID
 * @param uuid_list 储存返回的uuid
 * @param count 储存返回的uuid数量
 */
ble_uuid16_t *ble_iot_gatt_get_uuid(uint8_t *count)
{
    static const ble_uuid16_t uuid_list[] = {
        gatt_svr_svc_control,
        gatt_svr_svc_ota
    };

    if (count) {
        *count = sizeof(uuid_list) / sizeof(uuid_list[0]);
    }

    return (ble_uuid16_t *)uuid_list;
}

/**
 * @brief 空回调
 */
static int dummy_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    return 0;
}

/**
 * @brief GATT数据接收回调
 */
int ble_iot_gatt_data_rx_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint8_t *data = ctxt->om->om_data;
    uint16_t len = ctxt->om->om_len;

    uint8_t ret = ble_iot_data_protocol_parser(data, len);
    return ret;
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
                .access_cb = ble_iot_gatt_data_rx_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
            }, {
                .uuid = &gatt_svr_chr_control_notify.u,
                .access_cb = dummy_cb,
                .val_handle = &gatt_contor_notify_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY,
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
                .access_cb = ble_iot_gatt_data_rx_cb,
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
    ble_svc_gatt_init();

    ble_iot_biz_task_init();
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

    ret = ble_gatts_start();
    if (ret != 0) {
        LOGE(TAG, "gatt start failed| %d", ret);
        return;
    }
}
#endif