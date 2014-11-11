#ifndef CPU_HPP_
#define CPU_HPP_

#include <array>
#include <cstdint>

#include "operands.hpp"

namespace mjkgb {

class Cpu {
public:
    Cpu();

    uint8_t get(ByteRegister reg) const;
    uint16_t get(WordRegister reg) const;

    void set(ByteRegister reg, uint8_t value);
    void set(WordRegister reg, uint16_t value);

    void tick();

private:
    static constexpr int num_registers =
        2 * (static_cast<size_t>(WordRegister::SP) + 1);

    std::array<uint8_t, num_registers> registers_;
};

}

#endif /* CPU_HPP_ */

