cmake_minimum_required(VERSION 2.8)
project(mjkgb C CXX)
enable_testing()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake"
)

include(EnableCXX11)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(EXTRA_CXX_FLAGS
        "-Werror"
        "-Weverything"
        "-Wno-shadow"
        "-Wno-unused-parameter"
        "-Wno-global-constructors"
        "-Wno-c++11-extensions"
        "-Wno-c++98-compat")
    foreach(flag ${EXTRA_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endforeach()
endif ()

add_subdirectory(libmjkgb)
add_subdirectory(tests)

include_directories(./libmjkgb/include)
add_executable(mjkgb
    ./src/main.cpp
)
target_link_libraries(mjkgb libmjkgb)

