#include <sstream>

#include <gtest/gtest.h>

#include "gameboy_impl.hpp"

namespace {

using namespace std;
using namespace mjkgb;

class OpcodesTest : public testing::Test {
protected:
    GameboyImpl gb;
};

TEST_F(OpcodesTest, LD) {
    /* LD B, A; STOP */
    stringstream code{string{"\x47\x10\x00", 3}};
    gb.load(code);

    gb.set(ByteRegister::A, 0x42);
    gb.set(ByteRegister::B, 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(ByteRegister::B));
}

}

