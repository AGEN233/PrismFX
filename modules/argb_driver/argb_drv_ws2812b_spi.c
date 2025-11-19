#ifdef CONFIG_ARGB_TYPE_WS2812B_SPI
#include "argb_drv.h"
#include "string.h"
#include "log.h"
#include "driver/spi_master.h"
#define TAG "ws2812b"

#define SPI_BITS_PER_WS2812_BIT      4
#define BYTES_PER_LED               ((3 * 8 * SPI_BITS_PER_WS2812_BIT) / 8)

static uint8_t argb_spi_buf[CONFIG_LED_COUNT_MAX * 3];
static uint8_t argb_rgb_buf[CONFIG_LED_COUNT_MAX * 3];
static spi_device_handle_t spi_handle = NULL;

static void ws2812b_spi_init(void)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
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

    spi_device_interface_config_t devcfg = {
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
    memset(argb_spi_buf, 0, sizeof(CONFIG_LED_COUNT_MAX));
    LOGI(TAG, "driver init success");
}

void argb_driver_init(void)
{
    ws2812b_spi_init();
}

#endif 