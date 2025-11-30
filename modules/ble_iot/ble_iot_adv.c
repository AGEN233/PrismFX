#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "host/ble_hs.h"
#include "log.h"
#define TAG "BLE(IOT)ADV"

static ble_iot_adv_data_t              g_ble_iot_adv_data;
static struct ble_hs_adv_fields        ble_iot_adv_fields;
static struct ble_gap_adv_params       ble_iot_adv_params;

#define BLE_IOT_ADV_MAX_LEN     32

/**
 * @brief 蓝牙广播开始
 */
void ble_iot_adv_start(void)
{
    int ret = ble_gap_adv_set_fields(&ble_iot_adv_fields);
    if (ret) {
        LOGE(TAG, "ble_gap_adv_set_fields failed| %d", ret);
        return;
    }

    ret = ble_gap_adv_start(ble_iot_get_mac_type(), NULL, BLE_HS_FOREVER, &ble_iot_adv_params, NULL, NULL);

    if (ret) {
        LOGE(TAG, "adv_start failed| %d", ret);
        return;
    }
    LOGI(TAG, "ble adv start");
}

/**
 * @brief 蓝牙广播停止
 */
void ble_iot_adv_stop(void)
{
    int rc = ble_gap_adv_stop();
    if (rc) {
        LOGW(TAG, "adv_stop failed| %d", rc);
        return;
    }
}

/**
 * @brief 更新蓝牙广播
 */
void ble_iot_adv_update(const ble_iot_adv_data_t *adv_data)
{
    if (!adv_data) return;

    static uint8_t device_name_buf[31];
    static uint8_t svc_data_buf[7];

    g_ble_iot_adv_data = *adv_data;

    ble_iot_adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    uint8_t mac[6] = {0};
    ble_iot_get_mac_addr(mac);

    uint8_t max_name_len = sizeof(device_name_buf) - 1 - 8;
    uint8_t name_len = adv_data->device_name_len;
    if (name_len > max_name_len) name_len = max_name_len;

    memcpy(device_name_buf, adv_data->device_name, name_len);
    device_name_buf[name_len] = '_';
    for (int i = 0; i < 4; i++) {
        uint8_t b = mac[2 + i];
        device_name_buf[name_len + 1 + i * 2]     = (b >> 4) < 10 ? '0' + (b >> 4) : 'A' + (b >> 4) - 10;
        device_name_buf[name_len + 1 + i * 2 + 1] = (b & 0x0F) < 10 ? '0' + (b & 0x0F) : 'A' + (b & 0x0F) - 10;
    }

    ble_iot_adv_fields.name = device_name_buf;
    ble_iot_adv_fields.name_len = name_len + 1 + 8;
    ble_iot_adv_fields.name_is_complete = 1;

    svc_data_buf[0] = mac[5];
    svc_data_buf[1] = (uint8_t)adv_data->dual_ic;
    svc_data_buf[2] = adv_data->device_type;
    svc_data_buf[3] = (adv_data->fw_version >> 8) & 0xFF;
    svc_data_buf[4] = adv_data->fw_version & 0xFF;
    svc_data_buf[5] = (adv_data->dual_ic) ? adv_data->ic2_device_type : 0xFF;
    svc_data_buf[6] = (adv_data->dual_ic) ? adv_data->ic2_fw_version : 0xFF;

    ble_iot_adv_fields.uuids16 = NULL;
    ble_iot_adv_fields.uuids16_is_complete = 0;

    ble_iot_adv_fields.svc_data_uuid16 = svc_data_buf;
    ble_iot_adv_fields.svc_data_uuid16_len = sizeof(svc_data_buf);
}




/**
 * @brief 蓝牙广播初始化
 */
void ble_iot_adv_init(void)
{
    memset(&ble_iot_adv_fields, 0, sizeof(ble_iot_adv_fields));
    memset(&ble_iot_adv_params, 0, sizeof(ble_iot_adv_params));
    memset(&g_ble_iot_adv_data, 0, sizeof(g_ble_iot_adv_data));

    ble_iot_adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    ble_iot_adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_iot_adv_params.itvl_min = (uint16_t)(CONFIG_BLE_IOT_ADV_ITVL_MIN_MS * 16 / 10);
    ble_iot_adv_params.itvl_max = (uint16_t)(CONFIG_BLE_IOT_ADV_ITVL_MAX_MS * 16 / 10);
    ble_iot_adv_params.channel_map = 0;
    ble_iot_adv_params.filter_policy = 0;
    ble_iot_adv_params.high_duty_cycle = 0;

    static ble_iot_adv_data_t adv_data = {
        .device_type = 0x01,
        .device_name = "prismFX",
        .device_name_len = 8,
        .fw_version = 0x01,
        .dual_ic = false,
    };
    ble_iot_adv_update(&adv_data);
    ble_iot_adv_start();
}
#endif