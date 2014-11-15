#ifndef GAMEBOY_IMPL_HPP_
#define GAMEBOY_IMPL_HPP_

#include <cstdint>
#include <string>

#include "mjkgb.hpp"
#include "cpu.hpp"
#include "mmu.hpp"
#include "operands.hpp"

namespace mjkgb {

template<typename T> struct accessor;

/* Need to be able to pass GameboyImpl ref to accessor funcs, so can't directly
 * implement Gameboy::impl due to it being private. Also don't want to make
 * Gameboy::impl public. Use inheritance to work around this issue.
 */
struct GameboyImpl {
    GameboyImpl();

    template<typename T>
    typename accessor<T>::value_type get(T operand)
    {
        return accessor<T>().get(*this, operand);
    }

    template<typename T>
    void set(T operand, typename accessor<T>::value_type value)
    {
        accessor<T>().set(*this, operand, value);
    }

    void load(const std::string &filename);

    void run();

    Cpu cpu_;
    Mmu mmu_;

    template<typename T> friend struct accessor;
};

class Gameboy::impl : public GameboyImpl { };

/* Accessor functors, allow uniform access to various operands and automatic
 * cycle counting.
 */
template<>
struct accessor<ByteRegister> {
    using value_type = uint8_t;

    value_type get(GameboyImpl &gb, ByteRegister reg) const
    {
        return gb.cpu_.get(reg);
    }

    void set(GameboyImpl &gb, ByteRegister reg, value_type value) const
    {
        if (reg != ByteRegister::F)
            gb.cpu_.set(reg, value);
        else
            gb.cpu_.set(ByteRegister::F, value & 0xf0);
    }
};

template<>
struct accessor<WordRegister> {
    using value_type = uint16_t;

    value_type get(GameboyImpl &gb, WordRegister reg) const
    {
        return gb.cpu_.get(reg);
    }

    void set(GameboyImpl &gb, WordRegister reg, value_type value) const
    {
        if (reg != WordRegister::AF)
            gb.cpu_.set(reg, value);
        else
            gb.cpu_.set(WordRegister::AF, value & 0xfff0);
    }
};

template<>
struct accessor<ConditionCode> {
    using value_type = bool;

    value_type get(GameboyImpl &gb, ConditionCode cc) const
    {
        auto flags = gb.cpu_.get(ByteRegister::F);
        return (flags & (1 << (static_cast<int>(cc) + 4))) != 0;
    }

    void set(GameboyImpl &gb, ConditionCode cc, value_type value) const
    {
        auto flags = gb.cpu_.get(ByteRegister::F);

        if (value)
            flags |= (1 << (static_cast<int>(cc) + 4));
        else
            flags &= ~(1 << (static_cast<int>(cc) + 4));

        gb.cpu_.set(ByteRegister::F, flags);
    }
};

template<typename T, int inc, int off>
struct accessor<BytePointer<T, inc, off>> {
    using value_type = uint8_t;

    value_type get(GameboyImpl &gb, BytePointer<T, inc, off> ptr) const
    {
        using operand_type = typename accessor<T>::value_type;
        uint16_t address = sizeof(operand_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);
        address += off;

        auto ret = gb.mmu_.get(address);
        gb.cpu_.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));

        return ret;
    }

    void set(GameboyImpl &gb, BytePointer<T, inc, off> ptr, value_type value) const
    {
        using operand_type = typename accessor<T>::value_type;
        uint16_t address = sizeof(typename accessor<T>::value_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);
        address += off;

        gb.mmu_.set(address, value);
        gb.cpu_.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));
    } };

template<typename T, int inc, int off>
struct accessor<WordPointer<T, inc, off>> {
    using value_type = uint16_t;

    value_type get(GameboyImpl &gb, WordPointer<T, inc, off> ptr) const
    {
        using operand_type = typename accessor<T>::value_type;
        uint16_t address = sizeof(operand_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);
        address += off;

        auto ret = static_cast<value_type>(gb.mmu_.get(address));
        gb.cpu_.tick();
        ret |= (gb.mmu_.get(address + 1) << 8);
        gb.cpu_.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));

        return ret;
    }

    void set(GameboyImpl &gb, WordPointer<T, inc, off> ptr, value_type value) const
    {
        using operand_type =typename accessor<T>::value_type;
        uint16_t address = sizeof(operand_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);
        address += off;

        gb.mmu_.set(address, value & 0xff);
        gb.cpu_.tick();
        gb.mmu_.set(address + 1, (value >> 8) & 0xff);
        gb.cpu_.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));
    }
};

template<>
struct accessor<ByteImmediate> {
    using value_type = uint8_t;

    value_type get(GameboyImpl &gb, ByteImmediate) const
    {
        auto imm_ptr = BytePointer<WordRegister, sizeof(value_type)>(WordRegister::PC);
        auto ret = accessor<decltype(imm_ptr)>().get(gb, imm_ptr);
        return ret;
    }

    void set(GameboyImpl &, ByteImmediate, value_type) const;
};

template<>
struct accessor<WordImmediate> {
    using value_type = uint16_t;

    value_type get(GameboyImpl &gb, WordImmediate) const
    {
        auto imm_ptr = WordPointer<WordRegister, sizeof(value_type)>(WordRegister::PC);
        auto ret = accessor<decltype(imm_ptr)>().get(gb, imm_ptr);
        return ret;
    }

    void set(GameboyImpl &, WordImmediate, value_type) const;
};

template<>
struct accessor<Displacement> {
    using value_type = int8_t;

    value_type get(GameboyImpl &gb, Displacement) const
    {
        auto ret = accessor<ByteImmediate>().get(gb, ByteImmediate());
        return static_cast<value_type>(ret);
    }

    void set(GameboyImpl &, Displacement, value_type) const;
};

}

#endif /* GAMEBOY_IMPL_HPP_ */
