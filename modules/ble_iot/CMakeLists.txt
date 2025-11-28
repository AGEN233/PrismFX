set(SRCS    "ble_iot.c"
            "ble_iot_adv.c"
            "ble_iot_gatt.c"
            "ble_iot_notify.c"
    )

idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "."
    REQUIRES utils
    PRIV_REQUIRES
)

target_compile_options(${COMPONENT_LIB} PRIVATE "-include${CMAKE_BINARY_DIR}/config/sdkconfig.h")
