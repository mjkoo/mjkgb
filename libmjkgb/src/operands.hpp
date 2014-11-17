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
    Z, N, H, C, UNCONDITIONAL
};

template<int value> struct Constant { };

template<typename T> struct Ignore {
    T value;
    explicit Ignore(T value)
      : value(value)
    { }
};


template<typename T, int inc = 0, int off = 0>
struct BytePointer {
    T value;
    explicit BytePointer(T value)
      : value(value)
    { }
};

template<typename T, int inc = 0, int off = 0>
struct WordPointer {
    T value;
    explicit WordPointer(T value)
      : value(value)
    { }
};

struct ByteImmediate { };
struct WordImmediate { };
struct Displacement { };

}

#endif /* OPERANDS_HPP_ */

