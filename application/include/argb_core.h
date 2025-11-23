#ifndef __ARGB_CORE_H__
#define __ARGB_CORE_H__

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} color_st;

/**
 * @brief 模式类型
 */
typedef enum {
    STATIC_MODE,
    DYNAMIC_MODE,
}argb_mode_type_et;

/**
 * @brief 静态模式
 */
typedef struct {
    color_st color;
    uint8_t seg_num;
    color_st seg_color[10];

} argb_static_mode_st;

/**
 * @brief 动态模式
 */
typedef struct {
    uint8_t bg_color_num;
    uint8_t fg_color_num;

    color_st bg_color[8];
    color_st fg_color[8];

    uint8_t speed;
} argb_dynamic_mode_st;

typedef struct {
    bool power_sw;

    argb_mode_type_et mode_type;
    uint8_t mode;

    uint8_t brightness;
    argb_static_mode_st static_mode;
    argb_dynamic_mode_st dynamic_mode;

    
    uint16_t led_num;
} argb_config_st;


void prismFX_light_init(void);
#endif
