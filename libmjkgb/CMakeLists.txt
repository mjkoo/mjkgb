add_library(libmjkgb
    ./src/cpu.hpp
    ./src/gameboy.hpp
    ./src/operands.hpp

    ./src/cpu.cpp
)
set_target_properties(libmjkgb PROPERTIES PREFIX "")

