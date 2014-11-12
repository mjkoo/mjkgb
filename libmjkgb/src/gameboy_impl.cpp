#include <string>

#include "gameboy_impl.hpp"

using namespace std;

namespace mjkgb {

GameboyImpl::GameboyImpl()
  : cpu_(),
    mmu_()
{ }

void GameboyImpl::load(const string &filename)
{
    mmu_.load(filename);
}

}

