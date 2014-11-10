#ifndef MMU_HPP_
#define MMU_HPP_

#include <array>
#include <cstdint>
#include <string>

namespace mjkgb {

class Mmu {
public:
    Mmu();

    uint8_t getByte(uint16_t address) const;
    uint16_t getWord(uint16_t address) const;

    void setByte(uint16_t address, uint8_t value);
    void setWord(uint16_t address, uint16_t value);

    void load(const std::string &filename);

private:
    std::array<uint8_t, (1 << 16)> memory_;
};

}

#endif /* MMU_HPP_ */

