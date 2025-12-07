#ifndef __UTILS_LOG_H__
#define __UTILS_LOG_H__
#include "stdio.h"

#define LOGI(tag, fmt, ...)  printf("I[%s] " fmt "\n", tag, ##__VA_ARGS__)
#define LOGW(tag, fmt, ...)  printf("\033[33mW[%s] " fmt "\033[0m\n", tag, ##__VA_ARGS__)
#define LOGE(tag, fmt, ...)  printf("\033[31mE[%s] " fmt "\033[0m\n", tag, ##__VA_ARGS__)



#endif