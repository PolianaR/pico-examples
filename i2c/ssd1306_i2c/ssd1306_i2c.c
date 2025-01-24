/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "ssd1306_font.h"

// Definições dos pinos
#define I2C_SDA_PIN 4 
#define I2C_SCL_PIN 5 
#define LED_R_PIN 6
#define LED_G_PIN 7
#define BTN_A_PIN 8
#define BTN_B_PIN 9

// Configuração do display OLED 
#define SSD1306_HEIGHT 32 
#define SSD1306_WIDTH 128 
#define SSD1306_I2C_ADDR _u(0x3C) 
#define SSD1306_I2C_CLK 400

// Definições de comandos para o SSD1306
#define SSD1306_SET_MEM_MODE _u(0x20)
#define SSD1306_SET_COL_ADDR _u(0x21)
#define SSD1306_SET_PAGE_ADDR _u(0x22)
#define SSD1306_SET_DISP_START_LINE _u(0x40)
#define SSD1306_SET_CONTRAST _u(0x81)
#define SSD1306_SET_CHARGE_PUMP _u(0x8D)
#define SSD1306_SET_SEG_REMAP _u(0xA0)
#define SSD1306_SET_ENTIRE_ON _u(0xA4)
#define SSD1306_SET_NORM_DISP _u(0xA6)
#define SSD1306_SET_DISP _u(0xAE)
#define SSD1306_SET_COM_OUT_DIR _u(0xC0)
#define SSD1306_SET_DISP_OFFSET _u(0xD3)
#define SSD1306_SET_DISP_CLK_DIV _u(0xD5)
#define SSD1306_SET_PRECHARGE _u(0xD9)
#define SSD1306_SET_COM_PIN_CFG _u(0xDA)
#define SSD1306_SET_VCOM_DESEL _u(0xDB)

// Struct para o display OLED
typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t address;
    uint8_t *buffer;
} ssd1306_t;

// Buffer do display OLED
uint8_t oled_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Função para enviar comandos ao SSD1306
void SSD1306_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, buf, 2, false);
}

// Função para enviar o buffer ao SSD1306
void SSD1306_send_buffer(ssd1306_t *oled) {
    uint8_t temp_buf[SSD1306_WIDTH * SSD1306_HEIGHT / 8 + 1];
    temp_buf[0] = 0x40;
    memcpy(temp_buf + 1, oled->buffer, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
    i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, temp_buf, sizeof(temp_buf), false);
}

// Função para inicializar o SSD1306
void SSD1306_init(ssd1306_t *oled) {
    oled->width = SSD1306_WIDTH;
    oled->height = SSD1306_HEIGHT;
    oled->address = SSD1306_I2C_ADDR;
    oled->buffer = oled_buffer;

    // Inicialização do SSD1306
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    uint8_t cmds[] = {
        SSD1306_SET_DISP, // display off
        SSD1306_SET_MEM_MODE, 0x00, // horizontal addressing mode
        SSD1306_SET_DISP_START_LINE, // set display start line to 0
        SSD1306_SET_SEG_REMAP | 0x01, // set segment re-map
        SSD1306_SET_MUX_RATIO, SSD1306_HEIGHT - 1, // set multiplex ratio
        SSD1306_SET_COM_OUT_DIR | 0x08, // scan direction
        SSD1306_SET_DISP_OFFSET, 0x00, // no offset
        SSD1306_SET_COM_PIN_CFG, 0x02, // com pin config
        SSD1306_SET_DISP_CLK_DIV, 0x80, // display clock divide ratio
        SSD1306_SET_PRECHARGE, 0xF1, // pre-charge period
        SSD1306_SET_VCOM_DESEL, 0x30, // VCOMH deselect level
        SSD1306_SET_CONTRAST, 0xFF, // contrast control
        SSD1306_SET_ENTIRE_ON, // display follows RAM content
        SSD1306_SET_NORM_DISP, // normal display
        SSD1306_SET_CHARGE_PUMP, 0x14, // charge pump
        SSD1306_SET_DISP | 0x01 // turn display on
    };

    for (size_t i = 0; i < sizeof(cmds); i++) {
        SSD1306_send_cmd(cmds[i]);
    }
}

// Função para limpar o buffer do OLED
void SSD1306_clear(ssd1306_t *oled) {
    memset(oled->buffer, 0, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
}

// Função para exibir uma mensagem no OLED
void display_message(ssd1306_t *oled, const char *message) {
    SSD1306_clear(oled);
    // Esta função precisa de uma implementação de desenho de texto, que pode ser complexa.
    // Para simplificação, vou adicionar uma função fictícia que simplesmente copia a mensagem para o buffer.
    strncpy((char *)oled->buffer, message, sizeof(oled_buffer) - 1);
    SSD1306_send_buffer(oled);
}

// Funções de controle do semáforo 
void SinalAberto(ssd1306_t *oled) { 
    gpio_put(LED_R_PIN, 0); 
    gpio_put(LED_G_PIN, 1); 
    display_message(oled, "SINAL ABERTO - ATRAVESSAR COM CUIDADO"); 
} 

void SinalFechado(ssd1306_t *oled) { 
    gpio_put(LED_R_PIN, 1); 
    gpio_put(LED_G_PIN, 0); 
    display_message(oled, "SINAL FECHADO - AGUARDE"); 
} 

int WaitWithRead(int timeMS) {
    int startTime = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - startTime < timeMS) {
        if (!gpio_get(BTN_A_PIN)) { // Botão A pressionado
            return 1;
        }
        sleep_ms(10); // Evita sobrecarregar a CPU
    }
    return 0;
}

int main() {
    stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário

    ssd1306_t oled; // Struct do OLED

    // Inicialização de LEDs e Botões 
    gpio_init(LED_R_PIN); 
    gpio_set_dir(LED_R_PIN, GPIO_OUT); 
    gpio_init(LED_G_PIN); 
    gpio_set_dir(LED_G_PIN, GPIO_OUT); 

    gpio_init(BTN_A_PIN); 
    gpio_set_dir(BTN_A_PIN, GPIO_IN); 
    gpio_pull_up(BTN_A_PIN); 

    gpio_init(BTN_B_PIN); 
    gpio_set_dir(BTN_B_PIN, GPIO_IN); 
    gpio_pull_up(BTN_B_PIN);

    // Inicialização do OLED 
    SSD1306_init(&oled); 

    while (true) {
        SinalFechado(&oled); 
        if (WaitWithRead(13000)) { // Sinal vermelho por 13 segundos
            display_message(&oled, "Aperte o Botão A para Sinal Verde");
            sleep_ms(5000); // Espera 5 segundos para dar tempo do amarelo dos carros
            SinalAberto(&oled); 
            sleep_ms(8000); // Sinal verde por 8 segundos
        } else {
            sleep_ms(13000); // Sinal vermelho por 13 segundos
            display_message(&oled, "Aperte o Botão A para Sinal Verde");
            sleep_ms(5000); // Espera 5 segundos para dar tempo do amarelo dos carros
            SinalAberto(&oled);
            sleep_ms(8000); // Sinal verde por 8 segundos
        }

        if (!gpio_get(BTN_B_PIN)) { // Botão B pressionado
            SinalFechado(&oled);
        }
    }

    return 0;
}
