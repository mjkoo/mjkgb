#include <sstream>

#include <gtest/gtest.h>

#include "compiler.hpp"

namespace {

using namespace std;
using namespace mjkgb;

class CompilerTest : public testing::Test {
protected:
    Compiler comp;
};

TEST_F(CompilerTest, SimpleJIT) {
    /* LD A, B; STOP */
    comp.compile(0, { 0x78, 0x10, 0x00 });
}

TEST_F(CompilerTest, SimpleJIT2) {
    comp.compile(0, { 0x78, 0x10, 0x00 });
}

}

