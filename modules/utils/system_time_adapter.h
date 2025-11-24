#ifndef __UTILS_SYSTEM_TIME_ADAPTER_H__
#define __UTILS_SYSTEM_TIME_ADAPTER_H__
#include "esp_timer.h"

#define prismFX_get_timeus()    (esp_timer_get_time())
#define prismFX_get_timems()    (prismFX_get_timeus() / 1000ULL)
#define prismFX_get_times()     (prismFX_get_timems() / 1000ULL)

#endif