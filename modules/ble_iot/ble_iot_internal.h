#pragma once

#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "nimble/ble.h"
#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "log.h"
#ifdef CONFIG_ENABLE_BLE_IOT



// protocol
void ble_iot_gatt_data_queue_init(void);
int ble_gatt_iot_rx_data_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

// gatt
void ble_iot_gatt_init(void);

// adv
void ble_iot_adv_init(void);

// gap
int ble_iot_gap_event_cb(struct ble_gap_event *event, void *arg);
void ble_iot_gap_init(void);
#endif