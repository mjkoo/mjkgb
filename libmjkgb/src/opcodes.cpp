#include <cstdint>
#include <functional>
#include <type_traits>

#include "cpu.hpp"
#include "mmu.hpp"
#include "gameboy_impl.hpp"
#include "operands.hpp"

namespace mjkgb {

using namespace std;

namespace opcodes {
namespace {

void nop(GameboyImpl &)
{ }

void undefined(GameboyImpl &gb)
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

template<typename Dst, typename Src>
void ld(GameboyImpl &gb, Dst dst, Src src)
{
    gb.set(dst, gb.get(src));
}

void ld_hl_sp_n(GameboyImpl &gb)
{
    auto disp = static_cast<int8_t>(gb.get(ByteImmediate{}));
    gb.set(WordRegister::HL, static_cast<uint16_t>(gb.get(WordRegister::SP) + disp));
}

void push(GameboyImpl &gb, WordRegister reg)
{
    ld(gb, word_ptr<-2, -2>(WordRegister::SP), reg);
}

void pop(GameboyImpl &gb, WordRegister reg)
{
    ld(gb, reg, word_ptr(WordRegister::SP));
    gb.cpu_.set(WordRegister::SP, gb.get(WordRegister::SP) + 2);
}

template<bool sub, bool carry, typename Dst, typename Src>
void add_sub(GameboyImpl &gb, Dst dst, Src src)
{
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
    add_sub<false, false>(gb, dst, src);
}

void add_sp_n(GameboyImpl &gb)
{
    add(gb, WordRegister::SP, ByteImmediate{});
    gb.set(ConditionCode::Z, false);
    gb.cpu_.tick();
}

template<typename Dst, typename Src>
void adc(GameboyImpl &gb, Dst dst, Src src)
{
    add_sub<false, true>(gb, dst, src);
}

template<typename Dst, typename Src>
void sub(GameboyImpl &gb, Dst dst, Src src)
{
    add_sub<true, false>(gb, dst, src);
}

template<typename Dst, typename Src>
void sbc(GameboyImpl &gb, Dst dst, Src src)
{
    add_sub<true, true>(gb, dst, src);
}

template<typename Op>
void inc(GameboyImpl &gb, Op op)
{
    auto carry_flag = gb.get(ConditionCode::C);
    add(gb, op, Constant<1>());
    gb.set(ConditionCode::C, carry_flag);
}

template<>
void inc(GameboyImpl &gb, WordRegister op)
{
    gb.set(op, gb.get(op) + 1);
}

template<typename Op>
void dec(GameboyImpl &gb, Op op)
{
    auto carry_flag = gb.get(ConditionCode::C);
    sub(gb, op, Constant<1>());
    gb.set(ConditionCode::C, carry_flag);
}

template<>
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

template<BitwiseOperation kind, typename Op>
void bitwise_op(GameboyImpl &gb, Op op)
{
    using result_type = typename accessor<Op>::value_type;

    static_assert(sizeof(result_type) == 1,
            "Invalid ALU operation");

    result_type result;
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

    gb.set(ByteRegister::A, result);
    gb.set(ByteRegister::F, kind == BitwiseOperation::AND ? 0x20 : 0x00);
    gb.set(ConditionCode::Z, result);
}

template<typename Op>
void and_(GameboyImpl &gb, Op op)
{
    bitwise_op<BitwiseOperation::AND>(gb, op);
}

template<typename Op>
void or_(GameboyImpl &gb, Op op)
{
    bitwise_op<BitwiseOperation::OR>(gb, op);
}

template<typename Op>
void xor_(GameboyImpl &gb, Op op)
{
    bitwise_op<BitwiseOperation::XOR>(gb, op);
}

template<typename Op>
void swap(GameboyImpl &gb, Op op)
{
    using result_type = typename accessor<Op>::value_type;

    static_assert(sizeof(result_type) == 1,
            "Invalid ALU operation");

    auto value = gb.get(op);
    auto result = static_cast<result_type>((value & 0xf) << 4 |
            ((value & 0xf0) >> 4));
    gb.set(op, result);
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

template<RotateShiftOperation kind, typename Op>
void rotate_shift_op(GameboyImpl &gb, Op op)
{
    using result_type = typename accessor<Op>::value_type;

    static_assert(static_cast<int>(kind) < 4 ?
            is_same<Op, ByteRegister>::value : true,
            "Invalid rotate/shift operation");

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

    if (static_cast<int>(kind) >= 4)
        gb.set(ConditionCode::Z, value == 0);
    else
        gb.set(ConditionCode::Z, false);

    gb.set(ConditionCode::N, false);
    gb.set(ConditionCode::H, false);
    gb.set(ConditionCode::C, carry_flag);
}

void rlca(GameboyImpl &gb)
{
    rotate_shift_op<RotateShiftOperation::RLCA>(gb, ByteRegister::A);
}

void rla(GameboyImpl &gb)
{
    rotate_shift_op<RotateShiftOperation::RLA>(gb, ByteRegister::A);
}

void rrca(GameboyImpl &gb)
{
    rotate_shift_op<RotateShiftOperation::RRCA>(gb, ByteRegister::A);
}

void rra(GameboyImpl &gb)
{
    rotate_shift_op<RotateShiftOperation::RRA>(gb, ByteRegister::A);
}

template<typename Op>
void rlc(GameboyImpl &gb, Op op)
{
    rotate_shift_op<RotateShiftOperation::RLC>(gb, op);
}

template<typename Op>
void rl(GameboyImpl &gb, Op op)
{
    rotate_shift_op<RotateShiftOperation::RL>(gb, op);
}

template<typename Op>
void rrc(GameboyImpl &gb, Op op)
{
    rotate_shift_op<RotateShiftOperation::RRC>(gb, op);
}

template<typename Op>
void rr(GameboyImpl &gb, Op op)
{
    rotate_shift_op<RotateShiftOperation::RR>(gb, op);
}

template<typename Op>
void sla(GameboyImpl &gb, Op op)
{
    rotate_shift_op<RotateShiftOperation::SLA>(gb, op);
}

template<typename Op>
void sra(GameboyImpl &gb, Op op)
{
    rotate_shift_op<RotateShiftOperation::SRA>(gb, op);
}

template<typename Op>
void srl(GameboyImpl &gb, Op op)
{
    rotate_shift_op<RotateShiftOperation::SRL>(gb, op);
}

template<unsigned int b, typename Op>
void bit(GameboyImpl &gb, Op op)
{
    auto value = gb.get(op);

    // This is a no-op for byte registers, causes a tick for (HL)
    gb.set(op, value);

    gb.set(ConditionCode::Z, value & (1 << b));
    gb.set(ConditionCode::N, false);
    gb.set(ConditionCode::H, true);
}

template<unsigned int b, typename Op>
void set(GameboyImpl &gb, Op op)
{
    gb.set(op, gb.get(op) | (1 << b));
}

template<unsigned int b, typename Op>
void res(GameboyImpl &gb, Op op)
{
    gb.set(op, gb.get(op) & ~(1 << b));
}

template<ConditionCode cc, typename Op>
void jp(GameboyImpl &gb, Op op)
{
    if (gb.get(cc))
        gb.jump(gb.get(op));
}

void jp_hl(GameboyImpl &gb)
{
    gb.jump(gb.get(WordRegister::HL), false);
}

template<ConditionCode cc>
void jr(GameboyImpl &gb)
{
    auto disp = static_cast<int8_t>(gb.get(ByteImmediate{}));
    if (gb.get(cc))
        gb.jump(gb.get(WordRegister::PC) + disp);
}

template<ConditionCode cc, typename Op>
void call(GameboyImpl &gb, Op op)
{
    auto dest = gb.get(op);
    if (gb.get(cc)) {
        gb.set(word_ptr<0, 2>(WordRegister::SP), gb.get(WordRegister::PC));
        gb.set(WordRegister::SP, gb.get(WordRegister::SP) - 2);
        gb.jump(dest, false);
    }
}

template<uint16_t off>
void rst(GameboyImpl &gb)
{
    call<ConditionCode::UNCONDITIONAL>(gb, Constant<off>());
}

template<ConditionCode cc, bool enable_interrupts = false>
void ret(GameboyImpl &gb)
{
    if (cc != ConditionCode::UNCONDITIONAL)
        gb.cpu_.tick();

    if (enable_interrupts)
        gb.cpu_.enable_interrupts();

    if (gb.get(cc))
        gb.jump(gb.get(word_ptr<2>(WordRegister::SP)));
}

void cb_prefix(GameboyImpl &);

}
}

#ifndef EMIT_LLVM

/* Dispatch opcodes using a computed goto, see article at 
 * http://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
 * for a good description. We use this less for efficiency reasons, and more for
 * syntactic/convenience reasons.
 */
void GameboyImpl::run()
{
    uint8_t opcode;
    static const void *dispatch_table[] = {
#define X(name, def, is_jump, cycles) &&name,
#include "opcode_map.in"
#undef X
    };
#define DISPATCH() do {                                     \
    if (cpu_.is_stopped()) return;                          \
    opcode = get(ByteImmediate{});                          \
    goto *dispatch_table[opcode];                           \
} while (false);

    cpu_.reset();

    DISPATCH();
    while (true) {
#define X(name, def, is_jump, cycles) do {                  \
    name:                                                   \
        def(*this);                                         \
        DISPATCH();                                         \
} while (false);
#include "opcode_map.in"
#undef X
    }
}

namespace opcodes {
namespace {

void cb_prefix(GameboyImpl &gb)
{
    static const void *dispatch_table[] = {
#define X(name, def, is_jump, cycles) &&name,
#include "cb_opcode_map.in"
#undef X
    };

    auto opcode = gb.get(ByteImmediate{});
    goto *dispatch_table[opcode];

#define X(name, def, is_jump, cycles) do {                  \
    name:                                                   \
        def(gb);                                            \
        return;                                             \
} while (false);
#include "cb_opcode_map.in"
#undef X
}

}
}

#else

namespace opcodes {
namespace {

void cb_prefix(GameboyImpl &gb)
{
    gb.tick();
}

}
}

extern "C" {
#define X(name, def, is_jump, cycles)                       \
__attribute__((used))                                       \
void name(GameboyImpl &gb)                                  \
{                                                           \
    if (gb.cpu_.is_stopped()) return;                       \
    def(gb);                                                \
    gb.tick();                                              \
}
#include "opcode_map.in"
#include "cb_opcode_map.in"
#undef X
}


#endif

}

