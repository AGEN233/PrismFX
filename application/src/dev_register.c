#include "dev_public.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "REGISTER"

static g_register_st g_register;

static TaskHandle_t g_register_save_handle = NULL;


g_register_st *prismFX_register_get_ptr(void)
{
    return &g_register;
}

void prismFX_register_read(void)
{
    nvs_read();
}

void prismFX_register_save(void *target, const void *source, uint32_t len)
{
    if (!target || !source || len == 0) {
        LOGW(TAG, "Invalid parameters, skip save");
        return;
    }

    // 仅当数据变化时才拷贝并写入NVS
    if (memcmp(target, source, len) != 0) {
        memcpy(target, source, len);
        prismFX_nvs_save();
        LOGI(TAG, "Data changed, saved %u bytes to NVS", (unsigned int)len);
    } else {
        LOGW(TAG, "Data unchanged, skip saving %u bytes", (unsigned int)len);
    }
}


static void register_auto_save_task(void *arg)
{

}

/**
 * @brief 读注册表断电保存数据
 */
static void prismFX_nvs_read(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open("register", NVS_READONLY, &handle);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs open error| %s\n", esp_err_to_name(ret));
        return;
    }

    uint32_t data_size = sizeof(g_register.power_off_save);
    ret = nvs_get_blob(handle, REGISTER1_NVS_KEY, &g_register.power_off_save, &data_size);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs read error| %s\n", esp_err_to_name(ret));
        return;
    }
    nvs_close(handle);
}

/**
 * @brief 写注册表断电保存数据
 */
static void prismFX_nvs_save(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open("register", NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs open error| %s\n", esp_err_to_name(ret));
        return;
    }

    uint32_t data_size = sizeof(g_register.power_off_save);
    ret = nvs_set_blob(handle, REGISTER1_NVS_KEY, &g_register.power_off_save, sizeof(g_register.power_off_save));
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs set blob error| %s\n", esp_err_to_name(ret));
    }
    nvs_commit(handle);
    nvs_close(handle);

    LOGI(TAG, "nvs save done\n");
}


void prismFX_register_init(void)
{

}
