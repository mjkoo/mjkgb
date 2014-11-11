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
class GameboyImpl {
public:
    GameboyImpl()
      : cpu_(),
        mmu_()
    { }

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

private:
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
    typedef uint8_t value_type;

    value_type get(GameboyImpl &gb, ByteRegister reg) const
    {
        return gb.cpu_.get(reg);
    }

    void set(GameboyImpl &gb, ByteRegister reg, value_type value) const
    {
        gb.cpu_.set(reg, value);
    }
};

template<>
struct accessor<WordRegister> {
    typedef uint16_t value_type;

    value_type get(GameboyImpl &gb, WordRegister reg) const
    {
        return gb.cpu_.get(reg);
    }

    void set(GameboyImpl &gb, WordRegister reg, value_type value) const
    {
        return gb.cpu_.set(reg, value);
    }
};

template<>
struct accessor<ConditionCode> {
    typedef bool value_type;

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

template<typename T, int inc>
struct accessor<BytePointer<T, inc>> {
    typedef uint8_t value_type;

    value_type get(GameboyImpl &gb, BytePointer<T, inc> ptr) const
    {
        typedef typename accessor<T>::value_type operand_type;
        uint16_t address = sizeof(operand_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);

        auto ret = gb.mmu_.get(address);
        gb.cpu_.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));

        return ret;
    }

    void set(GameboyImpl &gb, BytePointer<T, inc> ptr, value_type value) const
    {
        typedef typename accessor<T>::value_type operand_type;
        uint16_t address = sizeof(typename accessor<T>::value_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);

        gb.mmu_.set(address, value);
        gb.cpu_.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));
    }
};

template<typename T, int inc>
struct accessor<WordPointer<T, inc>> {
    typedef uint16_t value_type;

    value_type get(GameboyImpl &gb, WordPointer<T, inc> ptr) const
    {
        typedef typename accessor<T>::value_type operand_type;
        uint16_t address = sizeof(typename accessor<T>::value_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);

        auto ret = static_cast<value_type>(gb.mmu_.get(address));
        gb.cpu_.tick();
        ret |= (gb.mmu_.get(address + 1) << 8);
        gb.cpu_.tick();

        if (inc)
            gb.set(ptr.value, static_cast<operand_type>(gb.get(ptr.value) + inc));

        return ret;
    }

    void set(GameboyImpl &gb, WordPointer<T, inc> ptr, value_type value) const
    {
        typedef typename accessor<T>::value_type operand_type;
        uint16_t address = sizeof(typename accessor<T>::value_type) == 1 ? 0xff00 : 0;
        address |= gb.get(ptr.value);

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
    typedef uint8_t value_type;

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
    typedef uint16_t value_type;

    value_type get(GameboyImpl &gb, WordImmediate) const
    {
        auto imm_ptr = WordPointer<WordRegister, sizeof(value_type)>(WordRegister::PC);
        auto ret = accessor<decltype(imm_ptr)>().get(gb, imm_ptr);
        return ret;
    }

    void set(GameboyImpl &, WordImmediate, value_type) const;
};

}

#endif /* GAMEBOY_IMPL_HPP_ */
