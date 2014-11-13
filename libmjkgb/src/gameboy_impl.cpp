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

void GameboyImpl::tick()
{
    cpu_.tick();
}

void GameboyImpl::restart(uint8_t n)
{
    set(WordPointer<WordRegister, 0, -2>(WordRegister::SP), get(WordRegister::PC));
    cpu_.set(WordRegister::PC, static_cast<uint16_t>(n), false);
    set(WordRegister::SP, get(WordRegister::SP) - 2);
}

}

