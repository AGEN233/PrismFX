#ifndef __ARGB_COLORFUL_IF_H__
#define __ARGB_COLORFUL_IF_H__
#include "dev_public.h"

typedef struct {
    float  current_br;      /* 当前亮度    (0-255) */
    uint8_t  start_br;      /* 渐变起始亮度 (0-255) */
    uint8_t  target_br;     /* 目标亮度     (0-255) */
    uint16_t fade_time_ms;  /* 渐变总时间 */
    uint32_t start_ms;      /* 渐变开始时间 */
    bool     active;        /* 是否正在渐变 */ 
} argb_color_fade_config_st;

argb_mode_type_et argb_get_mode_type(void);
void argb_set_mode_type(argb_mode_type_et mode_type);
uint8_t argb_get_mode(void);
void argb_set_mode(uint8_t mode);

argb_static_mode_st *argb_get_static_config(void);

void argb_mode_set_refresh(void);
bool argb_mode_get_refresh(void);
void argb_mode_clear_refresh(void);

void argb_set_unif_br(uint8_t br);
uint8_t argb_get_unif_br(void);

void argb_set_power(argb_power_type_et sw);
argb_power_type_et argb_get_power(void);

uint16_t argb_get_led_total_len(void);
#endif