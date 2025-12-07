#ifndef __DEV_PUBLIC_H__
#define __DEV_PUBLIC_H__

#include "sdkconfig.h"

#include "stdint.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "info_config.h"
#include "log.h"
#include "system_time_adapter.h"
#include "ble_iot.h"

#include "dev_register.h"
#include "led_pilot.h"
#include "argb_core.h"
#include "dev_data_event.h"


void prismFX_system_init(void);
#endif