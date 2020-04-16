#include <ien/packed_image.hpp>

#include <ien/alignment.hpp>
#include <ien/arithmetic.hpp>
#include <ien/platform.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <cstring>
#include <stdexcept>

namespace ien
{
    packed_image::packed_image(int width, int height)
        : _width(width)
        , _height(height)
    { 
        _data = std::make_unique<ien::fixed_vector<uint8_t>>(
            safe_mul<size_t>(_width, _height , 4),
            LIEN_DEFAULT_ALIGNMENT
        );
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());
    }

    packed_image::packed_image(const std::string& path)
    {
        int ch_dummy;
        uint8_t* stbdata = stbi_load(path.c_str(), &_width, &_height, &ch_dummy, 4);
        if(stbdata == nullptr)
        {
            throw std::invalid_argument("Invalid image path or file format");
        }
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, stbdata);

        _data = std::make_unique<ien::fixed_vector<uint8_t>>(
            safe_mul<size_t>(_width, _height , 4),
            LIEN_DEFAULT_ALIGNMENT
        );
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());

        std::copy(stbdata, stbdata + safe_mul<size_t>(_width, _height, 4), _data->data());
    }

    packed_image::packed_image(const packed_image& cp_src)
        : _data(std::make_unique<data_t>(*cp_src._data))
        , _width(cp_src._width)
        , _height(cp_src._height)
    {
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());
    }

    packed_image::packed_image(packed_image&& mv_src) LIEN_RELEASE_NOEXCEPT
        : _data(std::move(mv_src._data))
        , _width(mv_src._width)
        , _height(mv_src._height)
    {
        mv_src._width = 0;
        mv_src._height = 0;
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());
    }

    uint8_t* packed_image::data() noexcept { return _data->data(); }

    const uint8_t* packed_image::cdata() const noexcept { return _data->cdata(); }

    void packed_image::set_pixel(int idx, const uint8_t* rgba)
    {
        std::memcpy(_data->data() + safe_mul<size_t>(idx, 4), rgba, 4);
    }
    
    void packed_image::set_pixel(int x, int y, const uint8_t* rgba)
    {
        std::memcpy(_data->data() + safe_mul<size_t>(x, y, _width, 4), rgba, 4);
    }

    size_t packed_image::pixel_count() const noexcept { return safe_mul<size_t>(_width, _height); }

    int packed_image::width() const noexcept { return _width; }
    
    int packed_image::height() const noexcept  { return _height; }

    bool packed_image::save_to_file_png(const std::string& path, int compression_level) const
    {
        stbi_write_png_compression_level = compression_level;
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());
        return stbi_write_png(path.c_str(), _width, _height, 4, _data->data(), _width * 4);
    }

    bool packed_image::save_to_file_jpeg(const std::string& path, int quality) const
    {
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());
        return stbi_write_jpg(path.c_str(), _width, _height, 4, _data->data(), quality);
    }

    bool packed_image::save_to_file_tga(const std::string& path) const
    {
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());
        return stbi_write_tga(path.c_str(), _width, _height, 4, _data->data());
    }

    void packed_image::resize_absolute(int w, int h)
    {
        std::unique_ptr<data_t> resized_data = std::make_unique<data_t>(
            safe_mul<uint8_t>(w, h, 4), 
            LIEN_DEFAULT_ALIGNMENT
        );
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, resized_data->data());
        debug_assert_ptr_aligned(LIEN_DEFAULT_ALIGNMENT, _data->data());

        stbir_resize_uint8(_data->cdata(), _width, _height, 4, resized_data->data(), w, h, 4, 4);
        _data = std::move(resized_data);
    }

    void packed_image::resize_relative(float w, float h)
    {
        int real_w = static_cast<int>(safe_mul<float>(_width, w));
        int real_h = static_cast<int>(safe_mul<float>(_height, h));

        resize_absolute(real_w, real_h);
    }

    ien::fixed_vector<uint8_t> packed_image::get_rgba_buff_copy()
    {
        return *_data;
    }

    packed_image& packed_image::operator=(const packed_image& cp_src)
    {
        *_data = *cp_src._data;
        _width = cp_src._width;
        _height = cp_src._height;

        return *this;
    }

    packed_image& packed_image::operator=(packed_image&& mv_src)
    {
        *_data = std::move(*mv_src._data);
        _width = mv_src._width;
        _height = mv_src._height;

        mv_src._width = 0;
        mv_src._height = 0;
        
        return *this;
    }
}