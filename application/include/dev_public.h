#ifndef __DEV_PUBLIC_H__
#define __DEV_PUBLIC_H__

#include "sdkconfig.h"

#include "stdint.h"
#include "string.h"
#include "math.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "info_config.h"
#include "log.h"
#include "system_time_adapter.h"
#include "ble_iot.h"

#include "argb_core.h"
#include "argb_frame.h"
#include "argb_colorful_if.h"

#include "static_mode/static_mode.h"
#include "led_pilot.h"

#include "dev_data_event.h"
#include "dev_register.h"
#include "dev_utils.h"


void prismFX_system_init(void);
#endif