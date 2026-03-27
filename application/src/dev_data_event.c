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
    &argb_get_static_config()->color[0]
    );
    argb_get_static_config()->color_num = 1;

    argb_set_mode_type(STATIC_MODE);
    if (mode != current_mode) {
        argb_set_mode(mode);
        argb_mode_set_refresh();
    }

    prismFX_state_report();
}

/**
 * @brief 状态上报
 */
void prismFX_state_report(void)
{
    extern ble_iot_adv_data_st device_info;

    uint8_t buf[49] = {0};

    buf[0] = 0xFF;
    buf[1] = 0xFF;

    buf[2] = device_info.device_type;
    buf[3] = device_info.fw_version >> 8;
    buf[4] = device_info.fw_version & 0xFF;

    buf[5] = device_info.dual_ic;
    buf[6] = device_info.ic2_device_type;
    buf[7] = device_info.ic2_fw_version >> 8;
    buf[8] = device_info.ic2_fw_version & 0xFF;

    buf[9]  = CONFIG_LED_COUNT_MAX >> 8;
    buf[10] = CONFIG_LED_COUNT_MAX & 0xFF;

    buf[11] = argb_get_led_total_len() >> 8;
    buf[12] = argb_get_led_total_len() & 0xFF;

    buf[13] = 0; // 排序类型，再说
    buf[14] = argb_get_power();
    buf[15] = argb_get_unif_br();
    buf[16] = 0; // 速度，再说
    buf[17] = argb_get_mode_type();
    buf[18] = argb_get_mode();

    buf[19] = 0; // 模式可配置选项1，再说
    buf[20] = 0; // 模式可配置选项2，再说
    buf[21] = 0; // 模式可配置选项3，再说
    buf[22] = 0; // 下拉选项，再说


    const argb_color_type_st *color_ptr = NULL;
    uint8_t color_num = 0;

    if (argb_get_mode_type() == STATIC_MODE) {

        const argb_static_mode_st *cfg = argb_get_static_config();

        color_ptr = cfg->color;
        color_num = cfg->color_num;

    } else if (argb_get_mode_type() == DYNAMIC_MODE) {

    }


    if (color_num > 8) {
        color_num = 8;
    }

    buf[23] = color_num;
    argb_color_HStype_st hs;

    for (uint8_t i = 0; i < color_num; i++) {

        rgb_to_hs_convert(&color_ptr[i], &hs);

        buf[24 + i * 3] = hs.H >> 8;
        buf[25 + i * 3] = hs.H & 0xFF;
        buf[26 + i * 3] = hs.S;
    }

    ble_iot_sendata(0x00, buf, sizeof(buf));
}

/**
 * @brief opcode: 0x00数据解析
 * @param item
 */
static void prismFX_ble_public_data_press(const ble_iot_data_queue_item_st *item)
{
    uint16_t attr = item->cmd;
    switch (attr) {
        case 0xFFFF: {
            prismFX_state_report();
            break;
        }
        default: {
            break;
        }
    }
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
 * @brief 按钮单击回调
 */
static void prismFX_button_single_cb(void)
{
    argb_set_power(!argb_get_power());
    prismFX_state_report();
}


/**
 * @brief 数据回调注册
 */
void prismFX_data_register(void)
{
    ble_iot_register_data_callback(prismFX_ble_data_cb);
    button_register_single_cb(prismFX_button_single_cb);
}