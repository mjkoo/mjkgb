#ifndef CPU_HPP_
#define CPU_HPP_

#include <array>
#include <cstdint>

#include "operands.hpp"

namespace mjkgb {

class Cpu {
public:
    Cpu();

    inline uint8_t get(ByteRegister reg) const
    {
        return registers_[static_cast<size_t>(reg)];
    }

    inline uint16_t get(WordRegister reg) const
    {
        auto index = 2 * static_cast<size_t>(reg);
        return static_cast<uint16_t>(registers_[index] << 8 | registers_[index + 1]);
    }

    inline void set(ByteRegister reg, uint8_t value)
    {
        registers_[static_cast<size_t>(reg)] = value;
    }

    inline void set(WordRegister reg, uint16_t value, bool tick = true)
    {
        auto index = 2 * static_cast<size_t>(reg);
        registers_[index] = (value >> 8) & 0xff;
        if (tick)
            this->tick();
        registers_[index + 1] = value & 0xff;
    }

    void tick();
    void stop();
    void halt();
    void enable_interrupts();
    void disable_interrupts();

private:
    static constexpr int num_registers =
        2 * (static_cast<size_t>(WordRegister::SP) + 1);

    unsigned long clock_;
    std::array<uint8_t, num_registers> registers_;
};

}

#endif /* CPU_HPP_ */

