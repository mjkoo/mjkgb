#include <string>

#include "gameboy_impl.hpp"

namespace mjkgb {

using namespace std;

GameboyImpl::GameboyImpl()
  : cpu_(),
    mmu_()
{ }

void GameboyImpl::load(const string &filename)
{
    mmu_.load(filename);
}

}

