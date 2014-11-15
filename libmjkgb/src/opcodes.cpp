#include <type_traits>

#include "cpu.hpp"
#include "mmu.hpp"
#include "gameboy_impl.hpp"
#include "operands.hpp"

namespace {

using namespace mjkgb;

void nop(GameboyImpl &)
{ }

void stop(GameboyImpl &gb)
{
    gb.cpu_.stop();
}

void halt(GameboyImpl &gb)
{
    gb.cpu_.halt();
}

void ei(GameboyImpl &gb)
{
    gb.cpu_.enable_interrupts();
}

void di(GameboyImpl &gb)
{
    gb.cpu_.disable_interrupts();
}

template<uint16_t off>
void rst(GameboyImpl &gb)
{
    gb.set(WordPointer<WordRegister, 0, -2>(WordRegister::SP), gb.get(WordRegister::PC));
    gb.cpu_.set(WordRegister::PC, static_cast<uint16_t>(off), false);
    gb.set(WordRegister::SP, gb.get(WordRegister::SP) - 2);
}

template<typename Dst, typename Src>
void ld(GameboyImpl &gb, Dst dst, Src src)
{
    gb.set(dst, gb.get(src));
}

/* This is really just for LD HL, (SP + e), but do it this way for consistency. */
void ld(GameboyImpl &gb, WordRegister dst, WordRegister src, Displacement)
{
    auto disp = gb.mmu_.get(gb.cpu_.get(WordRegister::PC));
    gb.cpu_.set(WordRegister::PC, gb.cpu_.get(WordRegister::PC) + 1);
    auto addr = static_cast<uint16_t>(gb.cpu_.get(WordRegister::SP) + disp);
    gb.cpu_.set(dst, static_cast<uint16_t>(gb.mmu_.get(addr) | (gb.mmu_.get(addr + 1) << 8)));
}

template<typename Dst, typename Src, bool sub, bool carry>
void add_sub(GameboyImpl &gb, Dst dst, Src src)
{
    static_assert(sizeof(Dst::value_type) == sizeof(Src::value_type),
            "Invalid ALU operation");
    static_assert(!(sizeof(Dst::value_type) == 2 && sub),
            "No 16-bit subtraction");

    using result_type = typename Dst::value_type;
    constexpr auto half_carry_mask = 1 << (sizeof(result_type) - 4);

    auto dst_value = gb.get(dst);
    auto src_value = gb.get(src) + ((carry && gb.get(ConditionCode::C)) ? 1 : 0);
    if (sub) src_value = -src_value;

    auto result = dst_value + src_value;

    gb.set(dst, static_cast<result_type>(result));
    gb.set(ConditionCode::Z, result);
    gb.set(ConditionCode::N, sub);
    gb.set(ConditionCode::H, (result ^ dst_value ^ src_value) & half_carry_mask);
    gb.set(ConditionCode::C, result < dst_value);
}

template<typename Dst, typename Src>
void add(GameboyImpl &gb, Dst dst, Src src)
{
    add_sub<Dst, Src, false, false>(gb, dst, src);
}

template<typename Dst, typename Src>
void adc(GameboyImpl &gb, Dst dst, Src src)
{
    add_sub<Dst, Src, false, true>(gb, dst, src);
}

template<typename Dst, typename Src>
void sub(GameboyImpl &gb, Dst dst, Src src)
{
    add_sub<Dst, Src, true, false>(gb, dst, src);
}

template<typename Dst, typename Src>
void sbc(GameboyImpl &gb, Dst dst, Src src)
{
    add_sub<Dst, Src, true, true>(gb, dst, src);
}

template<typename Op>
void inc(GameboyImpl &gb, Op op)
{
    add(op, Constant<1>());
}

template<typename Op>
void dec(GameboyImpl &gb, Op op)
{
    sub(op, Constant<1>());
}

template<typename Op>
void cp(GameboyImpl &gb, Op op)
{
    sub(gb, Ignore<ByteRegister>(ByteRegister::A), op);
}

enum class BitwiseOperation {
    AND, OR, XOR
};

template<typename Dst, typename Src, BitwiseOperation kind>
void and_or_xor(GameboyImpl &gb, Dst dst, Src src)
{
    static_assert(sizeof(Dst::value_type) == sizeof(Src::value_type),
            "Invalid ALU operation");
    static_assert(sizeof(Dst::value_type) == 1,
            "No 16-bit AND");

    typename Dst::value_type result;
    switch (kind) {
    case BitwiseOperation::AND:
        result = gb.get(dst) & gb.get(src);
        break;
    case BitwiseOperation::OR:
        result = gb.get(dst) | gb.get(src);
        break;
    case BitwiseOperation::XOR:
        result = gb.get(dst) ^ gb.get(src);
        break;
    }

    gb.set(dst, result);
    gb.set(ByteRegister::F, kind == BitwiseOperation::AND ? 0x20 : 0x00);
    gb.set(ConditionCode::Z, result);
}

template<typename Dst, typename Src>
void and_(GameboyImpl &gb, Dst dst, Src src)
{
    and_or_xor<Dst, Src, BitwiseOperation::AND>(gb, dst, src);
}

template<typename Dst, typename Src>
void or_(GameboyImpl &gb, Dst dst, Src src)
{
    and_or_xor<Dst, Src, BitwiseOperation::OR>(gb, dst, src);
}

template<typename Dst, typename Src>
void xor_(GameboyImpl &gb, Dst dst, Src src)
{
    and_or_xor<Dst, Src, BitwiseOperation::XOR>(gb, dst, src);
}

}

namespace mjkgb {

#ifndef EMIT_LLVM
void GameboyImpl::run()
{
    ld(*this, ByteRegister::A, ByteRegister::B);
    nop(*this);
}
#endif

}

