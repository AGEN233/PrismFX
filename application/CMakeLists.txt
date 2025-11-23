# application/CMakeLists.txt

set(srcs
    "app_main.c"
    "src/dev_register.c"
    "src/argb_core.c"
)

set(includes
    "include"
    "."
)

set(requires
    "esp_app_format"
    "nvs_flash"
    "utils"
    "argb_driver"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES ${requires}
)
