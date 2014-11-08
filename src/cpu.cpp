#include "cpu.hpp"

Cpu::Cpu()
  : registers_()
{ }

uint8_t
Cpu::get(ByteRegister reg) const
{
    return registers_[static_cast<size_t>(reg)];
}

uint16_t
Cpu::get(WordRegister reg) const
{
    size_t index = 2 * static_cast<size_t>(reg);
    return static_cast<uint16_t>(registers_[index] << 8 | registers_[index + 1]);
}

void
Cpu::set(ByteRegister reg, uint8_t value)
{

}

void
Cpu::set(WordRegister reg, uint16_t value)
{

}

