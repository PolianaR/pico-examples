# Defina o nome do projeto
project(sinalpedestre_1306 C CXX ASM)

add_executable(sinalpedestre_1306
    main.c
    # Adicione outros arquivos .c necessários aqui
)

# Link com as bibliotecas padrão do Pico
target_link_libraries(sinalpedestre_1306 pico_stdlib)

# Cria o arquivo binário
pico_add_extra_outputs(sinalpedestre_1306)

# Configura I/O
pico_enable_stdio_uart(sinalpedestre_1306 1)
pico_enable_stdio_usb(sinalpedestre_1306 0)
