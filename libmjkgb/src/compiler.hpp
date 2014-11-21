#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <memory>
#include <vector>

namespace mjkgb {

class Compiler {
public:
    Compiler();
    ~Compiler();

    uintptr_t compile(uint16_t address, const std::vector<uint8_t> &block);

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif /* COMPILER_HPP_ */
