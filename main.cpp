#include <iostream>

#include "gbz80.hpp"

using namespace std;

int
main(int argc, char **argv)
{
    GbZ80 cpu;

    cout << hex
        << static_cast<int>(cpu.get(ByteRegister::A)) << endl
        << static_cast<int>(cpu.get(WordRegister::AF)) << endl
        << static_cast<int>(cpu.get(Immediate<uint8_t>(10))) << endl
        << static_cast<int>(cpu.get(WordPointer<ByteRegister>(ByteRegister::A)))
        << endl;

    return 0;
}

