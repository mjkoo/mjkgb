#ifndef GAMEBOY_HPP_
#define GAMEBOY_HPP_

#include <cstdint>

#include "operands.hpp"

template<typename T> struct get_;

class Gameboy {
public:
    Gameboy() {
        regs_[0] = 0xfe;
    }

    template<typename T>
    typename operand_type<T>::type get(T operand)
    {
        return get_<T>()(*this, operand);
    }

    uint8_t regs_[8];
};

template<>
struct get_<ByteRegister> {
    uint8_t operator()(const Gameboy &gb, ByteRegister reg)
    {
        return gb.regs_[0];
    }
};

template<>
struct get_<WordRegister> {
    uint16_t operator()(const Gameboy &gb, WordRegister reg)
    {
        return 1;
    }
};

template<>
struct get_<ConditionCode> {
    bool operator()(const Gameboy &gb, ConditionCode cc)
    {
        return false;
    }
};

template<typename T>
struct get_<BytePointer<T>> {
    uint8_t operator()(const Gameboy &gb, BytePointer<T> ptr)
    {
        return 2;
    }
};

template<typename T>
struct get_<WordPointer<T>> {
    uint16_t operator()(const Gameboy &gb, WordPointer<T> ptr)
    {
        return 3;
    }
};

template<typename T>
struct get_<Immediate<T>> {
    T operator()(const Gameboy &gb, Immediate<T> imm)
    {
        return 4;
    }
};

#endif /* GAMEBOY_HPP_ */

