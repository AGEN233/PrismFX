#include "dev_utils.h"

/**
 * @brief 线性整数范围映射
 * @param src      输入值
 * @param in_min   输入最小值
 * @param in_max   输入最大值
 * @param out_min  输出最小值
 * @param out_max  输出最大值
 * @return 映射后的输出值
 */
uint32_t util_math_map(uint32_t src, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    if (src <= in_min) return out_min;
    if (src >= in_max) return out_max;

    return (src - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * @brief HS转RGB
 * @param hs
 * @param rgb
 */
void hs_to_rgb_convert(const argb_color_HStype_st *hs, argb_color_type_st *rgb)
{
    float hf = (float)hs->H;
    float sf = (float)hs->S / 255.0f;

    float c = sf;
    float x = c * (1.0f - fabsf(fmodf(hf / 60.0f, 2.0f) - 1.0f));
    float m = 1.0f - c;

    float rf = 0, gf = 0, bf = 0;

    if (hf < 60) {
        rf = c;
        gf = x;
    } else if (hf < 120) {
        rf = x;
        gf = c;
    } else if (hf < 180) {
        gf = c;
        bf = x;
    } else if (hf < 240) {
        gf = x;
        bf = c;
    } else if (hf < 300) {
        rf = x;
        bf = c;
    } else {
        rf = c;
        bf = x;
    }

    rgb->R = (uint8_t)((rf + m) * 255.0f + 0.5f);
    rgb->G = (uint8_t)((gf + m) * 255.0f + 0.5f);
    rgb->B = (uint8_t)((bf + m) * 255.0f + 0.5f);
}

/**
* @brief RGB转HS
* @param rgb
* @param hs
*/
void rgb_to_hs_convert(const argb_color_type_st *rgb, argb_color_HStype_st *hs)
{
    uint8_t max = rgb->R;
    if (rgb->G > max) max = rgb->G;
    if (rgb->B > max) max = rgb->B;

    uint8_t min = rgb->R;
    if (rgb->G < min) min = rgb->G;
    if (rgb->B < min) min = rgb->B;

    uint8_t delta = max - min;

    float H = 0;

    if (delta != 0) {

        if (max == rgb->R) {

            H = 60.0f * (float)(rgb->G - rgb->B) / delta;

        } else if (max == rgb->G) {

            H = 120.0f + 60.0f * (float)(rgb->B - rgb->R) / delta;

        } else {

            H = 240.0f + 60.0f * (float)(rgb->R - rgb->G) / delta;
        }

        if (H < 0)
            H += 360.0f;
    }

    uint8_t S = 0;

    if (max != 0) {
        S = (uint16_t)delta * 255 / max;
    }

    hs->H = (uint16_t)(H + 0.5f);
    hs->S = S;
}