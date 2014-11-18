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

TEST_F(OpcodesTest, EightBitLoads) {
    /* LD A, B; STOP */
    stringstream code0{string{"\x78\x10\x00", 3}};
    gb.load(code0);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0);
    gb.set(ByteRegister::B, 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(ByteRegister::B));

    /* LD A, n */
    stringstream code1{string{"\x3e\x42\x10\x00", 4}};
    gb.load(code1);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));

    /* LD A, (HL); STOP */
    stringstream code2{string{"\x7e\x10\x00", 3}};
    gb.load(code2);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::HL, 0xff00);
    gb.set(ByteRegister::A, 0);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));

    /* LD (HL), A; STOP */
    stringstream code3{string{"\x77\x10\x00", 3}};
    gb.load(code3);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::HL, 0xff00);
    gb.set(ByteRegister::A, 0x42);
    gb.set(byte_ptr(Constant<0xff00>{}), 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0x42, gb.get(ByteRegister::A));

    /* LD (HL), n; STOP */
    stringstream code4{string{"\x36\x42\x10\x00", 4}};
    gb.load(code4);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::HL, 0xff00);
    gb.set(byte_ptr(Constant<0xff00>{}), 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));

    /* LD A, (BC); STOP */
    stringstream code5{string{"\x0a\x10\x00", 3}};
    gb.load(code5);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0);
    gb.set(WordRegister::BC, 0xff00);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));

    /* LD A, (nn); STOP */
    stringstream code6{string{"\xfa\x00\xff\x10\x00", 5}};
    gb.load(code6);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));

    /* LD (BC), A; STOP */
    stringstream code7{string{"\x02\x10\x00", 3}};
    gb.load(code7);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::BC, 0xff00);
    gb.set(ByteRegister::A, 0x42);
    gb.set(byte_ptr(Constant<0xff00>{}), 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0x42, gb.get(ByteRegister::A));

    /* LD A, (nn); STOP */
    stringstream code8{string{"\xea\x00\xff\x10\x00", 5}};
    gb.load(code8);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0x42);
    gb.set(byte_ptr(Constant<0xff00>{}), 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0x42, gb.get(ByteRegister::A));

    /* LD A, (C); STOP */
    stringstream code9{string{"\xf2\x10\x00", 3}};
    gb.load(code9);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::C, 0);
    gb.set(ByteRegister::A, 0);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));

    /* LD (C), A; STOP */
    stringstream code10{string{"\xe2\x10\x00", 3}};
    gb.load(code10);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0x42);
    gb.set(ByteRegister::C, 0);
    gb.set(byte_ptr(Constant<0xff00>{}), 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0x42, gb.get(ByteRegister::A));

    /* LDI A, (HL); STOP */
    stringstream code11{string{"\x2a\x10\x00", 3}};
    gb.load(code11);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0);
    gb.set(WordRegister::HL, 0xff00);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0xff01, gb.get(WordRegister::HL));

    /* LDI (HL), A; STOP */
    stringstream code12{string{"\x22\x10\x00", 3}};
    gb.load(code12);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0x42);
    gb.set(WordRegister::HL, 0xff00);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0xff01, gb.get(WordRegister::HL));

    /* LDD A, (HL); STOP */
    stringstream code13{string{"\x3a\x10\x00", 3}};
    gb.load(code13);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0);
    gb.set(WordRegister::HL, 0xff00);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0xfeff, gb.get(WordRegister::HL));

    /* LDD (HL), A; STOP */
    stringstream code14{string{"\x32\x10\x00", 3}};
    gb.load(code14);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0x42);
    gb.set(WordRegister::HL, 0xff00);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0xfeff, gb.get(WordRegister::HL));

    /* LDH A, (n); STOP */
    stringstream code15{string{"\xf0\x00\x10\x00", 4}};
    gb.load(code15);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0);
    gb.set(byte_ptr(Constant<0xff00>{}), 0x42);
    gb.run();

    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));

    /* LDH (n), A; STOP */
    stringstream code16{string{"\xe0\x00\x10\x00", 4}};
    gb.load(code16);

    gb.set(WordRegister::PC, 0);
    gb.set(ByteRegister::A, 0x42);
    gb.set(byte_ptr(Constant<0xff00>{}), 0);
    gb.run();

    EXPECT_EQ(0x42, gb.get(byte_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0x42, gb.get(ByteRegister::A));
}

TEST_F(OpcodesTest, SixteenBitLoads) {
    /* LD BC, nn; STOP */
    stringstream code0{string{"\x01\xef\xbe\x10\x00", 5}};
    gb.load(code0);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::BC, 0);
    gb.run();

    EXPECT_EQ(0xbeef, gb.get(WordRegister::BC));

    /* LD (nn), SP; STOP */
    stringstream code1{string{"\x08\x00\xff\x10\x00", 5}};
    gb.load(code1);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::SP, 0xbeef);
    gb.set(word_ptr(Constant<0xff00>{}), 0);
    gb.run();

    EXPECT_EQ(0xbeef, gb.get(word_ptr(Constant<0xff00>{})));
    EXPECT_EQ(0xbeef, gb.get(WordRegister::SP));

    /* LD SP, HL; STOP */
    stringstream code2{string{"\xf9\x10\x00", 3}};
    gb.load(code2);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::SP, 0);
    gb.set(WordRegister::HL, 0xbeef);
    gb.run();

    EXPECT_EQ(0xbeef, gb.get(WordRegister::SP));
    EXPECT_EQ(0xbeef, gb.get(WordRegister::HL));

    /* LD HL, (SP + e) */
    stringstream code3{string{"\xf8\x01\x10\x00", 4}};
    gb.load(code3);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::SP, 0xff00);
    gb.set(WordRegister::HL, 0);
    gb.run();

    EXPECT_EQ(0xff01, gb.get(WordRegister::HL));
    EXPECT_EQ(0xff00, gb.get(WordRegister::SP));

    /* PUSH BC; STOP */
    stringstream code4{string{"\xc5\x10\x00", 3}};
    gb.load(code4);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::BC, 0xbeef);
    gb.set(WordRegister::SP, 0xff00);
    gb.run();

    EXPECT_EQ(0xbeef, gb.get(word_ptr(Constant<0xfefe>{})));
    EXPECT_EQ(0xbeef, gb.get(WordRegister::BC));
    EXPECT_EQ(0xfefe, gb.get(WordRegister::SP));

    /* POP BC; STOP */
    stringstream code5{string{"\xc1\x10\x00", 3}};
    gb.load(code5);

    gb.set(WordRegister::PC, 0);
    gb.set(WordRegister::BC, 0);
    gb.set(WordRegister::SP, 0xfefe);
    gb.set(word_ptr(Constant<0xfefe>{}), 0xbeef);
    gb.run();

    EXPECT_EQ(0xbeef, gb.get(WordRegister::BC));
    EXPECT_EQ(0xbeef, gb.get(word_ptr(Constant<0xfefe>{})));
    EXPECT_EQ(0xff00, gb.get(WordRegister::SP));
}

}

