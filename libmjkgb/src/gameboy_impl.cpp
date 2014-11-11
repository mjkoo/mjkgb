#include <string>

#include "gameboy_impl.hpp"

using namespace std;

namespace mjkgb {

void GameboyImpl::load(const string &filename)
{
    mmu_.load(filename);
}

}

