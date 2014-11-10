#include <cstdint>
#include <fstream>

#include "mjkgb.hpp"
#include "cpu.hpp"

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
      : cpu_()
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

    }

private:
    Cpu cpu_;

    template<typename T> friend struct accessor;
};

class Gameboy::impl : public GameboyImpl { };

}

/* Accessor functors contained in separate header. */
#include "accessors.hpp"

/* Gameboy pimpl wrappers contained in separate header. */
#include "gameboy.hpp"

