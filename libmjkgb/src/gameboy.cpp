#include <cstdint>
#include <fstream>

#include "mjkgb.hpp"
#include "cpu.hpp"
#include "mmu.hpp"

using namespace std;

namespace mjkgb {

template<typename T> struct accessor;

/* Need to be able to pass GameboyImpl ref to accessor funcs, so can't directly
 * implement Gameboy::impl due to it being private. Also don't want to make
 * Gameboy::impl public. Use inheritance to work around this issue.
 */
class GameboyImpl {
public:
    GameboyImpl()
      : cpu_(),
        mmu_()
    { }

    template<typename T>
    typename accessor<T>::value_type get(T operand) const
    {
        return accessor<T>().get(*this, operand);
    }

    template<typename T>
    void set(T operand, typename accessor<T>::value_type value)
    {
        accessor<T>().set(*this, operand, value);
    }

    void load(const string &filename)
    {
        mmu_.load(filename);
    }

private:
    Cpu cpu_;
    Mmu mmu_;

    template<typename T> friend struct accessor;
};

}

/* Accessor functors contained in separate header. */
#include "accessors.hpp"

namespace mjkgb {

class Gameboy::impl : public GameboyImpl { };

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

