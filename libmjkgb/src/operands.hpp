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
    Z, N, H, C, NZ, NC, UNCONDITIONAL
};

template<int value> struct Constant { };

template<typename T> struct Ignore {
    T value;
    explicit Ignore(T v)
      : value(v)
    { }
};

template<typename T, int inc = 0, int off = 0>
struct BytePointer {
    T value;
    explicit BytePointer(T v)
      : value(v)
    { }
};

template<int inc = 0, int off = 0, typename T>
BytePointer<T, inc, off> byte_ptr(T value)
{
    return BytePointer<T, inc, off>{value};
}

template<typename T, int inc = 0, int off = 0>
struct WordPointer {
    T value;
    explicit WordPointer(T v)
      : value(v)
    { }
};

template<int inc = 0, int off = 0, typename T>
WordPointer<T, inc, off> word_ptr(T value)
{
    return WordPointer<T, inc, off>{value};
}

struct ByteImmediate { };
struct WordImmediate { };

}

#endif /* OPERANDS_HPP_ */

