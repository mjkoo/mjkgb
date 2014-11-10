#ifndef MJKGB_HPP_
#define MJKGB_HPP_

#include <cstdint>
#include <memory>
#include <string>

namespace mjkgb {

enum class Register {
    A, F, B, C, D, E, AF, BC, DE, HL, SP
};

class Gameboy {
public:
    Gameboy();
    explicit Gameboy(const std::string &filename);
    ~Gameboy();

    typedef std::function<void(const std::array<uint32_t, 160 * 144> &)> vsync_cb;
    void setVsyncCallback(vsync_cb callback);

    void load(const std::string &filename);
    void reset();
    void run();
    void pause();
    void stop();

    bool saveState(std::ostream &os);
    bool saveState(const std::string &filename);
    bool loadState(std::istream &is);
    bool loadState(const std::string &filename);

    void addBreakpoint(uint16_t address);
    void removeBreakpoint(uint16_t address);
    void addWatchpoint(uint16_t address);
    void removeWatchpoint(uint16_t address);

    uint16_t peekRegister(Register reg);
    void pokeRegsiter(Register reg, uint16_t value);
    uint8_t peekMemory(uint16_t address);
    void pokeMemory(uint16_t address, uint8_t value);

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif /* MJKGB_HPP_ */
