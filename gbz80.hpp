#ifndef GB_Z80_HPP_
#define GB_Z80_HPP_

#include <cstdint>

#include "operands.hpp"

class GbZ80 {
public:
    GbZ80() { }

    template<typename T>
    typename T::value_type get(T operand) {
        return get(*this, operand);
    }
};

namespace {
    template<typename T>
    struct get;

    template<>
    struct get<ByteRegister> {
        uint8_t operator()(const GbZ80 &cpu, ByteRegister reg)
        {
            return 0;
        }
    };

    template<>
    struct get<WordRegister> {
        uint16_t operator()(const GbZ80 &cpu, WordRegister reg)
        {
            return 1;
        }
    };

    template<>
    struct get<ConditionCode> {
        bool operator()(const GbZ80 &cpu, ConditionCode cc)
        {
            return false;
        }
    };

    template<typename T>
    struct get<BytePointer<T>> {
        uint8_t operator()(const GbZ80 &cpu, BytePointer<T> ptr)
        {
            return 2;
        }
    };

    template<typename T>
    struct get<WordPointer<T>> {
        uint16_t operator()(const GbZ80 &cpu, WordPointer<T> ptr)
        {
            return 3;
        }
    };

    template<typename T>
    struct get<Immediate<T>> {
        T operator()(const GbZ80 &cpu, Immediate<T> imm)
        {
            return 4;
        }
    };
}

#endif /* GB_Z80_HPP_ */

