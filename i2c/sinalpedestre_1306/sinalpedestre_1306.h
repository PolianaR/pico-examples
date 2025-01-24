#include "sinalpedestre_1306.h"
extern void calculate_render_area_buffer_length(struct render_area *area);
extern void sinalpedestre_1306_send_command(uint8_t cmd);
extern void sinalpedestre_1306_send_command_list(uint8_t *sinal, int number);
extern void sinalpedestre_1306_send_buffer(uint8_t sinal[], int buffer_length);
extern void sinalpedestre_1306_init();
extern void sinalpedestre_1306_scroll(bool set);
extern void render_on_display(uint8_t *sinal, struct render_area *area);
extern void sinalpedestre_1306_set_pixel(uint8_t *sinal, int x, int y, bool set);
extern void sinalpedestre_1306_draw_line(uint8_t *sinal, int x_0, int y_0, int x_1, int y_1, bool set);
extern void sinalpedestre_1306_draw_char(uint8_t *sinal, int16_t x, int16_t y, uint8_t character);
extern void sinalpedestre_1306_draw_string(uint8_t *sinal, int16_t x, int16_t y, char *string);
extern void sinalpedestre_1306_command(sinalpedestre_1306_t *sinal, uint8_t command);
extern void sinalpedestre_1306_config(sinalpedestre_1306_t *sinal);
extern void sinalpedestre_1306_init_bm(sinalpedestre_1306_t *sinal, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c);
extern void sinalpedestre_1306_send_data(sinalpedestre_1306_t *sinal);
extern void sinalpedestre_1306_draw_bitmap(sinalpedestre_1306_t *sinal, const uint8_t *bitmap);