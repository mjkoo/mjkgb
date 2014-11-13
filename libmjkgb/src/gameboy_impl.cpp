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

void GameboyImpl::reset()
{
    set(WordPointer<WordRegister, 0, -2>(WordRegister::SP), get(WordRegister::PC));
    cpu_.set(WordRegister::PC, 0x000f, false);
    set(WordRegister::SP, get(WordRegister::SP) - 2);
}

}

