cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(blink_custom)

pico_sdk_init()

add_executable(blink_custom
    blink_custom.c
    ssd1306.c 
    ssd1306_i2c.c
)

target_include_directories(blink_custom PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

target_link_libraries(blink_custom 
    pico_stdlib
    hardware_i2c
)

pico_add_extra_outputs(blink_custom)

example_auto_set_url(blink_custom)
