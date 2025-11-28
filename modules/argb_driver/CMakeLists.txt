set(SRCS "")

if(CONFIG_ARGB_TYPE_WS2812B_SPI)
    list(APPEND SRCS "argb_drv_ws2812b_spi.c")
elseif(CONFIG_ARGB_TYPE_SK6812)
    list(APPEND SRCS "argb_drv_sk6812.c")
endif()

idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "."
    REQUIRES utils
    PRIV_REQUIRES esp_driver_spi
)

target_compile_options(${COMPONENT_LIB} PRIVATE "-include${CMAKE_BINARY_DIR}/config/sdkconfig.h")
