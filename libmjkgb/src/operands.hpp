#ifndef OPERANDS_HPP_
#define OPERANDS_HPP_

namespace mjkgb {

enum class ByteRegister {
    A, F, B, C, D, E, H, L
};

enum class WordRegister {
    AF, BC, DE, HL, PC, SP
};

enum class ConditionCode {
    Z, H, N, C
};

template<typename T, bool inc = false>
struct BytePointer {
    T value;
    explicit BytePointer(T value)
      : value(value)
    { }
};

template<typename T, bool inc = false>
struct WordPointer {
    T value;
    explicit WordPointer(T value)
      : value(value)
    { }
};

struct ByteImmediate { };
struct WordImmediate { };

}

#endif /* OPERANDS_HPP_ */

