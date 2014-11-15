#include "cpu.hpp"
#include "mmu.hpp"
#include "gameboy_impl.hpp"
#include "operands.hpp"

namespace {

using namespace mjkgb;

enum class AluOperation {
    ADD, SUB, AND, OR, XOR, CP, INC, DEC
};

enum class RotateShiftOperation {
    RL, RLC, RR, RRC, SLA, SRA, SRL
};

void nop(GameboyImpl &)
{ }

void stop(GameboyImpl &gb)
{
    gb.stop();
}

void halt(GameboyImpl &gb)
{
    gb.halt();
}

void ei(GameboyImpl &gb)
{
    gb.enable_interrupts();
}

void di(GameboyImpl &gb)
{
    gb.disable_interrupts();
}

template<uint16_t off>
void rst(GameboyImpl &gb)
{
    gb.restart(off);
}

template<typename Dst, typename Src>
void ld(GameboyImpl &gb, Dst dst, Src src)
{
    gb.set(dst, gb.get(src));
}

template<AluOperation type, typename Operand, bool carry>
void alu_op(GameboyImpl &gb, Operand op)
{
    switch (type) {
    default:
        break;
    }
    // Special case for ADD SP, r8 (opcode 0xe8), needs extra tick
}

template<RotateShiftOperation type, typename Operand>
void rot_shft_op(GameboyImpl &gb, Operand op)
{
    switch (type) {
    default:
        break;
    }
}

}

namespace mjkgb {

void GameboyImpl::run()
{
    ld(*this, ByteRegister::A, ByteRegister::B);
    nop(*this);
}

}

