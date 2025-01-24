add_executable(sinalpedestre_1306
    sinalpedestre_1306_i2c.c
    display_oled_t6.c
)

target_link_libraries(sinalpedestre_1306 pico_stdlib)

pico_add_extra_outputs(sinalpedestre_1306)

