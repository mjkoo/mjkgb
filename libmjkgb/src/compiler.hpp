#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <cstdint>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>

namespace mjkgb {

using namespace llvm;

class GameboyImpl;

class Compiler {
public:
    using opcode_type = void (*)(GameboyImpl &);

    Compiler();
    
    opcode_type compile_basic_block(const std::vector<uint8_t> &bb);

private:
    std::unique_ptr<llvm::Module> module_;
};

}

#endif /* COMPILER_HPP_ */
