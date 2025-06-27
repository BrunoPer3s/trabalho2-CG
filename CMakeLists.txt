cmake_minimum_required(VERSION 3.26)
project(trabalho2_CG C)

include_directories(${SDL2_INCLUDE_DIR})
find_package(SDL2 REQUIRED)

set(CMAKE_C_STANDARD 11)

add_executable(trabalho2_CG main.c
        algebra.c
        algebra.h
        objeto.c
        objeto.h
        tela.c
        tela.h
        camera.c
        camera.h
)

target_link_libraries(trabalho2_CG ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 m)
