# application/CMakeLists.txt

set(srcs
    "app_main.c"
    "src/argb_core.c"
)

set(includes
    "include"
    "."
)

set(requires
    "esp_app_format"
    "utils"
    "argb_driver"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES ${requires}
)
