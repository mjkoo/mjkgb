#ifndef MJKGB_HPP_
#define MJKGB_HPP_

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>

namespace mjkgb {

class Gameboy {
public:
    static constexpr int xres = 160;
    static constexpr int yres = 144;

    Gameboy();
    explicit Gameboy(const std::string &filename);
    ~Gameboy();

    using vsync_cb = std::function<void(const std::array<uint8_t, 3 * xres * yres> &)>;
    void setVsyncCallback(vsync_cb callback);

    void load(const std::string &filename);
    void load(std::istream &is);

    void run();

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif /* MJKGB_HPP_ */
