#ifndef __BLE_IOT_H__
#define __BLE_IOT_H__
#include "stdint.h"

/**
 * @brief 广播数据
 */
typedef struct {
    uint8_t  device_type;
    uint8_t  device_name[8];
    uint8_t  device_name_len;
    uint16_t fw_version;
    
    bool     dual_ic;
    uint8_t  ic2_device_type;
    uint16_t  ic2_fw_version;
} ble_iot_adv_data_st;


// ble adv
void ble_iot_adv_start(void);
void ble_iot_adv_stop(void);
void ble_iot_adv_update(const ble_iot_adv_data_st *adv_data);

// ble gatt

// ble gap
bool ble_iot_get_connect_status(void);

// ble nimble stack
uint8_t ble_iot_get_mac_type(void);
void ble_iot_get_mac_addr(uint8_t *mac);
void ble_iot_init(void);
#endif