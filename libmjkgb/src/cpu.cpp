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

}

