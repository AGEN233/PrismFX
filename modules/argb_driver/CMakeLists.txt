set(SRCS
    "argb_drv_ws2812b.c"
    "argb_drv_sk6812.c"
)
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "."
    REQUIRES utils
    PRIV_REQUIRES esp_driver_spi
)