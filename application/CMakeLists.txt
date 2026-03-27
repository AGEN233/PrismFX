# application/CMakeLists.txt

set(srcs
    "app_main.c"
    "src/dev_register.c"
    "src/dev_utils.c"
    "src/argb_core.c"
    "src/argb_frame.c"
    "src/argb_colorful_if.c"
    "src/dev_data_event.c"

    "src/static_mode/static_mode.c"
)

set(includes
    "include"
    "."
)

set(requires
    "esp_app_format"
    "esp_driver_ledc"
    "esp_driver_gpio"
    "nvs_flash"

    "utils"
    "argb_driver"
    "ble_iot"
    "button"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES ${requires}
)
