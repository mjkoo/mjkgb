#include <type_traits>

#include "cpu.hpp"
#include "mmu.hpp"
#include "gameboy_impl.hpp"
#include "operands.hpp"

namespace {

using namespace std;
using namespace mjkgb;

void unimplemented_opcode(GameboyImpl &gb)
{ }

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
void ld(GameboyImpl &gb, WordRegister dst, WordPointer<WordRegister> src, Displacement)
{
    auto disp = gb.mmu_.get(gb.cpu_.get(WordRegister::PC));
    gb.cpu_.set(WordRegister::PC, gb.cpu_.get(WordRegister::PC) + 1);
    auto addr = static_cast<uint16_t>(gb.cpu_.get(src.value) + disp);
    gb.cpu_.set(dst, static_cast<uint16_t>(gb.mmu_.get(addr) | (gb.mmu_.get(addr + 1) << 8)));
}

void push(GameboyImpl &gb, WordRegister reg)
{
    ld(gb, WordPointer<WordRegister, -2>(WordRegister::SP), reg);
}

void pop(GameboyImpl &gb, WordRegister reg)
{
    ld(gb, reg, WordPointer<WordRegister, 2>(WordRegister::SP));
}

template<typename Dst, typename Src, bool sub, bool carry>
void add_sub(GameboyImpl &gb, Dst dst, Src src)
{
    static_assert(!is_same<Src, Constant<1>>::value ?
            sizeof(typename accessor<Dst>::value_type) ==
            sizeof(typename accessor<Src>::value_type) :
            true,
            "Invalid ALU operation");
    static_assert(!(sizeof(typename accessor<Dst>::value_type) == 2 && sub),
            "No 16-bit subtraction");

    using result_type = typename accessor<Dst>::value_type;
    constexpr auto half_carry_mask = 1 << (8 * sizeof(result_type) - 4);

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

void inc(GameboyImpl &gb, ByteRegister op)
{
    auto carry_flag = gb.get(ConditionCode::C);
    add(gb, op, Constant<1>());
    gb.set(ConditionCode::C, carry_flag);
}

void inc(GameboyImpl &gb, WordRegister op)
{
    gb.set(op, gb.get(op) + 1);
}

void dec(GameboyImpl &gb, ByteRegister op)
{
    auto carry_flag = gb.get(ConditionCode::C);
    sub(gb, op, Constant<1>());
    gb.set(ConditionCode::C, carry_flag);
}

void dec(GameboyImpl &gb, WordRegister op)
{
    gb.set(op, gb.get(op) - 1);
}

template<typename Op>
void cp(GameboyImpl &gb, Op op)
{
    sub(gb, Ignore<ByteRegister>(ByteRegister::A), op);
}

enum class BitwiseOperation {
    AND, OR, XOR
};

template<typename Op, BitwiseOperation kind>
void bitwise_op(GameboyImpl &gb, Op op)
{
    static_assert(sizeof(Op::value_type) == 1,
            "Invalid ALU operation");

    typename Op::value_type result;
    switch (kind) {
    case BitwiseOperation::AND:
        result = gb.get(ByteRegister::A) & gb.get(op);
        break;
    case BitwiseOperation::OR:
        result = gb.get(ByteRegister::A) | gb.get(op);
        break;
    case BitwiseOperation::XOR:
        result = gb.get(ByteRegister::A) ^ gb.get(op);
        break;
    }

    gb.set(op, result);
    gb.set(ByteRegister::F, kind == BitwiseOperation::AND ? 0x20 : 0x00);
    gb.set(ConditionCode::Z, result);
}

template<typename Dst, typename Src>
void and_(GameboyImpl &gb, Dst dst, Src src)
{
    bitwise_op<Dst, Src, BitwiseOperation::AND>(gb, dst, src);
}

template<typename Dst, typename Src>
void or_(GameboyImpl &gb, Dst dst, Src src)
{
    bitwise_op<Dst, Src, BitwiseOperation::OR>(gb, dst, src);
}

template<typename Dst, typename Src>
void xor_(GameboyImpl &gb, Dst dst, Src src)
{
    bitwise_op<Dst, Src, BitwiseOperation::XOR>(gb, dst, src);
}

template<typename Op>
void swap(GameboyImpl &gb, Op op)
{
    static_assert(sizeof(Op::value_type) == 1,
            "Invalid ALU operation");

    auto op_value = gb.get(op);
    gb.set(op, (op_value & 0xf) << 4 | ((op_value & 0xf0) >> 4));
}

void daa(GameboyImpl &gb)
{

}

void cpl(GameboyImpl &gb)
{
    gb.set(ByteRegister::A, ~gb.get(ByteRegister::A));
    gb.set(ConditionCode::N, true);
    gb.set(ConditionCode::H, true);
}

void ccf(GameboyImpl &gb)
{
    gb.set(ByteRegister::F, gb.get(ByteRegister::F) & 0x90);
    gb.set(ConditionCode::C, !gb.get(ConditionCode::C));
}

void scf(GameboyImpl &gb)
{
    gb.set(ByteRegister::F, gb.get(ByteRegister::F) & 0x90);
    gb.set(ConditionCode::C, true);
}

enum class RotateShiftOperation {
    RLCA, RLA, RRCA, RRA, RLC, RL, RRC, RR, SLA, SRA, SRL
};

template<typename Op, RotateShiftOperation kind>
void rotate_shift_op(GameboyImpl &gb, Op op)
{
    static_assert(static_cast<int>(kind) < 4 ?
            is_same<Op, ByteRegister>::value : true,
            "Invalid rotate/shift operation");

    using result_type = typename accessor<Op>::value_type;

    int result;
    auto value = gb.get(op);
    auto carry_flag = gb.get(ConditionCode::C);
    auto top_bit_mask = 1 << (8 * sizeof(result_type) - 1);

    switch (kind) {
    case RotateShiftOperation::RLCA:
    case RotateShiftOperation::RLC:
        carry_flag = value & top_bit_mask;
        result = (value << 1) | (carry_flag ? 0 : 1);
        break;
    case RotateShiftOperation::RLA:
    case RotateShiftOperation::RL:
        result = (value << 1) | (carry_flag ? 0 : 1);
        carry_flag = value & top_bit_mask;
        break;
    case RotateShiftOperation::RRCA:
    case RotateShiftOperation::RRC:
        carry_flag = value & 1;
        result = (value >> 1) | (carry_flag ? 0 : top_bit_mask);
        break;
    case RotateShiftOperation::RRA:
    case RotateShiftOperation::RR:
        result = (value >> 1) | (carry_flag ? 0 : top_bit_mask);
        carry_flag = value & 1;
        break;
    case RotateShiftOperation::SLA:
        carry_flag = value & top_bit_mask;
        result = value << 1;
        break;
    case RotateShiftOperation::SRA:
        carry_flag = value & 1;
        result = static_cast<int>(value) >> 1;
        break;
    case RotateShiftOperation::SRL:
        carry_flag = value & 1;
        result = value >> 1;
        break;
    }

    gb.set(op, static_cast<result_type>(value));

    if (static_cast<int>(kind) >= 4) {
        gb.set(ConditionCode::Z, value == 0);
        gb.cpu_.tick();
    } else {
        gb.set(ConditionCode::Z, false);
    }

    gb.set(ConditionCode::N, false);
    gb.set(ConditionCode::H, false);
    gb.set(ConditionCode::C, carry_flag);
}

void rlca(GameboyImpl &gb)
{
    rotate_shift_op<ByteRegister, RotateShiftOperation::RLCA>(gb, ByteRegister::A);
}

void rla(GameboyImpl &gb)
{
    rotate_shift_op<ByteRegister, RotateShiftOperation::RLA>(gb, ByteRegister::A);
}

void rrca(GameboyImpl &gb)
{
    rotate_shift_op<ByteRegister, RotateShiftOperation::RRCA>(gb, ByteRegister::A);
}

void rra(GameboyImpl &gb)
{
    rotate_shift_op<ByteRegister, RotateShiftOperation::RRA>(gb, ByteRegister::A);
}

template<typename Op>
void rlc(GameboyImpl &gb, Op op)
{
    rotate_shift_op<Op, RotateShiftOperation::RLC>(gb, op);
}

template<typename Op>
void rl(GameboyImpl &gb, Op op)
{
    rotate_shift_op<Op, RotateShiftOperation::RL>(gb, op);
}

template<typename Op>
void rrc(GameboyImpl &gb, Op op)
{
    rotate_shift_op<Op, RotateShiftOperation::RRC>(gb, op);
}

template<typename Op>
void rr(GameboyImpl &gb, Op op)
{
    rotate_shift_op<Op, RotateShiftOperation::RR>(gb, op);
}

template<typename Op>
void sla(GameboyImpl &gb, Op op)
{
    rotate_shift_op<Op, RotateShiftOperation::SLA>(gb, op);
}

template<typename Op>
void sra(GameboyImpl &gb, Op op)
{
    rotate_shift_op<Op, RotateShiftOperation::SRA>(gb, op);
}

template<typename Op>
void srl(GameboyImpl &gb, Op op)
{
    rotate_shift_op<Op, RotateShiftOperation::SRL>(gb, op);
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

