#include <iostream>

#include "gameboy.hpp"

using namespace std;

int
main(int argc, char **argv)
{
    Gameboy gb;

    cout << hex
        << static_cast<int>(gb.get(ByteRegister::A)) << endl
        << static_cast<int>(gb.get(WordRegister::AF)) << endl
        << static_cast<int>(gb.get(Immediate<uint8_t>(10))) << endl
        << static_cast<int>(gb.get(WordPointer<ByteRegister>(ByteRegister::A)))
        << endl;

    return 0;
}

