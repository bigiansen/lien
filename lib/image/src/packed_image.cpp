#include <ien/packed_image.hpp>

#include <ien/platform.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>

#include <cstring>
#include <stdexcept>

namespace ien::img
{
    packed_image::packed_image(int width, int height)
        : _width(width)
        , _height(height)
    {
        _data = reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(size_t(width) * height * 4));
    }

    packed_image::packed_image(const std::string& path)
    {
        int ch_dummy;
        _data = stbi_load(path.c_str(), &_width, &_height, &ch_dummy, 4);
        if(_data == nullptr)
        {
            throw std::invalid_argument("Invalid image path or file format");
        }
        _stb_free = true;
    }

    packed_image::~packed_image()
    {
        if(_stb_free)
        {
            stbi_image_free(_data);
        }
        else
        {
            LIEN_ALIGNED_FREE(_data);
        }
    }

    uint8_t* packed_image::data() noexcept { return _data; }

    const uint8_t* packed_image::cdata() const noexcept { return _data; }

    void packed_image::set_pixel(int idx, uint8_t* rgba)
    {
        std::memcpy(_data + (size_t(idx) * 4), rgba, 4);
    }
    
    void packed_image::set_pixel(int x, int y, uint8_t* rgba)
    {
        std::memcpy(_data + (size_t(x) * y * _width * 4), rgba, 4);
    }

    size_t packed_image::pixel_count() const noexcept { return size_t(_width) * _height; }

    int packed_image::width() const noexcept { return _width; }
    
    int packed_image::height() const noexcept  { return _height; }

    void packed_image::resize_absolute(int w, int h)
    {
        uint8_t* resized_data = reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(size_t(w) * h * 4));
        stbir_resize_uint8(_data, _width, _height, 4, resized_data, w, h, 4, 4);

        if(_stb_free)
        {
            LIEN_ALIGNED_FREE(_data);
        }
        else
        {
            stbi_image_free(_data);
        }

        _data = resized_data;
    }

    void packed_image::resize_relative(float w, float h)
    {
        int real_w = static_cast<int>(float(_width) * w);
        int real_h = static_cast<int>(float(_height) * h);

        resize_absolute(real_w, real_h);
    }
}