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
    std::array<uint8_t, (1 << 16)> memory_;
};

}

#endif /* MMU_HPP_ */

