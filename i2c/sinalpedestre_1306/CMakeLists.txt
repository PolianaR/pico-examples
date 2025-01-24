add_executable(sinalpedestre_1306
            sinalpedestre_1306.c 
)
target_link_libraries(sinalpedestre_1306 pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(sinalpedestre_1306)

# add url via pico_set_program_url
example_auto_set_url(sinalpedestre_1306)