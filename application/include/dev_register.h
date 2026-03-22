#ifndef __DEV_REGISTER_H__
#define __DEV_REGISTER_H__

#include "dev_public.h"
#define REGISTER1_NVS_KEY   "REGISTER1"

typedef struct {

    // 受预览控制
    struct {
        argb_config_st argb;
    } power_off_save;

    // 不受预览控制
    struct {
        uint16_t led_num;
    } persistent;

} g_register_st;


/**
 * @brief 退出预览模式行为
 */
typedef enum {
    DISCARD_PREVIEW = 0,
    SAVE_PREVIEW
} register_preview_exit_et;


/**
 * @brief 读注册表
 */
void prismFX_register_read(void);

/**
 * @brief 写注册表
 */
void prismFX_register_save(void);

/**
 * @brief 清空注册表
 */
void prismFX_register_clean(void);

/**
 * @brief 获取注册表指针
 */
g_register_st *prismFX_register_get_ptr(void);

/**
 * @brief 获取影子注册表指针
 */
g_register_st *prismFX_register_get_shardow_ptr(void);

/**
 * @brief 进入预览模式
 */
void prismFX_register_enter_preview(void);

/**
 * @brief 退出预览模式
 */
void prismFX_register_exit_preview(register_preview_exit_et apply);

/**
 * @brief 注册表初始化
 */
void prismFX_register_init(void);


#ifdef __cplusplus
}
#endif

#endif