cmake_minimum_required(VERSION 3.5)
project(nyufile)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-g -pedantic -std=gnu17 -fsigned-char)
add_link_options(-lcrypto)

add_executable(nyufile nyufile.c utils.c utils.h)
