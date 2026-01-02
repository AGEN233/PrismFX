#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"
#define TAG "BLE(IOT)DATA"

// dataqueue
QueueHandle_t g_tx_queue = NULL;
QueueHandle_t g_rx_queue = NULL;
static uint8_t g_data_rx_sn = 0;
static uint8_t g_data_tx_sn = 0;

// data callback
ble_iot_app_data_cb_t g_app_data_cb = NULL;

/**
 * @brief checksum计算
 */
static uint8_t calculate_checksum(const uint8_t *data, const uint16_t len)
{
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < len; i++) {
        checksum += data[i];
    }
    return checksum;
}

/**
 * @brief notify切片发送
 * @param data
 * @param len
 */
void ble_iot_control_notify(uint8_t *data, uint16_t len)
{
    if (g_conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        return;
    }

    uint16_t payload_len_max = g_ble_gap_mtu - 3;
    uint16_t payload_sent = 0; // 已发送的长度
    int ret;

    while (payload_sent < len) {
        // 当前分包的长度
        uint16_t chunk_len = len - payload_sent;
        if (chunk_len > payload_len_max) {
            chunk_len = payload_len_max;
        }

        struct os_mbuf *om = ble_hs_mbuf_from_flat(data + payload_sent, chunk_len);
        if (!om) {
            LOGE(TAG, "Failed to allocate mbuf for chunk.");
            return;
        }

        ret = ble_gattc_notify_custom(g_conn_handle, gatt_contor_notify_handle, om);
        if (ret == 0) {
            payload_sent += chunk_len;
        }else if (ret == BLE_HS_EAGAIN) {
            // buf满了重试
            os_mbuf_free(om); // 先释放buf
            LOGW(TAG, "BLE buffer full retrying...");
            vTaskDelay(pdMS_TO_TICKS(10));
        } else {
            // 其他错误，不处理
            os_mbuf_free(om);
            LOGE(TAG, "GATT Notify error|%d", ret);
            return;
        }
    }

}

/**
 * @brief 蓝牙notify数据发送
 * @param opcode
 * @param data
 * @param len
 */
void ble_iot_sendata(uint8_t opcode, uint8_t *data, uint16_t len)
{
    if (!data) {
        LOGE(TAG, "Send data is empty");
        return;
    }
    if (len >= 240) {
        LOGE(TAG, "Sen data too Long|max:240\n");
        return;
    }

    ble_send_queue_packet_st item;
    uint8_t *buf = item.payload;
    uint8_t index = 0;

    buf[index++] = 0xA0;
    buf[index++] = 0x0B;
    buf[index++] = 0x00;
    buf[index++] = g_data_tx_sn++;

    buf[index++] = opcode;
    buf[index++] = (len >> 8) & 0xFF;
    buf[index++] = len & 0xFF;

    memcpy(&buf[index], data, len);
    index += len;

    uint8_t checksum = calculate_checksum(buf, index);
    buf[index++] = checksum;

    item.payload_len = index;

    if (xQueueSend(g_tx_queue, &item, pdMS_TO_TICKS(10)) != pdPASS) {
        LOGW(TAG, "BLE send queue is full! Data dropped.");
        return;
    }
}

/**
 * @brief 蓝牙协议解析器
 * @param data
 * @param len
 */
uint8_t ble_iot_data_protocol_parser(uint8_t *data, uint16_t len)
{
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

    uint8_t ver = data[index++];

    uint8_t sn = data[index++];
    if (sn != 0 && sn == g_data_rx_sn) {
        LOGE(TAG, "Data SN error| src:%02X local:%02X \n", sn, g_data_rx_sn);
        return BLE_ATT_ERR_INVALID_PDU;
    }
    g_data_rx_sn = sn;

    uint8_t opcode = data[index++];

    uint16_t payload_len = data[index] << 8 | data[index + 1];
    index += 2;
    if (payload_len != (len - index - 1)) {
        LOGE(TAG, "Data length error| total: %d payload: %d src: %d\n", len, (len - index - 1), payload_len);
        return BLE_ATT_ERR_INVALID_PDU;
    }

    uint8_t src_checksum = data[len - 1];
    uint8_t calculated_checksum = calculate_checksum(data, len - 1);
    if (src_checksum != calculated_checksum) {
        LOGE(TAG, "Checksum error| src: %02X, local: %02X\n", src_checksum, calculated_checksum);
        return BLE_ATT_ERR_INVALID_PDU;
    }

    if (payload_len > 2) {

        memset(&item, 0, sizeof(item));

        item.opcode = opcode;
        item.ver = ver;
        item.cmd = (data[index] << 8) | data[index + 1];
        index += 2;

        item.payload_len = payload_len - 2; // 去掉CMD
        memcpy(item.payload, &data[index], item.payload_len);
         
        if (xQueueSend(g_rx_queue, &item, portMAX_DELAY) != pdPASS) {
            // 推送到队列
            LOGE(TAG, "Failed to send received data to RX queue");
            return BLE_ATT_ERR_INVALID_PDU;
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

#endif