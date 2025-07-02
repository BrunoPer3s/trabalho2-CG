cmake_minimum_required(VERSION 3.26)
project(trabalho2_CG C)

set(CMAKE_C_STANDARD 11)

# Adiciona os diretórios com os .h
include_directories(include)

# Encontra SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# Adiciona os arquivos fonte
set(SOURCES
        src/main.c
        src/algebra.c
        src/objeto.c
        src/tela.c
        src/camera.c
)

add_executable(trabalho2_CG ${SOURCES})

# Liga SDL2 e outras libs
target_link_libraries(trabalho2_CG ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 m)
