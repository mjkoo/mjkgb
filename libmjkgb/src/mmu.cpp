#include <fstream>

#include "mmu.hpp"

namespace mjkgb {

using namespace std;

Mmu::Mmu()
  : memory_()
{ }

uint8_t Mmu::get(uint16_t address) const
{
    return memory_[address];
}

void Mmu::set(uint16_t address, uint8_t value)
{
    memory_[address] = value;
}

void Mmu::load(const std::string &filename)
{
    memory_.fill(0);

    ifstream is(filename, ifstream::binary);
    if (is)
        is.read(reinterpret_cast<char *>(memory_.data()),
                static_cast<long>(memory_.size()));

    is.close();
}

}
