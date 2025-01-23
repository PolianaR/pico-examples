add_executable(blink_custom
        blink_custom.c
        ssd1306.c 
        ssd1306_i2c.c
)

# pull in common dependencies
target_link_libraries(blink_custom 
        pico_stdlib
        hardware_i2c     # Biblioteca para comunicação I2C

)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(blink_custom)

# call pico_set_program_url to set path to example on github, so users can find the source for an example via picotool
example_auto_set_url(blink_custom)
