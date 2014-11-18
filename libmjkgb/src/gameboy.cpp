#include <cstdint>
#include <fstream>
#include <istream>

#include "mjkgb.hpp"
#include "gameboy_impl.hpp"

namespace mjkgb {

using namespace std;

Gameboy::Gameboy()
  : pimpl_(new Gameboy::impl)
{ }

Gameboy::Gameboy(const string &filename)
  : Gameboy()
{
    load(filename);
}

Gameboy::~Gameboy()
{ }

void Gameboy::load(const string &filename)
{
    ifstream is{filename, ifstream::binary};
    if (is)
        pimpl_->load(is);
}

void Gameboy::load(istream &is)
{
    pimpl_->load(is);
}

void Gameboy::run()
{
    pimpl_->run();
}

}

