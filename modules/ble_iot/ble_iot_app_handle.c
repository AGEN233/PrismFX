#ifdef CONFIG_ENABLE_BLE_IOT

#include "ble_iot.h"
#include "ble_iot_internal.h"
#define TAG "BLE(IOT)APP"

extern ble_iot_adv_data_st g_ble_iot_adv_data;

/**
 * @brief 添加设备处理
 * @param data
 */
static void ble_iot_adddevice_event(const uint8_t *data)
{
    uint16_t app_ver = ((uint16_t)data[0] << 8) | data[1];
    LOGI(TAG, "APP Version: 0x%04X", app_ver);

    uint8_t *buf = calloc(6, sizeof(uint8_t));
    if (!buf) {
        LOGE(TAG, "adddevice calloc error!");
    }
    uint8_t index = 0;
    buf[index++] = 0x41;
    buf[index++] = 0x02;

    buf[index++] = 0x01;

    buf[index++] = (g_ble_iot_adv_data.device_type >> 8) & 0xFF;
    buf[index++] = (g_ble_iot_adv_data.fw_version >> 8) & 0xFF;
    buf[index++] = g_ble_iot_adv_data.fw_version & 0xFF;

    ble_iot_sendata(0x00, buf, index);
}

/**
 * @brief 蓝牙数据统一处理
 * @param item
 */
void ble_iot_unfied_data(ble_iot_data_queue_item_st *item)
{
    switch (item->opcode) {
        case 0x00: {
            switch (item->cmd) {
                case 0x4101: {
                    ble_iot_adddevice_event(item->payload);
                }
                break;

                default: {
                    if (g_app_data_cb) {
                        g_app_data_cb(item);
                    }
                }
                break;
            }
        }
        break;

        default: {
            if (g_app_data_cb) {
                g_app_data_cb(item);
            }
        }
        break;
    }
}

#endif