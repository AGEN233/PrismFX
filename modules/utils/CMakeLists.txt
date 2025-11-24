
set(requires
    "esp_timer"
)


idf_component_register(
    SRCS ""
    INCLUDE_DIRS "."
    REQUIRES ${requires}
)