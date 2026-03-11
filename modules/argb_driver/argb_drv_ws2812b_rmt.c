#ifdef CONFIG_ARGB_TYPE_WS2812B_RMT
#include "argb_drv.h"
#include "driver/rmt_tx.h"
#include "esp_attr.h"
#define TAG "WS2812B(RMT)"

#define WS2812B_RMT_HZ 10000000
static uint8_t ws2812b_rgb_buf[CONFIG_LED_COUNT_MAX * 3];
static SemaphoreHandle_t ws2812b_rmt_tx_done_sem;
static TaskHandle_t ws2812b_rmt_drv_task_handle = NULL;          // TaskHandle
static rmt_channel_handle_t led_chan = NULL;
static rmt_encoder_handle_t simple_encoder = NULL;
static rmt_tx_channel_config_t tx_chan_config;

rmt_transmit_config_t tx_config = {
    // no transfer loop
    .loop_count = 0,
};

static const rmt_symbol_word_t ws2812b_bit0 = {
    .level0 = 1,
    .duration0 = 3,
    .level1 = 0,
    .duration1 = 9
};

static const rmt_symbol_word_t ws2812b_bit1 = {
    .level0 = 1,
    .duration0 = 9,
    .level1 = 0,
    .duration1 = 3
};

static const  rmt_symbol_word_t ws2812b_reset = {
    .duration0 = 250,
    .level0 = 0,
    .duration1 = 250,
    .level1 = 0
};

static size_t encoder_callback(const void *data, size_t data_size, size_t symbols_written,  size_t symbols_free, rmt_symbol_word_t *symbols, bool *done, void *arg)
{
    // We need a minimum of 8 symbol spaces to encode a byte. We only
    // need one to encode a reset, but it's simpler to simply demand that
    // there are 8 symbol spaces free to write anything.
    if (symbols_free < 8) {
        return 0;
    }

    // We can calculate where in the data we are from the symbol pos.
    // Alternatively, we could use some counter referenced by the arg
    // parameter to keep track of this.
    size_t data_pos = symbols_written / 8;
    uint8_t *data_bytes = (uint8_t *)data;
    if (data_pos < data_size) {
        // Encode a byte
        size_t symbol_pos = 0;
        for (int bitmask = 0x80; bitmask != 0; bitmask >>= 1) {
            if (data_bytes[data_pos]&bitmask) {
                symbols[symbol_pos++] = ws2812b_bit1;
            } else {
                symbols[symbol_pos++] = ws2812b_bit0;
            }
        }
        // We're done; we should have written 8 symbols.
        return symbol_pos;
    } else {
        //All bytes already are encoded.
        //Encode the reset, and we're done.
        symbols[0] = ws2812b_reset;
        *done = 1; //Indicate end of the transaction.
        return 1; //we only wrote one symbol
    }
}

static bool rmt_tx_done_callback(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup = pdFALSE;

    xSemaphoreGiveFromISR(ws2812b_rmt_tx_done_sem, &high_task_wakeup);

    return high_task_wakeup == pdTRUE;
}

void argb_drv_sendata(const uint8_t *color_data, uint16_t len)
{
    if (len > CONFIG_LED_COUNT_MAX) len = CONFIG_LED_COUNT_MAX;
    memcpy(ws2812b_rgb_buf, color_data, (len * 3));
}

static void ws2812b_rmt_drv_task(void *arg)
{
    while (1) {
        ESP_ERROR_CHECK(rmt_transmit(led_chan, simple_encoder, ws2812b_rgb_buf, 9, &tx_config));
        xSemaphoreTake(ws2812b_rmt_tx_done_sem, portMAX_DELAY);
    }
}

static void ws2812b_rmt_init(void)
{
    tx_chan_config.clk_src = RMT_CLK_SRC_DEFAULT; // select source clock
    tx_chan_config.gpio_num = CONFIG_ARGB_OUT_PIN;
    tx_chan_config.mem_block_symbols = 64;// increase the block size can make the LED less flickering
    tx_chan_config.resolution_hz = WS2812B_RMT_HZ;
    tx_chan_config.trans_queue_depth = 4;// set the number of transactions that can be pending in the background
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    rmt_tx_event_callbacks_t cbs = {
        .on_trans_done = rmt_tx_done_callback,
    };
    ESP_ERROR_CHECK(rmt_tx_register_event_callbacks(led_chan, &cbs, NULL));
    ws2812b_rmt_tx_done_sem = xSemaphoreCreateBinary();

    const rmt_simple_encoder_config_t simple_encoder_cfg = {
        .callback = encoder_callback
        //Note we don't set min_chunk_size here as the default of 64 is good enough.
    };
    ESP_ERROR_CHECK(rmt_new_simple_encoder(&simple_encoder_cfg, &simple_encoder));

    ESP_ERROR_CHECK(rmt_enable(led_chan));

    memset(ws2812b_rgb_buf, 0, CONFIG_LED_COUNT_MAX * 3);

    BaseType_t ret = xTaskCreate(ws2812b_rmt_drv_task, "ws2812b_spi_drv_task", CONFIG_WS2812B_STACK_SIZE, NULL, 5, &ws2812b_rmt_drv_task_handle);
    assert(ret == pdPASS);
}
/**
 * @brief 驱动初始化
 */
void argb_driver_init(void)
{
    ws2812b_rmt_init();
}
#endif