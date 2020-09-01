#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/generic_image.hpp>

#include <array>
#include <cinttypes>
#include <memory>
#include <string>

namespace ien
{ 
    class image;

    class packed_image : public generic_image
    {
    private:
        std::unique_ptr<ien::fixed_vector<uint8_t>> _data;

    public:
        constexpr packed_image()
            : generic_image()
        { }

        packed_image(const packed_image& cp_src);
        packed_image(packed_image&& mv_src) noexcept;

        packed_image(size_t width, size_t height);
        packed_image(const std::string& path);

        uint8_t* data() noexcept;
        const uint8_t* cdata() const noexcept;

        uint32_t get_pixel(size_t index) const override;
        uint32_t get_pixel(size_t x, size_t y) const override;

        void set_pixel(size_t idx, uint32_t rgba) override;
        void set_pixel(size_t x, size_t y, uint32_t rgba) override;

        bool save_to_file_png(const std::string& path, int compression_level = 4) const override;
        bool save_to_file_jpeg(const std::string& path, int quality = 100) const override;
        bool save_to_file_tga(const std::string& path) const override;

        ien::fixed_vector<uint8_t> save_to_memory_png(int compression_level = 4) const override;
        ien::fixed_vector<uint8_t> save_to_memory_jpeg(int quality = 100) const override;
        ien::fixed_vector<uint8_t> save_to_memory_tga() const override;

        void resize_absolute(size_t w, size_t h) override;
        void resize_relative(float w, float h) override;

        ien::fixed_vector<uint8_t> get_rgba_buff_copy();

        ien::image to_image();

        packed_image& operator=(const packed_image& cp_src);
        packed_image& operator=(packed_image&& mv_src) noexcept;
    };
}