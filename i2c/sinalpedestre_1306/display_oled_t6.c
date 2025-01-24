#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "sinalpedestre_1306.h" // Biblioteca para controle do OLED
#include "hardware/i2c.h"

// Definições dos pinos
#define I2C_SDA_PIN 4 
#define I2C_SCL_PIN 5 
#define LED_R_PIN 6
#define LED_G_PIN 7
#define BTN_A_PIN 8
#define BTN_B_PIN 9

// Configuração do display OLED 
#define OLED_WIDTH 128 
#define OLED_HEIGHT 32 
sinalpedestre_1306_t oled; 

// Funções de inicialização do OLED
void init_oled() { 
    i2c_init(i2c_default, 100 * 1000); 
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA_PIN); 
    gpio_pull_up(I2C_SCL_PIN); 
    sinalpedestre_1306_init(&oled, i2c_default, 0x3C, OLED_WIDTH, OLED_HEIGHT); 
    sinalpedestre_1306_clear(&oled); 
    sinalpedestre_1306_show(&oled); 
} 

void display_message(const char* message) { 
    sinalpedestre_1306_clear(&oled); 
    sinalpedestre_1306_draw_string(&oled, 0, 0, message, 1); 
    sinalpedestre_1306_show(&oled); 
}

// Funções de controle do semáforo 
void SinalAberto() { 
    gpio_put(LED_R_PIN, 0); 
    gpio_put(LED_G_PIN, 1); 
    display_message("SINAL ABERTO - ATRAVESSAR COM CUIDADO"); 
} 

void SinalFechado() { 
    gpio_put(LED_R_PIN, 1); 
    gpio_put(LED_G_PIN, 0); 
    display_message("SINAL FECHADO - AGUARDE"); 
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
    init_oled(); 

    while (true) {
        SinalFechado(); 
        if (WaitWithRead(13000)) { // Sinal vermelho por 13 segundos
            display_message("Aperte o Botão A para Sinal Verde");
            sleep_ms(5000); // Espera 5 segundos para dar tempo do amarelo dos carros
            SinalAberto(); 
            sleep_ms(8000); // Sinal verde por 8 segundos
        } else {
            sleep_ms(13000); // Sinal vermelho por 13 segundos
            display_message("Aperte o Botão A para Sinal Verde");
            sleep_ms(5000); // Espera 5 segundos para dar tempo do amarelo dos carros
            SinalAberto();
            sleep_ms(8000); // Sinal verde por 8 segundos
        }

        if (!gpio_get(BTN_B_PIN)) { // Botão B pressionado
            SinalFechado();
        }
    }

    return 0;
}
