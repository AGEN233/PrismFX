#ifndef __UTILS_LOG_H__
#define __UTILS_LOG_H__
#include "stdio.h"

#define LOGI(tag, fmt, ...)  printf("%sI[%s: %s] " fmt "%s\n", "\033[37m", "APP", tag, ##__VA_ARGS__, "\033[0m")
#define LOGW(tag, fmt, ...)  printf("%sW[%s: %s] " fmt "%s\n", "\033[33m", "APP", tag, ##__VA_ARGS__, "\033[0m")
#define LOGE(tag, fmt, ...)  printf("%sE[%s: %s] " fmt "%s\n", "\033[31m", "APP", tag, ##__VA_ARGS__, "\033[0m")

#endif