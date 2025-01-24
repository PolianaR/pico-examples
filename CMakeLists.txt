cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0057 NEW)
set(CMAKE_MAKE_PROGRAM /usr/bin/make)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Configurações específicas para ARM
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-as)

set(CMAKE_C_FLAGS "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_CXX_FLAGS "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_EXE_LINKER_FLAGS "")

# Importa o SDK do Pico
include(pico_sdk_import.cmake)

# Inicialize o SDK do Pico
pico_sdk_init()

# Adiciona o executável
add_executable(ProjetoRasp
    # Lista todos os arquivos .c necessários
    # serial/hello_serial.c
)

# Alvo de biblioteca de SDK do Pico
target_link_libraries(ProjetoRasp pico_stdlib)

# Cria o arquivo binário
pico_add_extra_outputs(ProjetoRasp)

# Configura I/O
pico_enable_stdio_uart(ProjetoRasp 1)
pico_enable_stdio_usb(ProjetoRasp 0)

# Adiciona a nova pasta sinalpedestre_1306
add_subdirectory(i2c/sinalpedestre_1306)
