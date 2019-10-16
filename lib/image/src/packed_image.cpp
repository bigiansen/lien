#include <ien/packed_image.hpp>

#include <ien/platform.hpp>
#include <stb_image.h>
#include <stdexcept>

namespace ien::img
{
    packed_image::packed_image(int width, int height)
        : _width(width)
        , _height(height)
    {
        _data = reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(width * height * 4));
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

    size_t packed_image::pixel_count() const noexcept { return _width * _height; }

    int packed_image::width() const noexcept { return _width; }
    
    int packed_image::height() const noexcept  { return _height; }
}