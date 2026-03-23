#ifndef __DEV_UTILS_H__
#define __DEV_UTILS_H__

#include "stdint.h"

#include "dev_public.h"
uint32_t util_math_map(uint32_t src, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);
void hs_to_rgb_convert(const argb_color_HStype_st *hs, argb_color_type_st *rgb);
void rgb_to_hs_convert(const argb_color_type_st *rgb, argb_color_HStype_st *hs);
#endif