cmake_minimum_required(VERSION 3.5.0)
project(raylife VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_COMPILER "gcc")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lraylib -lGL -lm -lpthread -ldl -lrt -lX11")

add_executable(raylife raylife.c)
target_link_libraries(raylife raylib)
