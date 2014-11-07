#ifndef GAMEBOY_HPP_
#define GAMEBOY_HPP_

#include <cstdint>

#include "operands.hpp"
#include "cpu.hpp"

template<typename T> struct accessor;

class Gameboy {
public:
    Gameboy()
      : cpu_()
    { }


    template<typename T>
    typename accessor<T>::value_type get(T operand) const
    {
        return accessor<T>().get(*this, operand);
    }

    template<typename T>
    void set(T operand, typename accessor<T>::value_type value)
    {
        accessor<T>().set(*this, operand, value);
    }

    Cpu &cpu()
    {
        return cpu_;
    }

    const Cpu &cpu() const
    {
        return cpu_;
    }

private:
    Cpu cpu_;
};

template<>
struct accessor<ByteRegister> {
    typedef uint8_t value_type;

    value_type get(const Gameboy &gb, ByteRegister reg) const
    {
        return gb.cpu().registers[0];
    }

    void set(Gameboy &gb, ByteRegister reg, value_type value) const
    {

    }
};

template<>
struct accessor<WordRegister> {
    typedef uint16_t value_type;

    value_type get(const Gameboy &gb, WordRegister reg) const
    {
        return 1;
    }

    void set(Gameboy &gb, WordRegister reg, value_type value) const
    {

    }
};

template<>
struct accessor<ConditionCode> {
    typedef bool value_type;

    value_type get(const Gameboy &gb, ConditionCode cc) const
    {
        return false;
    }

    void set(Gameboy &gb, ConditionCode cc, value_type value) const
    {

    }
};

template<typename T>
struct accessor<BytePointer<T>> {
    typedef uint8_t value_type;

    value_type get(const Gameboy &gb, BytePointer<T> ptr) const
    {
        return 2;
    }

    void set(Gameboy &gb, BytePointer<T> ptr, value_type value) const
    {

    }
};

template<typename T>
struct accessor<WordPointer<T>> {
    typedef uint16_t value_type;

    value_type get(const Gameboy &gb, WordPointer<T> ptr) const
    {
        return 3;
    }

    void set(Gameboy &gb, WordPointer<T> ptr, value_type value) const
    {

    }
};

template<typename T>
struct accessor<Immediate<T>> {
    typedef T value_type;

    value_type get(const Gameboy &gb, Immediate<T> imm) const
    {
        return 4;
    }

    void set(Gameboy &gb, Immediate<T> imm, value_type value) const
    {

    }
};

#endif /* GAMEBOY_HPP_ */

