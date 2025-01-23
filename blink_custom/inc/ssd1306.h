#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// Funções do driver SSD1306
void ssd1306_init(void);
void ssd1306_draw_string(uint8_t *buffer, uint8_t x, uint8_t y, const char *str);
void render_on_display(uint8_t *buffer, struct render_area *frame_area);

#endif

