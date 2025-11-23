#ifndef __DEV_REGISTER_H__
#define __DEV_REGISTER_H__
#include "argb_core.h"

#define REGISTER1_NVS_KEY   "REGISTER1"

/**
 * @brief 全局注册表
 */
typedef struct {
    // argb_config_st argb;
    // 掉电保存
    struct {
        argb_config_st argb;
    } power_off_save;

    // 非掉电保存(临时)
    struct {
        bool preview_flag;
    } power_off_nosave;
} g_register_st;


#endif