#ifndef ACCESSOR_HPP_
#define ACCESSOR_HPP_

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

template<typename T, int offset>
struct accessor<BytePointer<T, offset>> {
    typedef uint8_t value_type;

    value_type get(const GameboyImpl &gb, BytePointer<T, offset> ptr) const
    {
        return sizeof(typename accessor<T>::value_type) == 1 ? 0xff : 2;
    }

    void set(GameboyImpl &gb, BytePointer<T, offset> ptr, value_type value) const
    {

    }
};

template<typename T, int offset>
struct accessor<WordPointer<T, offset>> {
    typedef uint16_t value_type;

    value_type get(const GameboyImpl &gb, WordPointer<T, offset> ptr) const
    {
        return sizeof(typename accessor<T>::value_type) == 1 ? 0xff03 : 3;
    }

    void set(GameboyImpl &gb, WordPointer<T, offset> ptr, value_type value) const
    {

    }
};

template<>
struct accessor<ByteImmediate> {
    typedef uint8_t value_type;

    value_type get(const GameboyImpl &gb, ByteImmediate imm) const
    {
        auto ptr = BytePointer<WordRegister, 1>(WordRegister::PC);
        return accessor<decltype(ptr)>().get(gb, ptr);
    }

    void set(GameboyImpl &gb, ByteImmediate imm, value_type value) const;
};

template<>
struct accessor<WordImmediate> {
    typedef uint16_t value_type;

    value_type get(const GameboyImpl &gb, WordImmediate imm) const
    {
        auto ptr = WordPointer<WordRegister, 1>(WordRegister::PC);
        return accessor<decltype(ptr)>().get(gb, ptr);
    }

    void set(GameboyImpl &gb, WordImmediate imm, value_type value) const;
};

}

#endif /* ACCESSOR_HPP_ */

