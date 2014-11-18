#ifndef MMU_HPP_
#define MMU_HPP_

#include <array>
#include <atomic>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace mjkgb {

struct GameboyImpl;

class Mmu {
public:
    Mmu()
      : memory_(),
        native_()
    { }

    inline uint8_t get(uint16_t address) const
    {
        return memory_[address];
    }

    inline void set(uint16_t address, uint8_t value)
    {
        memory_[address] = value;
    }

    inline uintptr_t get_native(uint16_t address)
    {
        return native_[address].load();
    }

    void set_native(uint16_t address, uintptr_t func);

    void load(std::istream &is);

private:
    static constexpr int memory_size = 1 << 16;

    std::array<uint8_t, memory_size> memory_;
    std::array<std::atomic_uintptr_t, memory_size> native_;
};

}

#endif /* MMU_HPP_ */

