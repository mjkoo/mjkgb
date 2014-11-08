cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake"
)

project(mjkgb)

include(EnableCXX11)

add_executable(mjkgb
    ./src/cpu.hpp
    ./src/gameboy.hpp
    ./src/operands.hpp
    ./src/main.cpp
)

