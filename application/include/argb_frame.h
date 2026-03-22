#ifndef __ARGB_FEAME_H__
#define __ARGB_FEAME_H__

typedef struct {
    float  current_br;      /* 当前亮度    (0-255) */
    uint8_t  start_br;      /* 渐变起始亮度 (0-255) */
    uint8_t  target_br;     /* 目标亮度     (0-255) */
    uint16_t fade_time_ms;  /* 渐变总时间 */
    uint32_t start_ms;      /* 渐变开始时间 */
    bool     active;        /* 是否正在渐变 */ 
} argb_frame_br_fade_config_st;

argb_frame_br_fade_config_st *argb_led_get_br_fade_config(void);
void argb_led_br_fade_handle(void);
void argb_led_br_fade_start(uint8_t target_br, uint16_t fade_time_ms);
void argb_send_all(void);
void argb_set_all_color(argb_color_type_st color);
#endif