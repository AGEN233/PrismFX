#ifndef __ARGB_DRV_H__
#define __ARGB_DRV_H__

#include "stdint.h"
void argb_drv_sendata(const uint8_t *color_data, uint16_t len);
void argb_driver_init(void);
#endif