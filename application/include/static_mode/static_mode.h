#ifndef __STATIC_MODE_H__
#define __STATIC_MODE_H__
#include "dev_public.h"

typedef enum {
    MODE_FULL_STATIC = 0x01,
} argb_static_mode_et;

argb_mode_loop_ptr_t static_mode_get_loop(argb_static_mode_et mode);
#endif
