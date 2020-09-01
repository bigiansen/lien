#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/rect.hpp>

#include <cinttypes>
#include <cstddef>
#include <string>
#include <vector>

namespace ien
{
    class generic_image
    {
    protected:
        size_t _width, _height;

    public:
        constexpr generic_image()
            : _width(0)
            , _height(0)
        { }

        constexpr generic_image(size_t w, size_t h)
            : _width(w)
            , _height(h)
        { }

        inline size_t width() const noexcept { return _width; }
        inline size_t height() const noexcept { return _height; }

        size_t pixel_count() const noexcept;
        size_t size() const noexcept;

        virtual uint32_t get_pixel(size_t idx) const = 0;
        virtual uint32_t get_pixel(size_t x, size_t y) const = 0;
 
        std::vector<uint32_t> get_chunk(const rect<size_t>& r) const;
 
        virtual void set_pixel(size_t idx, uint32_t px) = 0;
        virtual void set_pixel(size_t x, size_t y, uint32_t px) = 0;

        virtual bool save_to_file_png(const std::string& path, int compression_level = 4) const = 0;
        virtual bool save_to_file_jpeg(const std::string& path, int quality = 100) const = 0;
        virtual bool save_to_file_tga(const std::string& path) const = 0;

        virtual ien::fixed_vector<uint8_t> save_to_memory_png(int compression_level = 4) const = 0;
        virtual ien::fixed_vector<uint8_t> save_to_memory_jpeg(int quality = 100) const = 0;
        virtual ien::fixed_vector<uint8_t> save_to_memory_tga() const = 0;

        virtual void resize_absolute(size_t w, size_t h) = 0;
        virtual void resize_relative(float w, float h) = 0;
    };
}