# application/CMakeLists.txt

set(srcs
    "app_main.c"
    "src/dev_register.c"
    "src/argb_core.c"
    "src/led_pilot.c"
    "src/dev_data_event.c"
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
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES ${requires}
)
