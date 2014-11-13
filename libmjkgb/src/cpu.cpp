#include "cpu.hpp"

namespace mjkgb {

Cpu::Cpu()
  : clock_(0),
    registers_()
{ }

uint8_t Cpu::get(ByteRegister reg) const
{
    return registers_[static_cast<size_t>(reg)];
}

uint16_t Cpu::get(WordRegister reg) const
{
    auto index = 2 * static_cast<size_t>(reg);
    return static_cast<uint16_t>(registers_[index] << 8 | registers_[index + 1]);
}

void Cpu::set(ByteRegister reg, uint8_t value)
{
    registers_[static_cast<size_t>(reg)] = value;
}

void Cpu::set(WordRegister reg, uint16_t value, bool tick)
{
    auto index = 2 * static_cast<size_t>(reg);
    registers_[index] = (value >> 8) & 0xff;
    if (tick)
        this->tick();
    registers_[index + 1] = value & 0xff;
}

void Cpu::tick()
{
    clock_++;
}

}

