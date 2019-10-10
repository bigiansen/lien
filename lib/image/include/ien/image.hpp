#pragma once

#include <cinttypes>
#include <string>
#include <vector>

namespace ien::img
{
    class image;
    class image_unpacked_data
    {
        friend class image;

    private:
        uint8_t* _r;
        uint8_t* _g;
        uint8_t* _b;
        uint8_t* _a;
        size_t _size;

        image_unpacked_data();
        
    public:
        image_unpacked_data(size_t pixel_count);
        ~image_unpacked_data();

        uint8_t* data_r() noexcept;
        uint8_t* data_g() noexcept;
        uint8_t* data_b() noexcept;
        uint8_t* data_a() noexcept;

        const uint8_t* cdata_r() const noexcept;
        const uint8_t* cdata_g() const noexcept;
        const uint8_t* cdata_b() const noexcept;
        const uint8_t* cdata_a() const noexcept;

        size_t size() const noexcept;

        [[nodiscard]] std::vector<uint8_t> packed_data();
    };

    extern image_unpacked_data unpack_image_data(uint8_t* data, size_t len);

    class image
    {
    private:
        image_unpacked_data _data;
        int _width, _height;

    public:
        image(int width, int height);
        image(const std::string& path);
    };
}