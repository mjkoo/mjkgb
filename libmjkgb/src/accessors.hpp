#ifndef ACCESSOR_HPP_
#define ACCESSOR_HPP_

#include <type_traits>

namespace mjkgb {

template<>
struct accessor<ByteRegister> {
    typedef uint8_t value_type;

    value_type get(const GameboyImpl &gb, ByteRegister reg) const
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

    value_type get(const GameboyImpl &gb, WordRegister reg) const
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

    value_type get(const GameboyImpl &gb, ConditionCode cc) const
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

template<typename T, bool inc>
struct accessor<BytePointer<T, inc>> {
    typedef uint8_t value_type;

    value_type get(const GameboyImpl &gb, BytePointer<T, inc> ptr) const
    {
        return sizeof(typename accessor<T>::value_type) == 1 ? 0xff : 2;
    }

    void set(GameboyImpl &gb, BytePointer<T, inc> ptr, value_type value) const
    {
    }
};

template<typename T, bool inc>
struct accessor<WordPointer<T, inc>> {
    typedef uint16_t value_type;

    value_type get(const GameboyImpl &gb, WordPointer<T, inc> ptr) const
    {
        return sizeof(typename accessor<T>::value_type) == 1 ? 0xff03 : 3;
    }

    void set(GameboyImpl &gb, WordPointer<T, inc> ptr, value_type value) const
    {
        static_assert(!is_same<ByteImmediate, decltype(ptr)>::value, "");
    }
};

template<>
struct accessor<ByteImmediate> {
    typedef uint8_t value_type;

    value_type get(const GameboyImpl &gb, ByteImmediate) const
    {
        BytePointer<WordRegister, true> imm_ptr(WordRegister::PC);
        return accessor<decltype(imm_ptr)>().get(gb, imm_ptr);
    }

    void set(GameboyImpl &, ByteImmediate, value_type) const;
};

template<>
struct accessor<WordImmediate> {
    typedef uint16_t value_type;

    value_type get(const GameboyImpl &gb, WordImmediate) const
    {
        WordPointer<WordRegister, true> imm_ptr(WordRegister::PC);
        return accessor<decltype(imm_ptr)>().get(gb, imm_ptr);
    }

    void set(GameboyImpl &, WordImmediate, value_type) const;
};

}

#endif /* ACCESSOR_HPP_ */

