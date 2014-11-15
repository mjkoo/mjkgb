#include "cpu.hpp"

namespace mjkgb {

Cpu::Cpu()
  : clock_(0),
    registers_()
{ }

void Cpu::tick()
{
    clock_++;
}

void Cpu::stop()
{

}

void Cpu::halt()
{

}

void Cpu::enable_interrupts()
{

}

void Cpu::disable_interrupts()
{

}

}

