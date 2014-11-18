#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>

namespace mjkgb {

using namespace llvm;

struct GameboyImpl;

class Compiler {
public:
    Compiler();
    
    std::function<void(GameboyImpl &)> compile_basic_block(const std::vector<uint8_t> &bb);

private:
    std::unique_ptr<llvm::Module> module_;
};

}

#endif /* COMPILER_HPP_ */
