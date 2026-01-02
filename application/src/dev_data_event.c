#include "sdkconfig.h"
#include "dev_public.h"
#include "ble_iot.h"

#define TAG "DataEvent"
/**
 * @brief 灯光数据解析
 * @param item
 */
static void prismFX_ble_lcdata_press(ble_iot_data_queue_item_st *item)
{
    // uint16_t attr = item->cmd;
    // uint16_t data_len = item->payload_len - 2; // 减cmd
    // uint8_t  *data = item->payload + 2;
}

/**
 * @brief 蓝牙数据回调
 */
void prismFX_ble_data_cb(ble_iot_data_queue_item_st *item)
{
    LOGI(TAG, "Accept data(%d):", item->payload_len);
    for (uint16_t i = 0; i < item->payload_len; i++) {
        printf("%02X ", item->payload[i]);
    }
    printf("\n");
    
    if (item->opcode == 0x00) {
        prismFX_ble_lcdata_press(item);
    }
}

/**
 * @brief 数据回调注册
 */
void prismFX_data_register(void)
{
    ble_iot_register_data_callback(prismFX_ble_data_cb);
}