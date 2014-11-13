#include <fstream>

#include "mmu.hpp"

namespace mjkgb {

using namespace std;

Mmu::Mmu()
  : memory_()
{ }

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
