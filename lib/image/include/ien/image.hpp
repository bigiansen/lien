#pragma once

#include <ien/image_unpacked_data.hpp>

#include <array>
#include <cinttypes>
#include <string>
#include <vector>

namespace ien
{
    class packed_image;

    class image
    {
    private:
        image_unpacked_data _data;
        int _width, _height;

    public:
        image(int width, int height);
        image(const std::string& path);

        image(const image& cp_src);

        image_unpacked_data* data() noexcept;
        const image_unpacked_data* cdata() const noexcept;

        size_t pixel_count() const noexcept;
        int width() const noexcept;
        int height() const noexcept;

        std::array<uint8_t, 4> get_packed_pixel(int index) const;
        std::array<uint8_t, 4> get_packed_pixel(int x, int y) const;

        void set_pixel(int index, const uint8_t* rgba);
        void set_pixel(int x, int y, const uint8_t* rgba);

        bool save_to_file_png(const std::string& path, int compression_level = 4) const;
        bool save_to_file_jpeg(const std::string& path, int quality = 100) const;
        bool save_to_file_tga(const std::string& path) const;

        void resize_absolute(int w, int h);
        void resize_relative(float w, float h);

        packed_image to_packed_image();
    };
}