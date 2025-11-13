# application/CMakeLists.txt

set(srcs
    "app_main.c"
    "src/argb_drv.c"
    "src/argb_drv_ws2812b.c"
)

set(includes
    "include"
    "."
)


set(requires
    "utils"
    "argb_driver"
)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES ${requires}
)
