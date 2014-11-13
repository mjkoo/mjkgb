#include "gameboy_impl.hpp"

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

void reset(GameboyImpl &gb)
{
    gb.reset();
}

template<typename Dst, typename Src>
void ld(GameboyImpl &gb, Dst dst, Src src)
{
    gb.set(dst, gb.get(src));
}

template<typename Operand, int inc>
void inc_dec(GameboyImpl &gb, Operand op)
{
    gb.set(op, gb.get(op) + inc);
}

template<AluOperation op, typename Dst, typename Src, bool carry>
void add(GameboyImpl &gb, Dst dst, Src src)
{
    switch (op) {
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

