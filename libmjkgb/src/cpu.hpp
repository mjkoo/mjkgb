#ifndef CPU_HPP_
#define CPU_HPP_

#include <array>
#include <cstdint>

#include "operands.hpp"

class Cpu {
public:
    Cpu();

    uint8_t get(ByteRegister reg) const;
    uint16_t get(WordRegister reg) const;

    void set(ByteRegister reg, uint8_t value);
    void set(WordRegister reg, uint16_t value);

private:
    std::array<uint8_t, 2 * (static_cast<size_t>(WordRegister::SP) + 1)> registers_;
};

#endif /* CPU_HPP_ */

