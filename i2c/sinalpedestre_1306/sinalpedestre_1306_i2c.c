#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "sinalpedestre_1306_font.h"
#include "sinalpedestre_1306.h"

// Calcular quanto do buffer será destinado à área de renderização
void calculate_render_area_buffer_length(struct render_area *area) {
    area->buffer_length = (area->end_column - area->start_column + 1) * (area->end_page - area->start_page + 1);
}

// Processo de escrita do i2c espera um byte de controle, seguido por dados
void sinalpedestre_1306_send_command(uint8_t command) {
    uint8_t buffer[2] = {0x80, command};
    i2c_write_blocking(i2c1, sinalpedestre_1306_address, buffer, 2, false);
}

// Envia uma lista de comandos ao hardware
void sinalpedestre_1306_send_command_list(uint8_t *sinal, int number) {
    for (int i = 0; i < number; i++) {
        sinalpedestre_1306_send_command(sinal[i]);
    }
}

// Copia buffer de referência num novo buffer, a fim de adicionar o byte de controle desde o início
void sinalpedestre_1306_send_buffer(uint8_t sinal[], int buffer_length) {
    uint8_t *temp_buffer = malloc(buffer_length + 1);

    temp_buffer[0] = 0x40;
    memcpy(temp_buffer + 1, sinal, buffer_length);

    i2c_write_blocking(i2c1, sinalpedestre_1306_address, temp_buffer, buffer_length + 1, false);

    free(temp_buffer);
}

// Cria a lista de comandos (com base nos endereços definidos em sinalpedestre_1306_i2c.h) para a inicialização do display
void sinalpedestre_1306_init() {
    uint8_t commands[] = {
        sinalpedestre_1306_set_display, sinalpedestre_1306_set_memory_mode, 0x00,
        sinalpedestre_1306_set_display_start_line, sinalpedestre_1306_set_segment_remap | 0x01, 
        sinalpedestre_1306_set_mux_ratio, sinalpedestre_1306_height - 1,
        sinalpedestre_1306_set_common_output_direction | 0x08, sinalpedestre_1306_set_display_offset,
        0x00, sinalpedestre_1306_set_common_pin_configuration,
    
#if ((sinalpedestre_1306_width == 128) && (sinalpedestre_1306_height == 32))
    0x02,
#elif ((sinalpedestre_1306_width == 128) && (sinalpedestre_1306_height == 64))
    0x12,
#else
    0x02,
#endif
        sinalpedestre_1306_set_display_clock_divide_ratio, 0x80, sinalpedestre_1306_set_precharge,
        0xF1, sinalpedestre_1306_set_vcomh_deselect_level, 0x30, sinalpedestre_1306_set_contrast,
        0xFF, sinalpedestre_1306_set_entire_on, sinalpedestre_1306_set_normal_display,
        sinalpedestre_1306_set_charge_pump, 0x14, sinalpedestre_1306_set_scroll | 0x00,
        sinalpedestre_1306_set_display | 0x01,
    };

    sinalpedestre_1306_send_command_list(commands, count_of(commands));
}

// Cria a lista de comandos para configurar o scrolling
void sinalpedestre_1306_scroll(bool set) {
    uint8_t commands[] = {
        sinalpedestre_1306_set_horizontal_scroll | 0x00, 0x00, 0x00, 0x00, 0x03,
        0x00, 0xFF, sinalpedestre_1306_set_scroll | (set ? 0x01 : 0)
    };

    sinalpedestre_1306_send_command_list(commands, count_of(commands));
}

// Atualiza uma parte do display com uma área de renderização
void render_on_display(uint8_t *sinal, struct render_area *area) {
    uint8_t commands[] = {
        sinalpedestre_1306_set_column_address, area->start_column, area->end_column,
        sinalpedestre_1306_set_page_address, area->start_page, area->end_page
    };

    sinalpedestre_1306_send_command_list(commands, count_of(commands));
    sinalpedestre_1306_send_buffer(sinal, area->buffer_length);
}

// Determina o pixel a ser aceso (no display) de acordo com a coordenada fornecida
void sinalpedestre_1306_set_pixel(uint8_t *sinal, int x, int y, bool set) {
    assert(x >= 0 && x < sinalpedestre_1306_width && y >= 0 && y < sinalpedestre_1306_height);

    const int bytes_per_row = sinalpedestre_1306_width;

    int byte_idx = (y / 8) * bytes_per_row + x;
    uint8_t byte = sinal[byte_idx];

    if (set) {
        byte |= 1 << (y % 8);
    }
    else {
        byte &= ~(1 << (y % 8));
    }

    sinal[byte_idx] = byte;
}

// Algoritmo de Bresenham básico
void sinalpedestre_1306_draw_line(uint8_t *sinal, int x_0, int y_0, int x_1, int y_1, bool set) {
    int dx = abs(x_1 - x_0); // Deslocamentos
    int dy = -abs(y_1 - y_0);
    int sx = x_0 < x_1 ? 1 : -1; // Direção de avanço
    int sy = y_0 < y_1 ? 1 : -1;
    int error = dx + dy; // Erro acumulado
    int error_2;

    while (true) {
        sinalpedestre_1306_set_pixel(sinal, x_0, y_0, set); // Acende pixel no ponto atual
        if (x_0 == x_1 && y_0 == y_1) {
            break; // Verifica se o ponto final foi alcançado
        }

        error_2 = 2 * error; // Ajusta o erro acumulado

        if (error_2 >= dy) {
            error += dy;
            x_0 += sx; // Avança na direção x
        }
        if (error_2 <= dx) {
            error += dx;
            y_0 += sy; // Avança na direção y
        }
    }
}

// Adquire os pixels para um caractere (de acordo com sinalpedestre_1306_font.h)
inline int sinalpedestre_1306_get_font(uint8_t character)
{
  if (character >= 'A' && character <= 'Z') {
    return character - 'A' + 1;
  }
  else if (character >= '0' && character <= '9') {
    return character - '0' + 27;
  }
  else
    return 0;
}

// Desenha um único caractere no display
void sinalpedestre_1306_draw_char(uint8_t *sinal, int16_t x, int16_t y, uint8_t character) {
    if (x > sinalpedestre_1306_width - 8 || y > sinalpedestre_1306_height - 8) {
        return;
    }

    y = y / 8;

    character = toupper(character);
    int idx = sinalpedestre_1306_get_font(character);
    int fb_idx = y * 128 + x;

    for (int i = 0; i < 8; i++) {
        sinal[fb_idx++] = font[idx * 8 + i];
    }
}

// Desenha uma string, chamando a função de desenhar caractere várias vezes
void sinalpedestre_1306_draw_string(uint8_t *sinal, int16_t x, int16_t y, char *string) {
    if (x > sinalpedestre_1306_width - 8 || y > sinalpedestre_1306_height - 8) {
        return;
    }

    while (*string) {
        sinalpedestre_1306_draw_char(sinal, x, y, *string++);
        x += 8;
    }
}

// Comando de configuração com base na estrutura sinalpedestre_1306_t
void sinalpedestre_1306_command(sinalpedestre_1306_t *sinal, uint8_t command) {
  sinal->port_buffer[1] = command;
  i2c_write_blocking(
	sinal->i2c_port, sinal->address, sinal->port_buffer, 2, false );
}

// Função de configuração do display para o caso do bitmap
void sinalpedestre_1306_config(sinalpedestre_1306_t *sinal) {
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_display | 0x00);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_memory_mode);
    sinalpedestre_1306_command(sinal, 0x01);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_display_start_line | 0x00);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_segment_remap | 0x01);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_mux_ratio);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_height - 1);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_common_output_direction | 0x08);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_display_offset);
    sinalpedestre_1306_command(sinal, 0x00);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_common_pin_configuration);
    sinalpedestre_1306_command(sinal, 0x12);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_display_clock_divide_ratio);
    sinalpedestre_1306_command(sinal, 0x80);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_precharge);
    sinalpedestre_1306_command(sinal, 0xF1);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_vcomh_deselect_level);
    sinalpedestre_1306_command(sinal, 0x30);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_contrast);
    sinalpedestre_1306_command(sinal, 0xFF);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_entire_on);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_normal_display);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_charge_pump);
    sinalpedestre_1306_command(sinal, 0x14);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_display | 0x01);
}

// Inicializa o display para o caso de exibição de bitmap
void sinalpedestre_1306_init_bm(sinalpedestre_1306_t *sinal, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c) {
    sinal->width = width;
    sinal->height = height;
    sinal->pages = height / 8U;
    sinal->address = address;
    sinal->i2c_port = i2c;
    sinal->bufsize = sinal->pages * sinal->width + 1;
    sinal->ram_buffer = calloc(sinal->bufsize, sizeof(uint8_t));
    sinal->ram_buffer[0] = 0x40;
    sinal->port_buffer[0] = 0x80;
}

// Envia os dados ao display
void sinalpedestre_1306_send_data(sinalpedestre_1306_t *sinal) {
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_column_address);
    sinalpedestre_1306_command(sinal, 0);
    sinalpedestre_1306_command(sinal, sinal->width - 1);
    sinalpedestre_1306_command(sinal, sinalpedestre_1306_set_page_address);
    sinalpedestre_1306_command(sinal, 0);
    sinalpedestre_1306_command(sinal, sinal->pages - 1);
    i2c_write_blocking(
    sinal->i2c_port, sinal->address, sinal->ram_buffer, sinal->bufsize, false );
}

// Desenha o bitmap (a ser fornecido em display_oled.c) no display
void sinalpedestre_1306_draw_bitmap(sinalpedestre_1306_t *sinal, const uint8_t *bitmap) {
    for (int i = 0; i < sinal->bufsize - 1; i++) {
        sinal->ram_buffer[i + 1] = bitmap[i];

        sinalpedestre_1306_send_data(sinal);
    }
}