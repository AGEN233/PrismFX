#ifndef __ARGB_DRV_H__
#define __ARGB_DRV_H__

#include "stdint.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "log.h"
void argb_drv_set_led_len(uint16_t len);
void argb_drv_sendata(const uint8_t *color_data, uint16_t len);
void argb_driver_init(void);

#endif