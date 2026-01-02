#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"

#define TAG "BLE(IOT)BIZ"
#define BLE_BIZ_RX_TASK_PRIORITY  4
#define BLE_BIZ_TX_TASK_PRIORITY  5

/**
 * @brief 接收数据异步推送任务
 */
static void ble_iot_biz_rx_task(void *pvParameters)
{
    ble_iot_data_queue_item_st item;

    while (1) {
        if (xQueueReceive(g_rx_queue, &item, portMAX_DELAY) == pdPASS) {
            ble_iot_unfied_data(&item);
        }
    }
}

/**
 * @brief 异步notify任务
 * @param pvParameters
 */
static void ble_iot_biz_tx_task(void *pvParameters)
{
    ble_send_queue_packet_st item;
    if (g_tx_queue == NULL) {
        return;
    }
    while(1) {
        if (xQueueReceive(g_tx_queue, &item, portMAX_DELAY) == pdPASS) {
            ble_iot_control_notify(item.payload, item.payload_len);
        }
    }
}

/**
 * @brief 蓝牙GATT收发任务初始化
 */
void ble_iot_biz_task_init(void)
{
    if (g_rx_queue == NULL) {
        g_rx_queue = xQueueCreate(10, sizeof(ble_iot_data_queue_item_st));
        if (g_rx_queue == NULL) {
            LOGE(TAG, "Failed to create BLE RX queue!");
            return;
        }
    }
    xTaskCreate(ble_iot_biz_rx_task, "BLEBIZRX", (5 * 1024), NULL, BLE_BIZ_RX_TASK_PRIORITY, NULL);

    if (g_tx_queue == NULL) {
        g_tx_queue = xQueueCreate(10, sizeof(ble_send_queue_packet_st));
        if (g_tx_queue == NULL) {
            LOGE(TAG, "Failed to create BLE TX queue!");
            return;
        }
    }
    xTaskCreate(ble_iot_biz_tx_task, "BLEBIZTX", (5 * 1024), NULL, BLE_BIZ_TX_TASK_PRIORITY, NULL);
}
#endif