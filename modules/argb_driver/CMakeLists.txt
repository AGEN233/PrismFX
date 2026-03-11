idf_component_register(
    SRCS
    "argb_drv_ws2812b_spi.c"
    "argb_drv_ws2812b_rmt.c"
    "argb_drv_sk6812.c"

    INCLUDE_DIRS "."
    REQUIRES utils
    PRIV_REQUIRES esp_driver_spi esp_driver_rmt
)

target_compile_options(${COMPONENT_LIB} PRIVATE "-include${CMAKE_BINARY_DIR}/config/sdkconfig.h")
