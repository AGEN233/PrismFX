#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "host/ble_hs.h"
#include "log.h"
#define TAG "BLE(IOT)ADV"

static ble_iot_adv_data_st              g_ble_iot_adv_data;
static struct ble_hs_adv_fields        ble_iot_adv_fields;
static struct ble_hs_adv_fields        ble_iot_scan_fields;
static struct ble_gap_adv_params       ble_iot_adv_params;

#define BLE_IOT_ADV_MAX_LEN         31
#define BLE_IOT_ADV_DEVICE_NAME_MAX 8
#define BLE_IOT_ADV_DEVICE_MAC_LEN  6
#define BLE_IOT_ADV_DEVICE_MAX      (BLE_IOT_ADV_DEVICE_NAME_MAX + BLE_IOT_ADV_DEVICE_MAC_LEN)

/**
 * @brief 转 MAC 后三字节为 ASCII HEX
 */
static void mac_to_ascii_suffix(uint8_t mac[6], char buf[6])
{
    snprintf(buf, 7, "%02X%02X%02X", mac[3], mac[4], mac[5]);
}

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
    ret = ble_gap_adv_set_fields(&ble_iot_scan_fields);
    if (ret) {
        LOGE(TAG, "ble_gap_scanfadv_set_fields failed| %d", ret);
        return;
    }

    ret = ble_gap_adv_start(ble_iot_get_mac_type(), NULL, BLE_HS_FOREVER, &ble_iot_adv_params, ble_iot_gap_event_cb, NULL);

    if (ret) {
        LOGE(TAG, "adv_start failed| %d", ret);
        return;
    }
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
void ble_iot_adv_update(const ble_iot_adv_data_st *adv_data)
{
    if (!adv_data) return;
    g_ble_iot_adv_data = *adv_data;

    ble_iot_adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    uint8_t mac[6];
    ble_iot_get_mac_addr(mac);

    static char device_name[BLE_IOT_ADV_DEVICE_MAX];
    uint8_t name_len = strlen((const char *)adv_data->device_name);
    if (name_len > BLE_IOT_ADV_DEVICE_NAME_MAX) name_len = BLE_IOT_ADV_DEVICE_NAME_MAX;

    memcpy(device_name, adv_data->device_name, name_len);
    mac_to_ascii_suffix(mac, &device_name[name_len]);

    ble_iot_adv_fields.name = (const uint8_t *)device_name;
    ble_iot_adv_fields.name_len = name_len + BLE_IOT_ADV_DEVICE_MAC_LEN;
    ble_iot_adv_fields.name_is_complete = 1;

    static uint8_t mfg_data[10] = {0};
    mfg_data[0] = 0x41;
    mfg_data[1] = 0x47;

    mfg_data[2] = adv_data->device_type;
    mfg_data[3] = (adv_data->fw_version >> 8) & 0xFF;
    mfg_data[4] = adv_data->fw_version & 0xFF;
    mfg_data[5] = adv_data->device_type;

    mfg_data[6] = adv_data->dual_ic;
    mfg_data[7] = adv_data->dual_ic ? adv_data->ic2_device_type : 0xFF;
    mfg_data[8] = adv_data->dual_ic ? ((adv_data->ic2_fw_version >> 8) & 0xFF) : 0xFF;
    mfg_data[9] = adv_data->dual_ic ? (adv_data->ic2_fw_version & 0xFF) : 0xFF;

    ble_iot_adv_fields.mfg_data = mfg_data;
    ble_iot_adv_fields.mfg_data_len = sizeof(mfg_data);
    
}
/**
 * @brief 蓝牙广播初始化
 */
void ble_iot_adv_init(void)
{
    memset(&ble_iot_adv_fields, 0, sizeof(ble_iot_adv_fields));
    memset(&ble_iot_adv_params, 0, sizeof(ble_iot_adv_params));
    memset(&ble_iot_scan_fields, 0, sizeof(ble_iot_scan_fields));
    memset(&g_ble_iot_adv_data, 0, sizeof(g_ble_iot_adv_data));

    ble_iot_adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    ble_iot_adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_iot_adv_params.itvl_min = (uint16_t)(CONFIG_BLE_IOT_ADV_ITVL_MIN_MS * 16 / 10);
    ble_iot_adv_params.itvl_max = (uint16_t)(CONFIG_BLE_IOT_ADV_ITVL_MAX_MS * 16 / 10);
    ble_iot_adv_params.channel_map = 0;
    ble_iot_adv_params.filter_policy = 0;
    ble_iot_adv_params.high_duty_cycle = 0;

    static ble_iot_adv_data_st adv_data = {
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