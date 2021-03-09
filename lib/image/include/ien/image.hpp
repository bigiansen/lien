#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/rect.hpp>

#include <cinttypes>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace ien
{
    enum class image_type
    {
        INTERLEAVED,
        PLANAR
    };

    class image
    {
    protected:
        size_t _width, _height;
        image_type _imgtype;

    public:
        constexpr image(image_type imgtype)
            : _width(0)
            , _height(0)
            , _imgtype(imgtype)
        { }

        constexpr image(size_t w, size_t h, image_type imgtype)
            : _width(w)
            , _height(h)
            , _imgtype(imgtype)
        { }

        inline size_t width() const noexcept { return _width; }
        inline size_t height() const noexcept { return _height; }

        size_t pixel_count() const noexcept;
        size_t size() const noexcept;

        virtual uint32_t get_pixel(size_t idx) const = 0;
        virtual uint32_t get_pixel(size_t x, size_t y) const = 0;
 
        virtual std::vector<uint32_t> get_chunk(const rect<size_t>& r) const = 0;
 
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

        inline image_type imgtype() const { return _imgtype; }
    };

    extern std::unique_ptr<ien::image> read_image(const std::string& imgpath, image_type type);
}