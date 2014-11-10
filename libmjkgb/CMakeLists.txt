include_directories(./include)
add_library(libmjkgb
    ./src/accessors.hpp
    ./src/cpu.hpp
    ./src/operands.hpp

    ./src/cpu.cpp
    ./src/gameboy.cpp
)
set_target_properties(libmjkgb PROPERTIES PREFIX "")

