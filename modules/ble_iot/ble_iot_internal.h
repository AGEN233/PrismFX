#pragma once

#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "nimble/ble.h"
#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "log.h"

#ifdef CONFIG_ENABLE_BLE_IOT
// gatt
void ble_iot_gatt_init(void);

// adv
void ble_iot_adv_init(void);

// gap
int ble_iot_gap_event_cb(struct ble_gap_event *event, void *arg);
void ble_iot_gap_init(void);
#endif