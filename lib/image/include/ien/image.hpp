#pragma once

#include <cinttypes>
#include <string>
#include <vector>

namespace ien::img
{
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// class image_unpacked_data
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
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

        [[nodiscard]] std::vector<uint8_t> pack_data();
    };

    extern image_unpacked_data unpack_image_data(uint8_t* data, size_t len);

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// class image
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    class image
    {
    private:
        image_unpacked_data _data;
        int _width, _height;

    public:
        image(int width, int height);
        image(const std::string& path);

        image_unpacked_data* data() noexcept;
        const image_unpacked_data* cdata() const noexcept;

        size_t pixel_count() const noexcept;
        int width() const noexcept;
        int height() const noexcept;

        void save_to_file_png(const std::string& path, int compression_level = 8);
        void save_to_file_jpeg(const std::string& path, int quality = 100);
        void save_to_file_tga(const std::string& path);
    };
}