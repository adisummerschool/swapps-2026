cmake_minimum_required(VERSION 3.10)
project(practica C)

add_library(func SHARED libfunc.c)
target_link_libraries(func iio)

add_executable(main main.c)
target_link_libraries(main func)

