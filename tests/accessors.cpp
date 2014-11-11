#include <gtest/gtest.h>

#include "gameboy_impl.hpp"

using namespace mjkgb;

class AccessorsTest : public testing::Test {
protected:
    GameboyImpl gb;
};

TEST_F(AccessorsTest, ByteRegister) {
    EXPECT_EQ(0, gb.get(ByteRegister::A));

    gb.set(ByteRegister::A, 0xde);
    EXPECT_EQ(0xde, gb.get(ByteRegister::A));

    gb.set(ByteRegister::A, gb.get(ByteRegister::A) + 1);
    EXPECT_EQ(0xdf, gb.get(ByteRegister::A));

    EXPECT_EQ(0, gb.get(ByteRegister::E));

    gb.set(ByteRegister::E, 0xad);
    EXPECT_EQ(0xad, gb.get(ByteRegister::E));

    gb.set(ByteRegister::E, gb.get(ByteRegister::E) + 1);
    EXPECT_EQ(0xae, gb.get(ByteRegister::E));
}

TEST_F(AccessorsTest, WordRegister) {
    EXPECT_EQ(0, gb.get(WordRegister::AF));

    gb.set(WordRegister::AF, 0xdead);
    EXPECT_EQ(0xdead, gb.get(WordRegister::AF));
    EXPECT_EQ(0xde, gb.get(ByteRegister::A));
    EXPECT_EQ(0xad, gb.get(ByteRegister::F));

    gb.set(WordRegister::AF, gb.get(WordRegister::AF) + 1);
    EXPECT_EQ(0xdeae, gb.get(WordRegister::AF));
    EXPECT_EQ(0xde, gb.get(ByteRegister::A));
    EXPECT_EQ(0xae, gb.get(ByteRegister::F));

    EXPECT_EQ(0, gb.get(WordRegister::DE));

    gb.set(WordRegister::DE, 0xbeef);
    EXPECT_EQ(0xbeef, gb.get(WordRegister::DE));
    EXPECT_EQ(0xbe, gb.get(ByteRegister::D));
    EXPECT_EQ(0xef, gb.get(ByteRegister::E));

    gb.set(WordRegister::DE, gb.get(WordRegister::DE) + 1);
    EXPECT_EQ(0xbef0, gb.get(WordRegister::DE));
    EXPECT_EQ(0xbe, gb.get(ByteRegister::D));
    EXPECT_EQ(0xf0, gb.get(ByteRegister::E));
}

TEST_F(AccessorsTest, ConditionCode) {
    EXPECT_EQ(false, gb.get(ConditionCode::C));

    gb.set(ConditionCode::C, true);
    EXPECT_EQ(true, gb.get(ConditionCode::C));

    gb.set(ConditionCode::C, !gb.get(ConditionCode::C));
    EXPECT_EQ(false, gb.get(ConditionCode::C));

    EXPECT_EQ(false, gb.get(ConditionCode::Z));

    gb.set(ConditionCode::Z, true);
    EXPECT_EQ(true, gb.get(ConditionCode::Z));

    gb.set(ConditionCode::Z, !gb.get(ConditionCode::Z));
    EXPECT_EQ(false, gb.get(ConditionCode::Z));
}

TEST_F(AccessorsTest, BytePointer) {
    auto ptr0 = BytePointer<ByteRegister, 0>(ByteRegister::A);
    EXPECT_EQ(0, gb.get(ptr0));

    gb.set(ptr0, 0xde);
    EXPECT_EQ(0xde, gb.get(ptr0));

    gb.set(ptr0, gb.get(ptr0) + 1);
    EXPECT_EQ(0xdf, gb.get(ptr0));

    auto ptr1 = BytePointer<WordRegister, 0>(WordRegister::HL);
    EXPECT_EQ(0, gb.get(ptr1));

    gb.set(WordRegister::HL, 0xff00);
    EXPECT_EQ(0xdf, gb.get(ptr1));

    gb.set(ptr1, 0xbe);
    EXPECT_EQ(0xbe, gb.get(ptr1));

    gb.set(ptr1, gb.get(ptr1) + 1);
    EXPECT_EQ(0xbf, gb.get(ptr1));

    auto ptr2 = BytePointer<WordRegister, 1>(WordRegister::HL);
    EXPECT_EQ(0xff00, gb.get(WordRegister::HL));
    EXPECT_EQ(0xbf, gb.get(ptr2));
    EXPECT_EQ(0xff01, gb.get(WordRegister::HL));
    EXPECT_EQ(0x0, gb.get(ptr2));
    EXPECT_EQ(0xff02, gb.get(WordRegister::HL));

    auto ptr3 = BytePointer<WordRegister, -1>(WordRegister::HL);
    EXPECT_EQ(0xff02, gb.get(WordRegister::HL));
    EXPECT_EQ(0x0, gb.get(ptr3));
    EXPECT_EQ(0xff01, gb.get(WordRegister::HL));
    EXPECT_EQ(0x0, gb.get(ptr3));
    EXPECT_EQ(0xff00, gb.get(WordRegister::HL));
    EXPECT_EQ(0xbf, gb.get(ptr3));
}

TEST_F(AccessorsTest, WordPointer) {
    auto ptr0 = WordPointer<ByteRegister, 0>(ByteRegister::A);
    EXPECT_EQ(0, gb.get(ptr0));

    gb.set(ptr0, 0xdead);
    EXPECT_EQ(0xdead, gb.get(ptr0));

    gb.set(ptr0, gb.get(ptr0) + 1);
    EXPECT_EQ(0xdeae, gb.get(ptr0));

    auto ptr1 = WordPointer<WordRegister, 0>(WordRegister::HL);
    EXPECT_EQ(0, gb.get(ptr1));

    gb.set(WordRegister::HL, 0xff00);
    EXPECT_EQ(0xdeae, gb.get(ptr1));

    gb.set(ptr1, 0xbeef);
    EXPECT_EQ(0xbeef, gb.get(ptr1));

    gb.set(ptr1, gb.get(ptr1) + 1);
    EXPECT_EQ(0xbef0, gb.get(ptr1));

    auto ptr2 = WordPointer<WordRegister, 1>(WordRegister::HL);
    EXPECT_EQ(0xff00, gb.get(WordRegister::HL));
    EXPECT_EQ(0xbef0, gb.get(ptr2));
    EXPECT_EQ(0xff01, gb.get(WordRegister::HL));
    EXPECT_EQ(0xbe, gb.get(ptr2));
    EXPECT_EQ(0xff02, gb.get(WordRegister::HL));

    auto ptr3 = WordPointer<WordRegister, -1>(WordRegister::HL);
    EXPECT_EQ(0xff02, gb.get(WordRegister::HL));
    EXPECT_EQ(0x0, gb.get(ptr3));
    EXPECT_EQ(0xff01, gb.get(WordRegister::HL));
    EXPECT_EQ(0xbe, gb.get(ptr3));
    EXPECT_EQ(0xff00, gb.get(WordRegister::HL));
    EXPECT_EQ(0xbef0, gb.get(ptr3));
}

TEST_F(AccessorsTest, ByteImmediate) {
    EXPECT_EQ(0, gb.get(WordRegister::PC));
    EXPECT_EQ(0, gb.get(ByteImmediate()));
    EXPECT_EQ(1, gb.get(WordRegister::PC));

    gb.set(WordRegister::HL, 1);
    gb.set(BytePointer<WordRegister>(WordRegister::HL), 0xde);
    EXPECT_EQ(0xde, gb.get(ByteImmediate()));
    EXPECT_EQ(2, gb.get(WordRegister::PC));
}

TEST_F(AccessorsTest, WordImmediate) {
    EXPECT_EQ(0, gb.get(WordRegister::PC));
    EXPECT_EQ(0, gb.get(WordImmediate()));
    EXPECT_EQ(2, gb.get(WordRegister::PC));

    gb.set(WordRegister::HL, 2);
    gb.set(WordPointer<WordRegister>(WordRegister::HL), 0xdead);
    EXPECT_EQ(0xdead, gb.get(WordImmediate()));
    EXPECT_EQ(4, gb.get(WordRegister::PC));
}

