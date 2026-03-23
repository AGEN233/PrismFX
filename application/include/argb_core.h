#ifndef __ARGB_CORE_H__
#define __ARGB_CORE_H__

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} argb_color_type_st;

typedef struct {
    uint16_t H;
    uint8_t S;
} argb_color_HStype_st;

/**
 * @brief 开关类型
 */
typedef enum {
    POWER_OFF = false,
    POWER_ON = true,
} argb_power_type_et;

/**
 * @brief 模式类型
 */
typedef enum {
    STATIC_MODE = 0XC0,
    DYNAMIC_MODE,
} argb_mode_type_et;

/**
 * @brief 静态模式
 */
typedef struct {
    uint8_t color_num;
    argb_color_type_st color[8];
} argb_static_mode_st;

/**
 * @brief 动态模式
 */
typedef struct {
    uint8_t bg_color_num;
    uint8_t fg_color_num;

    argb_color_type_st bg_color[8];
    argb_color_type_st fg_color[8];

    uint8_t speed;
} argb_dynamic_mode_st;

typedef void(*argb_mode_loop_ptr_t)(void);

typedef struct {
    bool power_sw;

    argb_mode_type_et mode_type;
    uint8_t mode;

    uint8_t brightness;
    uint16_t fade_time_ms;
    argb_static_mode_st static_mode;
    argb_dynamic_mode_st dynamic_mode;
} argb_config_st;


void prismFX_light_init(void);
#endif
