#ifdef CONFIG_ENABLE_BLE_IOT

#include "ble_iot_internal.h"

#define TAG "BLE(IOT)GAP"
static struct ble_gap_conn_params ble_iot_conn_params;
int ble_iot_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT: {
            printf("1111111111");
            break;
        }
        default: {
            printf("1111111121122111");
            break;
        }
    }
    return 0;
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