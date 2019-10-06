#pragma once

#include <cinttypes>
#include <string>

namespace ien::img
{
    class image
    {
    private:
        uint8_t* _data_ptr = nullptr;
        int _width = 0, _height = 0, _channels = 0;
        bool _requires_stb_free = false;

    public:
        constexpr image() noexcept { }
        image(int width, int height, int channels);
        image(const std::string& path);
        ~image();

        int width() const noexcept;
        int height() const noexcept;
        int channels() const noexcept;

        uint8_t* pixel(int idx) const noexcept;
        uint8_t* pixel(int x, int y) const noexcept;

        void set_pixel(int idx, uint8_t* data);
        void set_pixel(int x, int y, uint8_t* data);

        uint8_t* data() noexcept;
        const uint8_t* cdata() const noexcept;        
    };
}