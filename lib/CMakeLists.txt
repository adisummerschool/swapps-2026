cmake_minimum_required(VERSION 3.10)

project(practica C)

add_library(ad5592r SHARED libad5592r.c)
target_link_libraries(ad5592r iio)

add_executable(main main.c)
target_link_libraries(main ad5592r)