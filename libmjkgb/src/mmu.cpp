#include <istream>

#include "mmu.hpp"

namespace mjkgb {

using namespace std;

void Mmu::set_native(uint16_t address, uintptr_t func)
{
    native_[address].store(func);
}

void Mmu::load(istream &is)
{
    memory_.fill(0);
    is.read(reinterpret_cast<char *>(memory_.data()),
            static_cast<long>(memory_.size()));
}

}
