cmake_minimum_required(VERSION 3.17)
project(laboratories_1st_part C)

set(CMAKE_C_STANDARD 11)

set(LAB2_SOURCE laboratory-2/main.c)
set(LAB3_SOURCE laboratory-3/main.c)
set(LAB4_SOURCE laboratory-4/main.c laboratory-4/list.c)

add_executable(laboratory-2 ${LAB2_SOURCE} )
add_executable(laboratory-3 ${LAB3_SOURCE} )
add_executable(laboratory-4 ${LAB4_SOURCE} )