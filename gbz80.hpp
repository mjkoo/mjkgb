#ifndef GB_Z80_HPP_
#define GB_Z80_HPP_

#include <cstdint>

#include "operands.hpp"

template<typename T> struct get_;

class GbZ80 {
public:
    GbZ80() {
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
    uint8_t operator()(const GbZ80 &cpu, ByteRegister reg)
    {
        return cpu.regs_[0];
    }
};

template<>
struct get_<WordRegister> {
    uint16_t operator()(const GbZ80 &cpu, WordRegister reg)
    {
        return 1;
    }
};

template<>
struct get_<ConditionCode> {
    bool operator()(const GbZ80 &cpu, ConditionCode cc)
    {
        return false;
    }
};

template<typename T>
struct get_<BytePointer<T>> {
    uint8_t operator()(const GbZ80 &cpu, BytePointer<T> ptr)
    {
        return 2;
    }
};

template<typename T>
struct get_<WordPointer<T>> {
    uint16_t operator()(const GbZ80 &cpu, WordPointer<T> ptr)
    {
        return 3;
    }
};

template<typename T>
struct get_<Immediate<T>> {
    T operator()(const GbZ80 &cpu, Immediate<T> imm)
    {
        return 4;
    }
};

#endif /* GB_Z80_HPP_ */

