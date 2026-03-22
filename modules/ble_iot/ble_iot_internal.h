#pragma once

#include "ble_iot.h"
#include "ble_iot_internal.h"
#include "os/os_cputime.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "log.h"

#include "sys/time.h"
#include "time.h"
#ifdef CONFIG_ENABLE_BLE_IOT

// app_handle
void ble_iot_unfied_data(ble_iot_data_queue_item_st *item);

//bik task
void ble_iot_biz_task_init(void);


// protocol
extern QueueHandle_t g_rx_queue;
extern QueueHandle_t g_tx_queue;
extern ble_iot_app_data_cb_t g_app_data_cb;
void ble_iot_control_notify(uint8_t *data, uint16_t len);
uint8_t ble_iot_data_protocol_parser(uint8_t *data, uint16_t len);

// gatt
extern uint16_t gatt_contor_notify_handle;
ble_uuid16_t *ble_iot_gatt_get_uuid(uint8_t *count);
void ble_iot_gatt_init(void);

// adv
void ble_iot_adv_init(void);

// gap
extern volatile uint16_t g_ble_gap_mtu;
extern uint16_t g_conn_handle;
int ble_iot_gap_event_cb(struct ble_gap_event *event, void *arg);
void ble_iot_gap_init(void);
#endif