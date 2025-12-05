#ifdef CONFIG_ENABLE_BLE_IOT

#include "ble_iot_internal.h"

#define TAG "BLE(IOT)GAP"
static struct ble_gap_conn_params ble_iot_conn_params;
static bool ble_is_connect = false;
static uint8_t mac_addr_master[6] = {0};

/**
 * @brief GAP连接处理
 */
static void ble_iot_gap_connect_handle(struct ble_gap_event *event)
{
    LOGI(TAG, ">>>>>>BLE CONNECT");
    ble_is_connect = true;
    uint16_t conn_handle = event->connect.conn_handle;

    struct ble_gap_conn_desc conn_desc;
    int ret = ble_gap_conn_find(conn_handle, &conn_desc);
    if (ret == 0) {
        memcpy(mac_addr_master, conn_desc.peer_id_addr.val, sizeof(mac_addr_master));
        LOGI(TAG, "device mac: %02X:%02X:%02X:%02X:%02X:%02X", mac_addr_master[5], mac_addr_master[4], mac_addr_master[3], mac_addr_master[2], mac_addr_master[1], mac_addr_master[0]);
    } else {
        LOGW(TAG, "device mac get fail| %d", ret);
    }
}

/**
 * @brief GAP断开连接处理
 */
static void ble_iot_gap_disconnect_handle(void)
{
    LOGI(TAG, ">>>>>>BLE DISCONNECT");
    ble_is_connect = false;
    ble_iot_adv_start();

}

/**
 * @brief GAP回调
 */
int ble_iot_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT: {
            ble_iot_gap_connect_handle(event);
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT: {
            ble_iot_gap_disconnect_handle();
            break;
        }
        default: {
            break;
        }
    }
    return 0;
}

/**
 * @brief 获取蓝牙链接状态
 * @return true
 * @return false
 */
bool ble_iot_get_connect_status(void)
{
    return ble_is_connect;
}

/**
 * @brief GAP参数设置和初始化
 */
void ble_iot_gap_init(void)
{
    ble_iot_conn_params.itvl_min = 24;
    ble_iot_conn_params.itvl_max = 40;
    ble_iot_conn_params.latency = 0;
    ble_iot_conn_params.supervision_timeout = 500 / 10;
    ble_iot_conn_params.min_ce_len = 0;
    ble_iot_conn_params.max_ce_len = 0;
}

#endif