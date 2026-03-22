#ifdef CONFIG_ENABLE_BLE_IOT

#include "ble_iot.h"
#include "ble_iot_internal.h"
#define TAG "BLE(IOT)APP"

extern ble_iot_adv_data_st g_ble_iot_adv_data;

/**
 * @brief 添加设备处理
 * @param data
 */
static void ble_iot_device_info_report(void)
{
    uint8_t *buf = calloc(9, sizeof(uint8_t));
    uint8_t index = 0;
    buf[index++] = 0xFF;
    buf[index++] = 0x01;

    buf[index++] = g_ble_iot_adv_data.device_type;
    buf[index++] = (g_ble_iot_adv_data.fw_version >> 8) & 0xFF;
    buf[index++] = g_ble_iot_adv_data.fw_version & 0xFF;

    buf[index++] = g_ble_iot_adv_data.dual_ic;
    buf[index++] = g_ble_iot_adv_data.ic2_device_type;
    buf[index++] = (g_ble_iot_adv_data.ic2_fw_version >> 8) & 0xFF;
    buf[index++] = g_ble_iot_adv_data.ic2_fw_version & 0xFF;

    ble_iot_sendata(0x00, buf, index);
}

static void ble_iot_system_time_update(const ble_iot_data_queue_item_st *item)
{
    if (item->payload_len < 4) {
        return;
    }

    uint32_t ts = ((uint32_t)item->payload[0] << 24) | ((uint32_t)item->payload[1] << 16) | ((uint32_t)item->payload[2] << 8)  | ((uint32_t)item->payload[3]);
    struct timeval tv = {
        .tv_sec = ts,
        .tv_usec = 0
    };

    settimeofday(&tv, NULL);

    uint8_t buf[2] = {0xFF, 0x02};
    ble_iot_sendata(0x00, buf, 2);
}

/**
 * @brief 蓝牙数据统一处理
 * @param item
 */
void ble_iot_unfied_data(ble_iot_data_queue_item_st *item)
{
    if (item->opcode == 0x00) {
        switch (item->cmd) {
            case 0xFF01: {
                ble_iot_device_info_report();
                return;
            }
            case 0xFF02: {
                ble_iot_system_time_update(item);
                return;
            }
            default: {
                break;
            }
        }
    }

    if (g_app_data_cb) {
        g_app_data_cb(item);
    }
}

#endif