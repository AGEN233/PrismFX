#ifdef CONFIG_ARGB_TYPE_WS2812B_SPI
#include "argb_drv.h"

#include "esp_attr.h"
#include "driver/spi_master.h"
#define TAG "WS2812B(SPI)"

#define SPI_BITS_PER_WS2812_BIT      4
#define BYTES_PER_LED               ((3 * 8 * SPI_BITS_PER_WS2812_BIT) / 8)

// WS2812B Driver buff
static DMA_ATTR uint8_t ws2812b_spi_buf[CONFIG_LED_COUNT_MAX * BYTES_PER_LED];
static uint8_t ws2812b_rgb_buf[CONFIG_LED_COUNT_MAX * 3];

// WS2812B RGB - > SPI LUT
static const uint16_t ws2812b_half_lut_buf[16] = {
    0x8888,  // 0b0000 → 1000 1000 1000 1000
    0x888E,  // 0b0001 → 1000 1000 1000 1110
    0x88E8,  // 0b0010 → 1000 1000 1110 1000
    0x88EE,  // 0b0011 → 1000 1000 1110 1110
    0x8E88,  // 0b0100 → 1000 1110 1000 1000
    0x8E8E,  // 0b0101 → 1000 1110 1000 1110
    0x8EE8,  // 0b0110 → 1000 1110 1110 1000
    0x8EEE,  // 0b0111 → 1000 1110 1110 1110
    0xE888,  // 0b1000 → 1110 1000 1000 1000
    0xE88E,  // 0b1001 → 1110 1000 1000 1110
    0xE8E8,  // 0b1010 → 1110 1000 1110 1000
    0xE8EE,  // 0b1011 → 1110 1000 1110 1110
    0xEE88,  // 0b1100 → 1110 1110 1000 1000
    0xEE8E,  // 0b1101 → 1110 1110 1000 1110
    0xEEE8,  // 0b1110 → 1110 1110 1110 1000
    0xEEEE   // 0b1111 → 1110 1110 1110 1110
};

static volatile bool ws2812b_sendata_flag = false;
static bool ws2812b_init_done = false;
static spi_device_handle_t spi_handle = NULL;               // SPIHandle
static TaskHandle_t ws2812b_spi_drv_handle = NULL;          // TaskHandle

/**
 * @brief 设置发送灯光数据
 * @param color_data
 * @param len
 */
void argb_drv_sendata(const uint8_t *color_data, uint16_t len)
{
    if (len > CONFIG_LED_COUNT_MAX) len = CONFIG_LED_COUNT_MAX;
    memcpy(ws2812b_rgb_buf, color_data, (len * 3));
}

/**
 * @brief RGB ->ws2812b(查表) -> SPIBIT
 * @param rgb_buf
 * @param spi_buf
 */
static inline void ws2812b_data_rgb_to_spi(const uint8_t *rgb_buf, uint8_t *spi_buf)
{
    uint32_t spi_index = 0;
    for (uint16_t i = 0; i < CONFIG_LED_COUNT_MAX; i++) {
        uint8_t R = rgb_buf[i * 3];
        uint8_t G = rgb_buf[(i * 3) + 1];
        uint8_t B = rgb_buf[(i * 3) + 2];

        uint8_t colors[3] = {G, R, B};

        for (uint8_t i = 0; i < 3; i++) {
            uint8_t color_val = colors[i];
            uint8_t high = color_val >> 4;
            uint8_t low = color_val & 0x0F;

            uint16_t spi_high = ws2812b_half_lut_buf[high];
            uint16_t spi_low = ws2812b_half_lut_buf[low];

            spi_buf[spi_index++] = (spi_high >> 8) & 0xFF;
            spi_buf[spi_index++] = spi_high & 0xFF;
            spi_buf[spi_index++] = (spi_low >> 8) & 0xFF;
            spi_buf[spi_index++] = spi_low & 0xFF;
        }
    }
}

/**
 * @brief 驱动线程
 */
static void ws2812b_spi_drv_task(void *arg)
{
    static spi_transaction_t t;
    static spi_transaction_t *ret_trans;
    while (1) {
        if (!ws2812b_init_done) {
            vTaskDelay(pdMS_TO_TICKS(40));
            continue;
        }

        ws2812b_data_rgb_to_spi(ws2812b_rgb_buf, ws2812b_spi_buf);

        // SPI发送
        memset(&t, 0, sizeof(t));
        t.length = CONFIG_LED_COUNT_MAX * BYTES_PER_LED * 8;
        t.tx_buffer = ws2812b_spi_buf;

        spi_device_queue_trans(spi_handle, &t, portMAX_DELAY);
        spi_device_get_trans_result(spi_handle, &ret_trans, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/**
 * @brief 驱动初始化
 */
static void ws2812b_spi_init(void)
{
    esp_err_t ret;
    const static spi_bus_config_t buscfg = {
        .mosi_io_num = CONFIG_ARGB_OUT_PIN,
        .sclk_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = CONFIG_LED_COUNT_MAX *  BYTES_PER_LED,
    };
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        LOGE(TAG, "spi bus init error(%d)| %s", ret, esp_err_to_name(ret));
        return;
    }

    const static spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 2500000, // 2.5M 波特率
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1,
    };
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        LOGE(TAG, "spi device init error(%d)| %s", ret, esp_err_to_name(ret));
        return;
    }

    memset(ws2812b_spi_buf, 0, CONFIG_LED_COUNT_MAX * BYTES_PER_LED);
    memset(ws2812b_rgb_buf, 0, CONFIG_LED_COUNT_MAX * 3);

    if (ws2812b_spi_drv_handle == NULL) {
        xTaskCreate(ws2812b_spi_drv_task, "ws2812b_spi_drv_task", CONFIG_WS2812B_STACK_SIZE,
                    NULL, 5, &ws2812b_spi_drv_handle);

        if (ws2812b_spi_drv_handle == NULL) {
            LOGI(TAG, "driver task create error\n");
            return;
        }
    }

    ws2812b_init_done = true;
    LOGI(TAG, "driver init success");
}

/**
 * @brief 驱动初始化
 */
void argb_driver_init(void)
{
    ws2812b_spi_init();
}

#endif