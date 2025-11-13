/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "dev_public.h"

#define TAG "main"
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    uint32_t flash_size;
    esp_flash_get_size(NULL, &flash_size);
    printf("\n");
    ESP_LOGI(TAG, "Hello>>>World!");
    ESP_LOGI(TAG, "This Chip is %s ", CONFIG_IDF_TARGET);
    ESP_LOGI(TAG, "External Flash: %u bytes (~%u KB)", flash_size, flash_size / 1024);

    prismFX_system_init();
}
