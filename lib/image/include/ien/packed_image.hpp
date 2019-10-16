#pragma once

#include <cinttypes>
#include <string>

namespace ien::img
{
    class packed_image
    {
    private:
        uint8_t* _data;
        int _width, _height;
        bool _stb_free = false;

    public:
        packed_image(int width, int height);
        packed_image(const std::string& path);
        ~packed_image();

        uint8_t* data() noexcept;
        const uint8_t* cdata() const noexcept;

        size_t pixel_count() const noexcept;
        int width() const noexcept;
        int height() const noexcept;
    };
}