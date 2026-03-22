#include "sdkconfig.h"
#include "dev_public.h"
#include "ble_iot.h"

#define TAG "DataEvent"


static void EF00_data_handle(const ble_iot_data_queue_item_st *item)
{

}

static void EF01_data_handle(const ble_iot_data_queue_item_st *item)
{

}

/**
 * @brief 设置全屏静态模式
 * @param item
 */
static void EF02_data_handle(const uint8_t *data, uint8_t len)
{
    uint8_t sw = data[0];
    uint8_t br = data[1];
    uint8_t mode = data[3];

    uint16_t h = ((uint16_t)data[4] << 8) | data[5];
    uint8_t  s = data[6];

    uint8_t current_mode = argb_get_mode();
    prismFX_register_exit_preview(DISCARD_PREVIEW);

    argb_set_power(sw);
    argb_set_unif_br(br);

    hs_to_rgb_convert(
    &(argb_color_HStype_st) {
        .H = h,
        .S = s
    },
    &argb_get_static_config()->color
    );

    argb_set_mode_type(STATIC_MODE);
    if (mode != current_mode) {
        argb_set_mode(mode);
        argb_mode_set_refresh();
    }
}

static void prismFX_ble_public_data_press(const ble_iot_data_queue_item_st *item)
{
    // uint16_t attr = item->cmd;
    // uint16_t data_len = item->payload_len - 2; // 减cmd
    // uint8_t  *data = item->payload + 2;
}

/**
 * @brief 灯光数据解析
 * @param item
 */
static void prismFX_ble_led_data_press(const ble_iot_data_queue_item_st *item)
{
    uint16_t attr = item->cmd;
    switch (attr) {
        case 0xEF00: {
            break;
        }
        case 0xEF01: {
            break;
        }
        case 0xEF02: {
            EF02_data_handle(item->payload, item->payload_len);
            break;
        }

        default: {
            break;
        }
    }
}

/**
 * @brief 蓝牙数据回调
 */
void prismFX_ble_data_cb(ble_iot_data_queue_item_st *item)
{
    printf("BLE RX FRAME OPCODE:%02X CMD:%04X LEN:%02X -> ",
           item->opcode,
           item->cmd,
           item->payload_len);

    for (uint8_t i = 0; i < item->payload_len; i++) {
        printf("%02X ", item->payload[i]);
    }

    printf("\r\n");

    switch (item->opcode) {
        case 0x00:
            prismFX_ble_public_data_press(item);
            break;

        case 0x01:
            prismFX_ble_led_data_press(item);
            break;

        default:
            break;
    }
}

/**
 * @brief 数据回调注册
 */
void prismFX_data_register(void)
{
    ble_iot_register_data_callback(prismFX_ble_data_cb);
}