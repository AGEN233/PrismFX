#ifdef CONFIG_ENABLE_BLE_IOT

#include "ble_iot_internal.h"
#define TAG "BLE(IOT)GAP"

static struct ble_npl_callout mtu_timeout_timer_handle;
static bool ble_is_connect = false;
volatile uint16_t g_ble_gap_mtu = 23;
uint16_t g_conn_handle = BLE_HS_CONN_HANDLE_NONE;

/**
 * @brief 主动发起mtu协商
 * @param ev
 */
static void ble_iot_gap_mtu_exchange_initiate(struct ble_npl_event *ev)
{
    if (g_ble_gap_mtu == 23 && g_conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        int ret = ble_gattc_exchange_mtu(g_conn_handle, NULL, NULL);
        if (ret != 0) {
            LOGE(TAG, "initiate MTU exchange ERROR| %d", ret);
        }
    }
}

/**
 * @brief GAP连接处理
 */
static void ble_iot_gap_connect_handle(struct ble_gap_event *event)
{
    if (event->connect.status == 0) {
        ble_is_connect = true;
        g_conn_handle = event->connect.conn_handle;

        struct ble_gap_conn_desc conn_desc;
        if (ble_gap_conn_find(g_conn_handle, &conn_desc) == 0) {
            const uint8_t *addr = conn_desc.peer_id_addr.val;
            LOGI(TAG, "master mac: %02X:%02X:%02X:%02X:%02X:%02X",
                 addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
        } else {
            LOGW(TAG, "master mac get fail");
        }

        ble_npl_time_t ticks;
        ble_npl_time_ms_to_ticks(500, &ticks);
        ble_npl_callout_reset(&mtu_timeout_timer_handle, ticks);

        LOGI(TAG, ">>>>>>BLE CONNECT");
    }
}

/**
 * @brief GAP断开连接处理
 */
static void ble_iot_gap_disconnect_handle(void)
{
    ble_npl_callout_stop(&mtu_timeout_timer_handle);
    ble_is_connect = false;
    g_conn_handle = BLE_HS_CONN_HANDLE_NONE; // 重置连接句柄
    ble_iot_adv_start();
    LOGI(TAG, ">>>>>>BLE DISCONNECTED");
}

/**
 * @brief MTU更新处理
 */
static void ble_iot_gap_mtu_handle(struct ble_gap_event *event)
{
    g_ble_gap_mtu = event->mtu.value;
    ble_npl_callout_stop(&mtu_timeout_timer_handle);
    LOGI(TAG, ">>>>>>BLE MTU: %d", g_ble_gap_mtu);
}

/**
 * @brief 获取MTU
 * @return uint16_t
 */
uint16_t ble_iot_gap_get_mtu(void)
{
    return g_ble_gap_mtu;
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
        case BLE_GAP_EVENT_MTU: {
            ble_iot_gap_mtu_handle(event);
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
    ble_svc_gap_init();
    ble_npl_callout_init(&mtu_timeout_timer_handle, nimble_port_get_dflt_eventq(), ble_iot_gap_mtu_exchange_initiate, NULL);
}

#endif