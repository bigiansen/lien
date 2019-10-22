#pragma once

#include <array>
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

        void set_pixel(int idx, const uint8_t* rgba);
        void set_pixel(int x, int y, const uint8_t* rgba);

        size_t pixel_count() const noexcept;
        int width() const noexcept;
        int height() const noexcept;

        bool save_to_file_png(const std::string& path, int compression_level = 8) const;
        bool save_to_file_jpeg(const std::string& path, int quality = 100) const;
        bool save_to_file_tga(const std::string& path) const;

        void resize_absolute(int w, int h);
        void resize_relative(float w, float h);
    };
}