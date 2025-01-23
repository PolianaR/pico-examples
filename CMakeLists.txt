cmake_minimum_required(VERSION 3.13)

# Defina o nome do projeto
project(hello_world C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Importa o SDK do Pico
include(pico_sdk_import.cmake)

# Inicialize o SDK do Pico
pico_sdk_init()

# Adiciona o executável
add_executable(hello_world
    serial/hello_serial.c
)

# Alvo de biblioteca de SDK do Pico
target_link_libraries(hello_world pico_stdlib)

# Cria o arquivo binário
pico_add_extra_outputs(hello_world)

# Copie arquivos adicionais
pico_enable_stdio_uart(hello_world 1)
pico_enable_stdio_usb(hello_world 0)
