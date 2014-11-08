cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake"
)

project(mjkgb CXX)

include(EnableCXX11)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(EXTRA_CXX_FLAGS
        "-Werror"
        "-Weverything"
        "-Wno-shadow"
        "-Wno-unused-parameter"
        "-Wno-c++11-extensions"
        "-Wno-c++98-compat")
    foreach(flag ${EXTRA_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endforeach()
endif ()

add_executable(mjkgb
    ./src/cpu.hpp
    ./src/cpu.cpp
    ./src/gameboy.hpp
    ./src/operands.hpp
    ./src/main.cpp
)

