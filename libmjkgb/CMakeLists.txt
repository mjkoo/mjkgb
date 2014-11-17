find_program(CLANG_EXECUTABLE
    NAMES clang
)

if(CLANG_EXECUTABLE)
    message(STATUS "Found clang: ${CLANG_EXECUTABLE}")
else()
    message(FATAL_ERROR "Cound not find clang")
endif()

# Use clang to automatically generate llvm biecode for all opcodes
# See https://stackoverflow.com/questions/14776463/compile-and-add-object-file-from-binary-with-cmake
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(EMIT_LLVM_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(EMIT_LLVM_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    set(EMIT_LLVM_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    set(EMIT_LLVM_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_MINSIZEREL}")
else()
    set(EMIT_LLVM_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
string(REPLACE " " ";" EMIT_LLVM_CXX_FLAGS ${EMIT_LLVM_CXX_FLAGS})

add_custom_command(OUTPUT opcodes.o
    COMMAND ${CLANG_EXECUTABLE} ${EMIT_LLVM_CXX_FLAGS} -DEMIT_LLVM -emit-llvm -o opcodes.bc
        -I ${CMAKE_CURRENT_SOURCE_DIR}/include
        -c ${CMAKE_CURRENT_SOURCE_DIR}/src/opcodes.cpp
    COMMAND ld -r -b binary -o opcodes.o opcodes.bc
    COMMAND objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents
        ${CMAKE_CURRENT_BINARY_DIR}/opcodes.o ${CMAKE_CURRENT_BINARY_DIR}/opcodes.o
    IMPLICIT_DEPENDS CXX ./src/opcodes.cpp
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

