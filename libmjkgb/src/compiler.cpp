#include <cstdint>
#include <vector>

#include <llvm/ADT/StringRef.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>

#include "compiler.hpp"

namespace mjkgb {

using namespace std;
using namespace llvm;

extern char _binary_opcodes_bc_start[];
extern size_t _binary_opcodes_bc_size;

Compiler::Compiler()
  : module_()
{
    auto sr = StringRef{_binary_opcodes_bc_start, _binary_opcodes_bc_size};
    auto buffer = MemoryBuffer::getMemBuffer(sr);
//    module_ = parseBitcodeFile(buffer);
}

Compiler::opcode_type Compiler::compile_basic_block(const vector<uint8_t> &bb)
{
    return nullptr;
}

}

