find_program(CLANG_EXECUTABLE
    NAMES clang
)

if (CLANG_EXECUTABLE)
    message(STATUS "Found clang: ${CLANG_EXECUTABLE}")
else()
    message(FATAL_ERROR "Cound not find clang")
endif()

# Use clang to automatically generate llvm bytecode for all opcodes
# See https://stackoverflow.com/questions/14776463/compile-and-add-object-file-from-binary-with-cmake
add_custom_command(OUTPUT opcodes.o
    COMMAND ${CLANG_EXECUTABLE} -std=c++11 -emit-llvm -o opcodes.bc
        -I ${CMAKE_CURRENT_SOURCE_DIR}/include
        -c ${CMAKE_CURRENT_SOURCE_DIR}/src/opcodes.cpp
    COMMAND ld -r -b binary -o opcodes.o opcodes.bc
    COMMAND objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents
        ${CMAKE_CURRENT_BINARY_DIR}/opcodes.o ${CMAKE_CURRENT_BINARY_DIR}/opcodes.o
)
add_library(opcodes STATIC opcodes.o)
set_source_files_properties(opcodes.o
    PROPERTIES
    EXTERNAL_OBJECT true
    GENERATED true
)
set_target_properties(opcodes
    PROPERTIES
    LINKER_LANGUAGE C
)

include_directories(./include)
add_library(libmjkgb
    ./include/mjkgb.hpp

    ./src/cpu.hpp
    ./src/mmu.hpp
    ./src/operands.hpp

    ./src/cpu.cpp
    ./src/mmu.cpp
    ./src/gameboy.cpp
    ./src/gameboy_impl.cpp
    ./src/opcodes.cpp
)
target_link_libraries(libmjkgb opcodes)
set_target_properties(libmjkgb PROPERTIES PREFIX "")

