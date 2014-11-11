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
)
set_target_properties(libmjkgb PROPERTIES PREFIX "")

