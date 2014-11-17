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
    stringstream code0{string{"\x47\x10\x00", 3}};
    gb.load(code0);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 42);
    gb.set(ByteRegister::B, 0);
    gb.run();

    EXPECT_EQ(42, gb.get(ByteRegister::A));
    EXPECT_EQ(42, gb.get(ByteRegister::B));

    /* LD (HL), A; STOP */
    stringstream code1{string{"\x77\x10\x00", 3}};
    gb.load(code1);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 42);
    gb.set(WordRegister::HL, 0xff00);
    gb.run();

    EXPECT_EQ(42, gb.get(ByteRegister::A));
    EXPECT_EQ(42, gb.get(byte_ptr(Constant<0xff00>{})));
}

}

