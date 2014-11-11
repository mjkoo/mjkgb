#include <cstdint>
#include <fstream>

#include "mjkgb.hpp"
#include "gameboy_impl.hpp"

using namespace std;

namespace mjkgb {

Gameboy::Gameboy()
  : pimpl_(new Gameboy::impl)
{ }

Gameboy::Gameboy(const string &filename)
  : Gameboy()
{
    load(filename);
}

Gameboy::~Gameboy() = default;

void Gameboy::load(const string &filename)
{
    pimpl_->load(filename);
}

}

