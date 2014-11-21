#include <sstream>

#include <gtest/gtest.h>

#include "compiler.hpp"
#include "gameboy_impl.hpp"

namespace {

using namespace std;
using namespace mjkgb;

class CompilerTest : public testing::Test {
protected:
    Compiler comp;
    GameboyImpl gb;
};

TEST_F(CompilerTest, SimpleJIT) {
    /* LD A, B; STOP */
    auto code0 = reinterpret_cast<void (*)(GameboyImpl &)>(
            comp.compile(0, { 0x78, 0x10, 0x00 }));
    ASSERT_NE(code0, nullptr);

    gb.set(ByteRegister::A, 0);
    gb.set(ByteRegister::B, 0x42);
    code0(gb);

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(ByteRegister::B));

    EXPECT_EQ(2, gb.cpu_.get_clock());
}

}

