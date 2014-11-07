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
    T value_;
    explicit BytePointer(T value)
      : value_(value) { }
};

template<typename T>
struct WordPointer {
    T value_;
    explicit WordPointer(T value)
      : value_(value) { }
};

template<typename T>
struct Immediate {
    static_assert(std::is_integral<T>::value,
            "Non-integral immediate value");
    T value_;
    explicit Immediate(T value)
      : value_(value) { }
};

template<typename T> struct operand_type;
template<> struct operand_type<ByteRegister> { typedef uint8_t type; };
template<> struct operand_type<WordRegister> { typedef uint16_t type; };
template<> struct operand_type<ConditionCode> { typedef bool type; };
template<typename T> struct operand_type<BytePointer<T>> { typedef uint8_t type; };
template<typename T> struct operand_type<WordPointer<T>> { typedef uint16_t type; };
template<typename T> struct operand_type<Immediate<T>> { typedef T type; };

#endif /* OPERANDS_HPP_ */

