# Defina o caminho do SDK
set(PICO_SDK_PATH "/mnt/c/Users/3p/Documents/ProjetoRasp/pico-sdk")

# Incluir o arquivo pico_sdk_import.cmake para importar o SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável
add_executable(blink_custom
        blink_custom.c
        ssd1306.c 
        ssd1306_i2c.c
)

# Define os diretórios de inclusão
target_include_directories(blink_custom PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/inc  # Inclui a pasta 'inc' para arquivos de cabeçalho
)

# Vincula as bibliotecas necessárias
target_link_libraries(blink_custom 
        pico_stdlib
        hardware_i2c
)

# Cria os arquivos binários, hex, uf2, etc.
pico_add_extra_outputs(blink_custom)

# Configura o URL do exemplo no GitHub (opcional)
example_auto_set_url(blink_custom)
