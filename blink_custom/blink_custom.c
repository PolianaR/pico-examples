#include <stdio.h>
#include <stdbool.h>   // Para o tipo bool
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306_font.h"

// Definições do display OLED
#define SSD1306_BUFFER_LENGTH 1024
#define COUNT_OF(arr) (sizeof(arr) / sizeof(arr[0]))

// Pinos do I2C
const unsigned int I2C_SDA = 14;
const unsigned int I2C_SCL = 15;

// Funções que exibem mensagens no display OLED
void exibir_mensagem(const char* mensagem) {
    uint8_t ssd[SSD1306_BUFFER_LENGTH];
    memset(ssd, 0, SSD1306_BUFFER_LENGTH);  // Limpa a tela
    ssd1306_draw_string(ssd, 5, 0, mensagem);  // Exibe a mensagem
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    render_on_display(ssd, &frame_area);  // Atualiza o display
}

// Função para o sinal verde
void SinalAberto() {
    exibir_mensagem("SINAL ABERTO - ATRAVESSAR COM CUIDADO");
}

// Função para o sinal amarelo
void SinalAtencao() {
    exibir_mensagem("SINAL DE ATENCAO - PREPARE-SE");
}

// Função para o sinal vermelho
void SinalFechado() {
    exibir_mensagem("SINAL FECHADO - AGUARDE");
}

int main() {
    // Inicialização da UART para depuração (opcional)
    stdio_init_all();

    // Inicialização do barramento I2C
    i2c_init(i2c1, 400 * 1000); // Frequência de 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização do SSD1306
    if (ssd1306_init() != 0) {
        printf("Erro ao inicializar o display SSD1306!\n");
        return -1;  // Se falhar, retorna erro
    }

    // Exemplo de lógica do semáforo
    while (true) {
        // Simulação de alternância dos sinais
        SinalAberto();  // Verde
        sleep_ms(5000); // Espera 5 segundos

        SinalAtencao();  // Amarelo
        sleep_ms(2000); // Espera 2 segundos

        SinalFechado();  // Vermelho
        sleep_ms(5000); // Espera 5 segundos

        // Função para evitar bloqueio do processador
        tight_loop_contents();
    }

    return 0;
}
