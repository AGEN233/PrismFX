# application/CMakeLists.txt

set(srcs    "app_main.c"
            "src/argb_drv.c"
            "src/argb_drv_ws2812b.c"
)

set(includes    "include"
				".")

idf_component_register(SRCS ${srcs}
					   INCLUDE_DIRS ${includes}
                       REQUIRES utils)
