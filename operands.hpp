#ifndef OPERANDS_HPP_
#define OPERANDS_HPP_

#include <type_traits>

enum class ByteRegister {
    A, F, B, C, D, E, H, L
};

enum class WordRegister {
    AF, BC, DE, HL, PC, SP
};

enum class ConditionCode {
    Z, H, N, C
};

template<typename T>
struct BytePointer {
    T value;
    explicit BytePointer(T value)
      : value(value)
    { }
};

template<typename T>
struct WordPointer {
    T value;
    explicit WordPointer(T value)
      : value(value)
    { }
};

template<typename T>
struct Immediate {
    static_assert(std::is_integral<T>::value,
            "Non-integral immediate value");
    T value;
    explicit Immediate(T value)
      : value(value)
    { }
};

#endif /* OPERANDS_HPP_ */

