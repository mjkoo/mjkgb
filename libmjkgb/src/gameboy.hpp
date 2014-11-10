#ifndef GAMEBOY_HPP_
#define GAMEBOY_HPP_

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

#endif /* GAMEBOY_HPP_ */

