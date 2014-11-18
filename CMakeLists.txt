cmake_minimum_required(VERSION 2.8)
project(mjkgb C CXX)
enable_testing()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake"
)

include(EnableCXX11)

# Turn off a lot of warnings, some are overly pedantic, some are valid but flag LLVM code
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(EXTRA_CXX_FLAGS
        "-Werror"
        "-Weverything"
        "-Wno-conversion"
        "-Wno-padded"
        "-Wno-shadow"
        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-label"
        "-Wno-unused-value"
        "-Wno-unused-variable"
        "-Wno-global-constructors"
        "-Wno-missing-noreturn"
        "-Wno-missing-prototypes"
        "-Wno-shorten-64-to-32"
        "-Wno-switch-enum"
        "-Wno-c++11-extensions"
        "-Wno-c++98-compat"
        "-Wno-c++98-compat-pedantic"
        "-Wno-shift-sign-overflow"
        "-Wno-sign-conversion"
        "-Wno-gnu-label-as-value")
    foreach(flag ${EXTRA_CXX_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endforeach()
else()
    message(WARNING "C++ compiler is not clang, unsupported configuration")
endif()

add_subdirectory(libmjkgb)
add_subdirectory(tests)

include_directories(./libmjkgb/include)
add_executable(mjkgb
    ./src/main.cpp
)
target_link_libraries(mjkgb libmjkgb)

