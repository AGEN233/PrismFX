#ifndef __DEV_REGISTER_H__
#define __DEV_REGISTER_H__
#include "argb_core.h"

#define REGISTER1_NVS_KEY   "REGISTER1"

/**
 * @brief 全局注册表
 */
typedef struct {
    // 掉电保存
    struct {
        argb_config_st argb;
    } power_off_save;

    // 非掉电保存(临时)
    struct {
        bool preview_flag;
    } power_off_nosave;
} g_register_st;


void prismFX_register_read(void);
void prismFX_register_save(void);
void prismFX_register_clean(void);
g_register_st *prismFX_register_get_ptr(void);
void prismFX_register_init(void);
#endif