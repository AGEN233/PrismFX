#ifdef CONFIG_ENABLE_BLE_IOT
#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_store.h"
#include "host/util/util.h"
#include "esp_mac.h"
#include "log.h"

#define TAG "BLE(IOT)"

static TaskHandle_t ble_iot_host_handle = NULL;
static uint8_t g_mac_type = 0;


/**
 * @brief 获取mac类型
 * @return  #define BLE_ADDR_PUBLIC      (0x00)
            #define BLE_ADDR_RANDOM      (0x01)
            #define BLE_ADDR_PUBLIC_ID   (0x02)
            #define BLE_ADDR_RANDOM_ID   (0x03)
            #define BLE_ADDR_ANONYMOUS   (0xFF)
 */
uint8_t ble_iot_get_mac_type(void)
{
    return g_mac_type;
}

/**
 * @brief 获取蓝牙公共地址
 * @param mac
 */
void ble_iot_get_mac_addr(uint8_t *mac)
{
    if (!mac) return;
    esp_read_mac(mac, ESP_MAC_BT);
}
/**
 * @brief
 */
static void ble_iot_on_stack_reset_cb(int reason)
{

}

/**
 * @brief 协议栈与控制器完成同步回调
 */
static void ble_iot_on_stack_sync_cb(void)
{
    /* On stack sync, do advertising initialization */
    LOGI(TAG, "Host Sync BleContorll success");
    ble_iot_gap_init();
    ble_iot_gatt_init();
    ble_iot_adv_init();
}

/**
 * @brief
 */
static int ble_iot_on_stack_store_status_cb(struct ble_store_status_event *event, void *arg)
{
    return 0;
}

/**
 * @brief 蓝牙主机任务
 */
static void ble_iot_host_task(void *arg)
{
    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    /* Clean up at exit */
    vTaskDelete(NULL);
}

/**
 * @brief nimble栈初始化
 */
static void ble_iot_nimble_init(void)
{
    /* NimBLE host stack initialization */
    esp_err_t ret = nimble_port_init();
    if (ret != ESP_OK) {
        LOGE(TAG, "nimble port init error| %s\n", esp_err_to_name(ret));
        return;
    }

    /* NimBLE host configuration initialization */
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = ble_iot_on_stack_reset_cb;
    ble_hs_cfg.sync_cb = ble_iot_on_stack_sync_cb;
    ble_hs_cfg.store_status_cb = ble_iot_on_stack_store_status_cb;


    /* Start NimBLE host task thread and return */
    if (ble_iot_host_handle == NULL) {
        xTaskCreate(ble_iot_host_task, "NimBLE Host", 4 * 1024, NULL, 5, &ble_iot_host_handle);
        if (ble_iot_host_handle == NULL) {
            LOGE(TAG, "host task create error\n");
            return;
        }
    }

    g_mac_type  = BLE_ADDR_PUBLIC;
    uint8_t mac[6];
    if (ble_hs_id_copy_addr(g_mac_type, mac, NULL) != 0) {
        LOGW(TAG, "No PUBLIC MAC FallBack to RANDOM");
        g_mac_type = BLE_ADDR_RANDOM;
    } else {
        LOGI(TAG, "BT MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    }
}

/**
 * @brief 蓝牙初始化
 */
void ble_iot_init(void)
{
    ble_iot_nimble_init();
}
#endif