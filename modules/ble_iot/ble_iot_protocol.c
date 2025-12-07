#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#define TAG "BLE(IOT)DATA"

// dataqueue
static QueueHandle_t g_tx_queue = NULL;
static QueueHandle_t g_rx_queue = NULL;
static uint8_t g_data_sn = 0;

// data callback
static ble_iot_app_data_cb_t g_app_data_cb = NULL;


/**
 * @brief checksum计算
 */
static uint8_t calculate_checksum(uint8_t *data, uint16_t len) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < len; i++) {
        checksum ^= data[i];  // XOR操作计算校验和
    }
    return checksum;
}

/**
 * @brief GATT数据接收回调
 */
int ble_gatt_iot_rx_data_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint8_t *data = ctxt->om->om_data;
    uint16_t len = ctxt->om->om_len;

    ble_iot_data_queue_item_st item;

    if (len < 8) {
        LOGE(TAG, "Data too short for protocol header| %d\n", len);
        return BLE_ATT_ERR_INVALID_PDU;
    }

    uint8_t index = 0;
    uint16_t header = (data[index] << 8) | (data[index + 1]);
    index += 2;
    if (header != 0xA00B) {
        LOGE(TAG, "Data Header error| src:%04X \n", header);
        return BLE_ATT_ERR_INVALID_PDU;
    }

    uint8_t sn = data[index++];
    if (sn != 0 && sn == g_data_sn) {
        LOGE(TAG, "Data SN error| src:%02X local:%02X \n", sn, g_data_sn);
        return BLE_ATT_ERR_INVALID_PDU;
    }  
    g_data_sn = sn;

    uint8_t ver = data[index++];
    uint8_t opcode = data[index++];

    uint16_t payload_len = data[index] << 8 | data[index + 1];
    index += 2;
    if (payload_len != (len - index - 1)) {
        LOGE(TAG, "Data length error| total: %d payload: %d src: %d\n", len, (len - index - 1), payload_len);
        return BLE_ATT_ERR_INVALID_PDU;
    }

    uint8_t src_checksum = data[len - 1];
    uint8_t calculated_checksum = calculate_checksum(&data[0], len - 1);
    if (src_checksum != calculated_checksum) {
        LOGE(TAG, "Checksum error| src: %02X, local: %02X\n", src_checksum, calculated_checksum);
        return BLE_ATT_ERR_INVALID_PDU;
    }

    if (payload_len > 0) {
        memcpy(item.payload, &data[index], payload_len);
        item.payload_len = payload_len;
        item.opcode = opcode;
        item.ver = ver;
        item.cmd = (data[index] << 8) | data[index + 1];

        if (xQueueSend(g_rx_queue, &item, portMAX_DELAY) != pdPASS) {
            LOGE(TAG, "Failed to send received data to RX queue");
            return BLE_ATT_ERR_INVALID_PDU;
        }

        if (g_app_data_cb) {
            g_app_data_cb(&item);
        }
    }
    return 0;
}

/**
 * @brief 回调注册
 * @param cb
 */
void ble_iot_register_data_callback(ble_iot_app_data_cb_t cb)
{
    if (cb) {
        g_app_data_cb = cb;
    }
}

/**
 * @brief gatt数据队列初始化
 */
void ble_iot_gatt_data_queue_init(void)
{
    g_tx_queue = xQueueCreate(10, sizeof(ble_iot_data_queue_item_st));
    g_rx_queue = xQueueCreate(10, sizeof(ble_iot_data_queue_item_st));

    if (g_tx_queue == NULL || g_tx_queue == NULL) {
        LOGE(TAG, "Failed to create data queue");
        return;
    }
}
#endif