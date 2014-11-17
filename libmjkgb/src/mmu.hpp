#ifndef MMU_HPP_
#define MMU_HPP_

#include <array>
#include <cstdint>
#include <istream>
#include <string>

namespace mjkgb {

class Mmu {
public:
    Mmu()
      : memory_()
    { }

    uint8_t get(uint16_t address) const
    {
        return memory_[address];
    }

    void set(uint16_t address, uint8_t value)
    {
        memory_[address] = value;
    }

    void load(std::istream &is)
    {
        memory_.fill(0);
        is.read(reinterpret_cast<char *>(memory_.data()),
                static_cast<long>(memory_.size()));
    }

private:
    static constexpr int memory_size = 1 << 16;

    std::array<uint8_t, memory_size> memory_;
};

}

#endif /* MMU_HPP_ */

