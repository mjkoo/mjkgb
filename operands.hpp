#ifndef OPERANDS_HPP_
#define OPERANDS_HPP_

#include <type_traits>

struct ByteRegister {
    typedef uint8_t value_type;
    enum {
        A, F, B, C, D, E, H, L
    };
};

struct WordRegister {
    typedef uint16_t value_type;
    enum {
        AF, BC, DE, HL, PC, SP
    };
};

struct ConditionCode {
    typedef bool value_type;
    enum {
        Z, H, N, C
    };
};

template<typename T>
struct BytePointer {
    typedef uint8_t value_type;

    T value_;
    explicit BytePointer(T value)
      : value_(value) { }
};

template<typename T>
struct WordPointer {
    typedef uint16_t value_type;

    T value_;
    explicit WordPointer(T value)
      : value_(value) { }
};

template<typename T>
struct Immediate {
    static_assert(std::is_integral<T>::value,
            "Non-integral immediate value");
    typedef T value_type;

    T value_;
    explicit Immediate(T value)
      : value_(value) { }
};

#endif /* OPERANDS_HPP_ */

