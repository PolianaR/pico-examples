#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#ifndef sinalpedestre_1306_h
#define sinalpedestre_1306_h

#define sinalpedestre_1306_height 64 // Define a altura do display (32 pixels)
#define sinalpedestre_1306width 128 // Define a largura do display (128 pixels)

#define sinalpedestre_1306_i2c_address _u(0x3C) // Define o endereço do i2c do display

#define sinalpedestre_1306_i2c_clock 400 // Define o tempo do clock (pode ser aumentado)

// Comandos de configuração (endereços)
#define sinalpedestre_1306_set_memory_mode _u(0x20)
#define sinalpedestre_1306_set_column_address _u(0x21)
#define sinalpedestre_1306_set_page_address _u(0x22)
#define sinalpedestre_1306_set_horizontal_scroll _u(0x26)
#define sinalpedestre_1306_set_scroll _u(0x2E)

#define sinalpedestre_1306_set_display_start_line _u(0x40)

#define sinalpedestre_1306_set_contrast _u(0x81)
#define sinalpedestre_1306_set_charge_pump _u(0x8D)

#define sinalpedestre_1306_set_segment_remap _u(0xA0)
#define sinalpedestre_1306_set_entire_on _u(0xA4)
#define sinalpedestre_1306_set_all_on _u(0xA5)
#define sinalpedestre_1306_set_normal_display _u(0xA6)
#define sinalpedestre_1306_set_inverse_display _u(0xA7)
#define sinalpedestre_1306_set_mux_ratio _u(0xA8)
#define sinalpedestre_1306_set_display _u(0xAE)
#define sinalpedestre_1306_set_common_output_direction _u(0xC0)
#define sinalpedestre_1306_set_common_output_direction_flip _u(0xC0)

#define sinalpedestre_1306_set_display_offset _u(0xD3)
#define sinalpedestre_1306_set_display_clock_divide_ratio _u(0xD5)
#define sinalpedestre_1306_set_precharge _u(0xD9)
#define sinalpedestre_1306_set_common_pin_configuration _u(0xDA)
#define sinalpedestre_1306_set_vcomh_deselect_level _u(0xDB)

#define sinalpedestre_1306_page_height _u(8)
#define sinalpedestre_1306_n_pages (sinalpedestre_1306_height / sinalpedestre_1306_page_height)
#define sinalpedestre_1306_buffer_length (sinalpedestre_1306_n_pages * sinalpedestre_1306_width)

#define sinalpedestre_1306_write_mode _u(0xFE)
#define sinalpedestre_1306_read_mode _u(0xFF)

struct render_area {
    uint8_t start_column;
    uint8_t end_column;
    uint8_t start_page;
    uint8_t end_page;

    int buffer_length;
};

typedef struct {
  uint8_t width, height, pages, address;
  i2c_inst_t * i2c_port;
  bool external_vcc;
  uint8_t *ram_buffer;
  size_t bufsize;
  uint8_t port_buffer[2];
} sinalpedestre_1306_t;

#endif