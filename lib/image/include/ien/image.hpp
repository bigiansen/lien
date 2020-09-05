#pragma once

#include <ien/generic_image.hpp>
#include <ien/image_unpacked_data.hpp>

#include <array>
#include <cinttypes>
#include <string>
#include <vector>

namespace ien
{
    class packed_image;

    class image : public generic_image
    {
    private:
        image_unpacked_data _data;

    public:
        constexpr image() 
            : generic_image()
        { }

        image(size_t width, size_t height);
        image(const std::string& path);

        image(const image& cp_src) = default;
        image(image&& mv_src) = default;

        image(const uint8_t* rgba_buff, size_t w, size_t h);

        image_unpacked_data* data() noexcept;
        const image_unpacked_data* cdata() const noexcept;

        uint32_t get_pixel(size_t index) const override;
        uint32_t get_pixel(size_t x, size_t y) const override;

        void set_pixel(size_t idx, uint32_t px) override;
        void set_pixel(size_t x, size_t y, uint32_t px) override;

        bool save_to_file_png(const std::string& path, int compression_level = 4) const override;
        bool save_to_file_jpeg(const std::string& path, int quality = 100) const override;
        bool save_to_file_tga(const std::string& path) const override;

        ien::fixed_vector<uint8_t> save_to_memory_png(int compression_level = 4) const override;
        ien::fixed_vector<uint8_t> save_to_memory_jpeg(int quality = 100) const override;
        ien::fixed_vector<uint8_t> save_to_memory_tga() const override;

        void resize_absolute(size_t w, size_t h) override;
        void resize_relative(float w, float h) override;

        packed_image to_packed_image();
        std::string to_png_base64(int comp_level = 4);

        image& operator=(const image& cp_src);
        image& operator=(image&& mv_src) noexcept;
    };
}