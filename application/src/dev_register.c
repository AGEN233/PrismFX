#include "dev_public.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "REGISTER"

static g_register_st g_register = {0};
static g_register_st g_shadow_register = {0};
static TaskHandle_t g_register_save_handle = NULL;

/**
 * @brief 读nav数据到影子
 */
static size_t prismFX_nvs_read2shadow(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open("register", NVS_READONLY, &handle);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs open error| %s\n", esp_err_to_name(ret));
        return 0;
    }

    size_t data_size = sizeof(g_shadow_register.power_off_save);
    ret = nvs_get_blob(handle, REGISTER1_NVS_KEY, &g_shadow_register.power_off_save, &data_size);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs read error| %s\n", esp_err_to_name(ret));
        return 0;
    }
    nvs_close(handle);

    return data_size;
}

/**
 * @brief 写影子数据到nvs
 */
static void prismFX_nvs_save2shadow(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open("register", NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs open error| %s", esp_err_to_name(ret));
        return;
    }

    size_t data_size = sizeof(g_shadow_register.power_off_save);
    ret = nvs_set_blob(handle, REGISTER1_NVS_KEY, &g_shadow_register.power_off_save, data_size);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs set blob error| %s", esp_err_to_name(ret));
    }

    nvs_commit(handle);
    nvs_close(handle);

    LOGI(TAG, "NVS save done");
}

/**
 * @brief 读注册表
 */
void prismFX_register_read(void)
{
    size_t len = prismFX_nvs_read2shadow();
    if (len <= 0) return;
    memcpy(&g_register.power_off_save, &g_shadow_register.power_off_save, len);
}

/**
 * @brief 写注册表
 */
void prismFX_register_save(void)
{
    if (memcmp(&g_register.power_off_save, &g_shadow_register.power_off_save, sizeof(g_register.power_off_save)) != 0) {

        memcpy(&g_shadow_register.power_off_save, &g_register.power_off_save, sizeof(g_register.power_off_save));
        prismFX_nvs_save2shadow();
    }
}

/**
 * @brief 清空注册表
 */
void prismFX_register_clean(void)
{
    nvs_handle_t handle;
    esp_err_t ret = nvs_open("register", NVS_READWRITE, &handle);
    if (ret != ESP_OK) {
        LOGE(TAG, "nvs open error| %s", esp_err_to_name(ret));
        return;
    }

    memset(&g_register.power_off_save, 0, sizeof(g_register.power_off_save));
    memset(&g_shadow_register.power_off_save, 0, sizeof(g_shadow_register.power_off_save));

    ret = nvs_set_blob(handle,
                       REGISTER1_NVS_KEY,
                       &g_shadow_register.power_off_save,
                       sizeof(g_shadow_register.power_off_save));

    if (ret != ESP_OK) {
        LOGE(TAG, "nvs set blob error| %s", esp_err_to_name(ret));
    }

    nvs_commit(handle);
    nvs_close(handle);

    LOGI(TAG, "Register Clean All (factory reset data cleared)");
}


/**
 * @brief 获取注册表指针
 * @return g_register_st*
 */
g_register_st *prismFX_register_get_ptr(void)
{
    return &g_register;
}

/**
 * @brief 获取影子注册表指针
 * @return g_register_st*
 */
g_register_st *prismFX_register_get_shardow_ptr(void)
{
    return &g_shadow_register;
}

/**
 * @brief 自动保存任务
 */
static void register_auto_save_task(void *arg)
{
    while (1) {
        uint32_t now_time = prismFX_get_timems();
        static uint32_t last_time = 0;

        if (now_time - last_time >= 5000) {

            if (memcmp(&g_shadow_register.power_off_save, &g_register.power_off_save, sizeof(g_register.power_off_save)) != 0) {

                memcpy(&g_shadow_register.power_off_save, &g_register.power_off_save, sizeof(g_register.power_off_save));
                prismFX_nvs_save2shadow();
            }

            last_time = now_time;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/**
 * @brief 默认参数
 */
static void register_set_default(void)
{
    g_register.power_off_save.argb.power_sw = POWER_ON;
    g_register.power_off_save.argb.brightness = 50;
}

/**
 * @brief 注册表初始化
 */
void prismFX_register_init(void)
{
    memset(&g_register.power_off_save, 0, sizeof(g_register.power_off_save));
    memset(&g_shadow_register.power_off_save, 0, sizeof(g_shadow_register.power_off_save));

    prismFX_register_read();

    uint8_t full_zero = 0;
    uint8_t full_0xFF = 0xFF;
    uint8_t *dataptr = (uint8_t *)&g_register.power_off_save;

    // 空芯片判断
    for (uint32_t i = 0; i < sizeof(g_register.power_off_save); i++) {
        full_zero |= dataptr[i];
        full_0xFF &= dataptr[i];
    }
    if (full_zero == 0 || full_0xFF == 0xFF) {
        LOGI(TAG, "This is Factory!!!\n");
        register_set_default();
    }
    LOGI(TAG, "register init success\n");
    // 自动保存
    if (g_register_save_handle == NULL) {
        xTaskCreate(register_auto_save_task, "register_task", 2048,
                    NULL, 5, &g_register_save_handle);

        if (g_register_save_handle == NULL) {
            LOGW(TAG, "register auto save init error\n");
        }
    }
}
