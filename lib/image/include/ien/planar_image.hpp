#pragma once

#include <ien/image.hpp>
#include <ien/image_planar_data.hpp>

#include <array>
#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

namespace ien
{
    class interleaved_image;
    class planar_image : public image
    {
    private:
        image_planar_data _data;

    public:
        constexpr planar_image() 
            : image(image_type::PLANAR)
        { }

        planar_image(size_t width, size_t height);
        planar_image(const std::string& path);

        planar_image(const planar_image& cp_src) = default;
        planar_image(planar_image&& mv_src) = default;

        planar_image(const uint8_t* rgba_buff, size_t w, size_t h);

        image_planar_data* data() noexcept;
        const image_planar_data* cdata() const noexcept;

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

        interleaved_image to_interleaved_image();
        std::string to_png_base64(int comp_level = 4);

        planar_image& operator=(const planar_image& cp_src);
        planar_image& operator=(planar_image&& mv_src) noexcept;
    };
}