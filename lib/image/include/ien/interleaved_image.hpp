#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/image.hpp>

#include <array>
#include <cinttypes>
#include <memory>
#include <string>

namespace ien
{ 
    class planar_image;
    class interleaved_image : public image
    {
    private:
        std::unique_ptr<ien::fixed_vector<uint8_t>> _data;

    public:
        constexpr interleaved_image()
            : image(image_type::INTERLEAVED)
        { }

        interleaved_image(const interleaved_image& cp_src);
        interleaved_image(interleaved_image&& mv_src) noexcept;

        interleaved_image(size_t width, size_t height);
        interleaved_image(const std::string& path);

        uint8_t* data() noexcept;
        const uint8_t* cdata() const noexcept;

        uint32_t get_pixel(size_t index) const override;
        uint32_t get_pixel(size_t x, size_t y) const override;

        void set_pixel(size_t idx, uint32_t rgba) override;
        void set_pixel(size_t x, size_t y, uint32_t rgba) override;

        std::vector<uint32_t> get_chunk(const rect<size_t>& r) const override;

        bool save_to_file_png(const std::string& path, int compression_level = 4) const override;
        bool save_to_file_jpeg(const std::string& path, int quality = 100) const override;
        bool save_to_file_tga(const std::string& path) const override;

        ien::fixed_vector<uint8_t> save_to_memory_png(int compression_level = 4) const override;
        ien::fixed_vector<uint8_t> save_to_memory_jpeg(int quality = 100) const override;
        ien::fixed_vector<uint8_t> save_to_memory_tga() const override;

        void resize_absolute(size_t w, size_t h) override;
        void resize_relative(float w, float h) override;

        ien::fixed_vector<uint8_t> get_rgba_buff_copy();

        ien::planar_image to_planar_image();

        interleaved_image& operator=(const interleaved_image& cp_src);
        interleaved_image& operator=(interleaved_image&& mv_src) noexcept;
    };
}