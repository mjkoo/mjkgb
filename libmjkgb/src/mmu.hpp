#ifndef MMU_HPP_
#define MMU_HPP_

#include <array>
#include <cstdint>
#include <string>

namespace mjkgb {

class Mmu {
public:
    Mmu();

    uint8_t get(uint16_t address) const;
    void set(uint16_t address, uint8_t value);

    void load(const std::string &filename);

private:
    static constexpr int memory_size = 1 << 16;

    std::array<uint8_t, memory_size> memory_;
};

}

#endif /* MMU_HPP_ */

