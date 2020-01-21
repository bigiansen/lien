#include <ien/packed_image.hpp>

#include <ien/platform.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <cstring>
#include <stdexcept>

#define PKIMG_ALIGNMENT LIEN_DEFAULT_ALIGNMENT

namespace ien
{
    packed_image::packed_image(int width, int height)
        : _width(width)
        , _height(height)
    { 
        _data = std::make_unique<ien::fixed_vector<uint8_t>>(
            static_cast<size_t>(_width * _height * 4),
            PKIMG_ALIGNMENT
        );
    }

    packed_image::packed_image(const std::string& path)
    {
        int ch_dummy;
        uint8_t* stbdata = stbi_load(path.c_str(), &_width, &_height, &ch_dummy, 4);
        if(stbdata == nullptr)
        {
            throw std::invalid_argument("Invalid image path or file format");
        }

        _data = std::make_unique<ien::fixed_vector<uint8_t>>(
            static_cast<size_t>(_width * _height * 4),
            PKIMG_ALIGNMENT
        );

        std::copy(stbdata, stbdata + (_width * _height * 4), _data->data());
    }

    packed_image::packed_image(packed_image&& mv_src)
        : _data(std::move(mv_src._data))
        , _width(mv_src._height)
        , _height(mv_src._height)
    {
        _width = 0;
        _height = 0;
    }

    uint8_t* packed_image::data() noexcept { return _data->data(); }

    const uint8_t* packed_image::cdata() const noexcept { return _data->cdata(); }

    void packed_image::set_pixel(int idx, const uint8_t* rgba)
    {
        uint8_t* dataptr = LIEN_HINT_ALIGNED(_data->data(), PKIMG_ALIGNMENT);
        std::memcpy(dataptr + (static_cast<size_t>(idx) * 4), rgba, 4);
    }
    
    void packed_image::set_pixel(int x, int y, const uint8_t* rgba)
    {
        uint8_t* dataptr = LIEN_HINT_ALIGNED(_data->data(), PKIMG_ALIGNMENT);
        std::memcpy(dataptr + (static_cast<size_t>(x) * y * _width * 4), rgba, 4);
    }

    size_t packed_image::pixel_count() const noexcept { return static_cast<size_t>(_width) * _height; }

    int packed_image::width() const noexcept { return _width; }
    
    int packed_image::height() const noexcept  { return _height; }

    bool packed_image::save_to_file_png(const std::string& path, int compression_level) const
    {
        stbi_write_png_compression_level = compression_level;
        const uint8_t* dataptr = LIEN_HINT_ALIGNED(_data->cdata(), PKIMG_ALIGNMENT);
        return stbi_write_png(path.c_str(), _width, _height, 4, dataptr, _width * 4);
    }

    bool packed_image::save_to_file_jpeg(const std::string& path, int quality) const
    {
        const uint8_t* dataptr = LIEN_HINT_ALIGNED(_data->cdata(), PKIMG_ALIGNMENT);
        return stbi_write_jpg(path.c_str(), _width, _height, 4, dataptr, quality);
    }

    bool packed_image::save_to_file_tga(const std::string& path) const
    {
        const uint8_t* dataptr = LIEN_HINT_ALIGNED(_data->cdata(), PKIMG_ALIGNMENT);
        return stbi_write_tga(path.c_str(), _width, _height, 4, dataptr);
    }

    void packed_image::resize_absolute(int w, int h)
    {
        std::unique_ptr<data_t> resized_data = std::make_unique<data_t>(
            static_cast<uint8_t>(w) * h * 4, 
            PKIMG_ALIGNMENT
        );

        stbir_resize_uint8(_data->cdata(), _width, _height, 4, resized_data->data(), w, h, 4, 4);
        _data = std::move(resized_data);
    }

    void packed_image::resize_relative(float w, float h)
    {
        int real_w = static_cast<int>(float(_width) * w);
        int real_h = static_cast<int>(float(_height) * h);

        resize_absolute(real_w, real_h);
    }
}