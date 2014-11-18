#ifndef GAMEBOY_IMPL_HPP_
#define GAMEBOY_IMPL_HPP_

#include <cstdint>
#include <iosfwd>
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
    GameboyImpl()
      : cpu_(),
        mmu_()
    { }

    template<typename T>
    typename accessor<T>::value_type get(T operand)
    {
        return accessor<T>{}.get(*this, operand);
    }

    template<typename T>
    void set(T operand, typename accessor<T>::value_type value)
    {
        accessor<T>{}.set(*this, operand, value);
    }

    inline void tick()
    {
        cpu_.tick();
    }

    inline void load(std::istream &is)
    {
        mmu_.load(is);
    }

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
        if (cc == ConditionCode::UNCONDITIONAL) return true;

        int flag_index;
        switch (cc) {
        case ConditionCode::Z:
        case ConditionCode::N:
        case ConditionCode::H:
        case ConditionCode::C:
            flag_index = 3 - static_cast<size_t>(cc) + 4;
            break;
        case ConditionCode::NZ:
            flag_index = 7;
            break;
        case ConditionCode::NC:
            flag_index = 4;
            break;
        case ConditionCode::UNCONDITIONAL:
            return true;
        }

        auto flags = gb.cpu_.get(ByteRegister::F);
        auto mask = 1 << flag_index;
        auto value = (flags & mask) != 0;

        return (cc != ConditionCode::NZ && cc != ConditionCode::NC) ? value : !value;
    }

    void set(GameboyImpl &gb, ConditionCode cc, value_type value) const
    {
        if (static_cast<size_t>(cc) >= 4) return;
        
        auto flags = gb.cpu_.get(ByteRegister::F);
        auto mask = 1 << ((3 - static_cast<size_t>(cc)) + 4);

        if (value)
            flags |= mask;
        else
            flags &= ~mask;

        gb.cpu_.set(ByteRegister::F, flags);
    }
};

template<int value>
struct accessor<Constant<value>> {
    using value_type = int;

    value_type get(GameboyImpl &gb, Constant<value>) const
    {
        return value;
    }

    void set(GameboyImpl &gb, Constant<value>, value_type) const;
};

template<typename T>
struct accessor<Ignore<T>> {
    using value_type = typename accessor<T>::value_type;

    value_type get(GameboyImpl &gb, Ignore<T> ign) const
    {
        return gb.get(ign.value);
    }

    void set(GameboyImpl &gb, Ignore<T>, value_type) const { }
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
        gb.tick();

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
        gb.tick();

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

        auto ret = value_type{gb.mmu_.get(address)};
        gb.tick();
        ret |= (gb.mmu_.get(address + 1) << 8);
        gb.tick();

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
        gb.tick();
        gb.mmu_.set(address + 1, (value >> 8) & 0xff);
        gb.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));
    }
};

template<>
struct accessor<ByteImmediate> {
    using value_type = uint8_t;

    value_type get(GameboyImpl &gb, ByteImmediate) const
    {
        auto imm_ptr = byte_ptr(WordRegister::PC);
        auto ret = accessor<decltype(imm_ptr)>().get(gb, imm_ptr);
        gb.cpu_.set(WordRegister::PC, gb.cpu_.get(WordRegister::PC) + sizeof(value_type));
        return ret;
    }

    void set(GameboyImpl &, ByteImmediate, value_type) const;
};

template<>
struct accessor<WordImmediate> {
    using value_type = uint16_t;

    value_type get(GameboyImpl &gb, WordImmediate) const
    {
        auto imm_ptr = word_ptr(WordRegister::PC);
        auto ret = accessor<decltype(imm_ptr)>().get(gb, imm_ptr);
        gb.cpu_.set(WordRegister::PC, gb.cpu_.get(WordRegister::PC) + sizeof(value_type));
        return ret;
    }

    void set(GameboyImpl &, WordImmediate, value_type) const;
};

}

#endif /* GAMEBOY_IMPL_HPP_ */
