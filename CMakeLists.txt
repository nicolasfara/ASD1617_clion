cmake_minimum_required(VERSION 3.8)
project(ASD1617)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c lib1617.c lib1617.h)
add_executable(ASD1617 ${SOURCE_FILES})

target_link_libraries(ASD1617 m)