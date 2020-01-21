#include <ien/image.hpp>

#include <ien/assert.hpp>
#include <ien/platform.hpp>
#include <ien/image_ops.hpp>
#include <ien/packed_image.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <cstring>
#include <stdexcept>

namespace ien
{
    image::image(int width, int height)
        : _data(static_cast<size_t>(width) * height)
        , _width(width)
        , _height(height)
    { }

    image::image(const std::string& path)
        : _width(0), _height(0)
    {
        int channels_dummy = 0;
        uint8_t* packed_data = stbi_load(
            path.c_str(),
            &_width,
            &_height,
            &channels_dummy,
            4
        );

        if(packed_data == nullptr)
        {
            throw std::invalid_argument("Unable to load image with path: " + path);
        }
        _data = image_ops::unpack_image_data(packed_data, (static_cast<size_t>(_width) * _height * 4));
        stbi_image_free(packed_data);
    }

    image::image(const image& cp_src)
        : _data(cp_src.pixel_count())
        , _width(cp_src._width)
        , _height(cp_src._height)
    {
        std::memcpy(_data.data_r(), cp_src.cdata()->cdata_r(), cp_src.pixel_count());
        std::memcpy(_data.data_g(), cp_src.cdata()->cdata_g(), cp_src.pixel_count());
        std::memcpy(_data.data_b(), cp_src.cdata()->cdata_b(), cp_src.pixel_count());
        std::memcpy(_data.data_a(), cp_src.cdata()->cdata_a(), cp_src.pixel_count());
    }

    image_unpacked_data* image::data() noexcept { return &_data; }

    const image_unpacked_data* image::cdata() const noexcept { return &_data; }

    size_t image::pixel_count() const noexcept
    {
        return static_cast<size_t>(_width) * _height;
    }

    int image::width() const noexcept { return _width; }
    
    int image::height() const noexcept { return _height; }

    std::array<uint8_t, 4> image::get_packed_pixel(int index) const
    {
        return { 
            _data.cdata_r()[index], 
            _data.cdata_g()[index], 
            _data.cdata_b()[index],
            _data.cdata_a()[index] 
        };
    }

    std::array<uint8_t, 4> image::get_packed_pixel(int x, int y) const
    {
        return {
            _data.cdata_r()[(y * _height) + x],
            _data.cdata_g()[(y * _height) + x],
            _data.cdata_b()[(y * _height) + x],
            _data.cdata_a()[(y * _height) + x],
        };
    }

    void image::set_pixel(int index, const uint8_t* rgba)
    {
        debug_assert(index < (_width * _height), "Pixel index out of range!");
        _data.set_pixel(index, rgba);
    }

    void image::set_pixel(int x, int y, const uint8_t* rgba)
    {
        set_pixel((x * y), rgba);
    }

    bool image::save_to_file_png(const std::string& path, int compression_level) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        stbi_write_png_compression_level = compression_level;
        return stbi_write_png(path.c_str(), _width, _height, 4, packed_data.data(), _width * 4);
    }

    bool image::save_to_file_jpeg(const std::string& path, int quality) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        return stbi_write_jpg(path.c_str(), _width, _height, 4, packed_data.data(), quality);
    }

    bool image::save_to_file_tga(const std::string& path) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        return stbi_write_tga(path.c_str(), _width, _height, 4, packed_data.data());
    }

    void image::resize_absolute(int w, int h)
    {
        const ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        _data.resize(static_cast<size_t>(w) * h); // realloc unpacked data buffers
        
        std::vector<uint8_t> resized_packed_data;
        resized_packed_data.resize(size_t(w) * h * 4);

        stbir_resize_uint8(
            packed_data.cdata(), _width, _height, 4, resized_packed_data.data(), w, h, 4, 4
        );
        
        uint8_t* r = _data.data_r();
        uint8_t* g = _data.data_g();
        uint8_t* b = _data.data_b();
        uint8_t* a = _data.data_a();

        for(size_t i = 0; i < (size_t(w) * h); ++i)
        {
            r[i] = resized_packed_data[(i * 4) + 0];
            g[i] = resized_packed_data[(i * 4) + 1];
            b[i] = resized_packed_data[(i * 4) + 2];
            a[i] = resized_packed_data[(i * 4) + 3];
        }
    }

    void image::resize_relative(float w, float h)
    {
        int real_w = static_cast<int>(static_cast<float>(_width) * w);
        int real_h = static_cast<int>(static_cast<float>(_height) * h);

        resize_absolute(real_w, real_h);
    }

    packed_image image::to_packed_image()
    {
        auto packed_data = _data.pack_data();
        packed_image result(_width, _height);
        std::copy(packed_data.begin(), packed_data.end(), result.data());
        return result;
    }
}