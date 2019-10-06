#include <ien/image.hpp>

#include <ien/assert.hpp>
#include <ien/platform.hpp>
#include <cassert>
#include <stdexcept>
#include <stb_image.h>

namespace ien::img
{
    image::image(int w, int h, int ch)
        : _width(w)
        , _height(h)
        , _channels(ch)
        , _requires_stb_free(false)
    {
        _data_ptr = new uint8_t[w * h * ch];
    }

    image::image(const std::string& path)
    {
        _data_ptr = stbi_load(
            path.c_str(), 
            &_width, 
            &_height, 
            &_channels, 
            4
        );
        _channels = 4;

        if(_data_ptr == nullptr)
            throw std::invalid_argument("Could not open image at path:" + path);
    }

    image::~image()
    {
        if(_requires_stb_free)
            stbi_image_free(_data_ptr);
        else
            delete[](_data_ptr);
    }

    int image::width() const noexcept
    {
        return _channels;
    }

    int image::height() const noexcept
    {
        return _channels;
    }

    int image::channels() const noexcept
    {
        return _channels;
    }

    uint8_t* image::pixel(int idx) const noexcept
    {
        ien::debug_assert(idx < ((_width * _height) - 1), "Pixel index out of range!");
        return _data_ptr + (idx * _channels);
    }

    uint8_t* image::pixel(int x, int y) const noexcept
    {
        ien::debug_assert((x * y) < ((_width * _height) - 1), "Pixel index out of range!");
        return _data_ptr + (x * _channels) + (y * _width * _channels);
    }

    void image::set_pixel(int idx, uint8_t* data)
    {
        ien::debug_assert(idx < ((_width * _height) - 1), "Pixel index out of range!");
        for(int i = 0; i < _channels; ++i)
        {
            int data_offset = (idx * _channels);
            _data_ptr[data_offset + i] = data[i];
        }
    }

    void image::set_pixel(int x, int y, uint8_t* data)
    {
        ien::debug_assert((x * y) < ((_width * _height) - 1), "Pixel index out of range!");
        for(int i = 0; i < _channels; ++i)
        {
            int data_offset = (x * _channels) + (y * _width * _channels);
            _data_ptr[data_offset + i] = data[i];
        }
    }

    uint8_t* image::data() noexcept 
    { return _data_ptr; }

    const uint8_t* image::cdata() const noexcept 
    { return _data_ptr; }
}