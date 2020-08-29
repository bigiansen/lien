#include <ien/image.hpp>

#include <ien/arithmetic.hpp>
#include <ien/assert.hpp>
#include <ien/base64.hpp>
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
        : _data(safe_mul<size_t>(width, height))
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
        _data = image_ops::unpack_image_data(packed_data, safe_mul<size_t>(_width, _height, 4));

        stbi_image_free(packed_data);
    }

    image::image(const uint8_t* rgba_buff, int w, int h)
        : _data(image_ops::unpack_image_data(rgba_buff, safe_mul<size_t>(w, h, 4)))
        , _width(w)
        , _height(h)
    { }

    image_unpacked_data* image::data() noexcept { return &_data; }

    const image_unpacked_data* image::cdata() const noexcept { return &_data; }

    size_t image::pixel_count() const noexcept
    {
        return safe_mul<size_t>(_width, _height);
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
        LIEN_DEBUG_ASSERT_MSG(index < (_width * _height), "Pixel index out of range!");
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

    void save_to_memory_func(void* ctx, void* data, int size)
    {
        printf("STBI REPORTED LEN: %d\n", size);
        auto* vec = reinterpret_cast<ien::fixed_vector<uint8_t>*>(ctx);
        
        *vec = ien::fixed_vector<uint8_t>(size);
        std::memcpy(vec->data(), data, size);
    }

    ien::fixed_vector<uint8_t> image::save_to_memory_png(int compression_level) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();

        ien::fixed_vector<uint8_t> result;

        stbi_write_png_compression_level = compression_level;
        bool ok = stbi_write_png_to_func(
            save_to_memory_func, 
            reinterpret_cast<void*>(&result), 
            _width, 
            _height, 
            4, 
            packed_data.data(), 
            _width * 4
        );

        if(!ok)
            throw std::runtime_error("Failed to write png data to memory");

        return result;
    }

    ien::fixed_vector<uint8_t> image::save_to_memory_jpeg(int quality) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();

        ien::fixed_vector<uint8_t> result;
        stbi_write_jpg_to_func(
            save_to_memory_func,
            reinterpret_cast<void*>(&result), 
            _width, 
            _height,
            4,
            packed_data.data(),
            quality
        );
        return result;
    }

    ien::fixed_vector<uint8_t> image::save_to_memory_tga() const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();

        ien::fixed_vector<uint8_t> result;
        stbi_write_tga_to_func(
            save_to_memory_func,
            reinterpret_cast<void*>(&result), 
            _width, 
            _height,
            4,
            packed_data.data()
        );
        return result;
    }

    void image::resize_absolute(int w, int h)
    {
        const ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        _data.resize(safe_mul<size_t>(w, h)); // realloc unpacked data buffers
        
        std::vector<uint8_t> resized_packed_data;
        resized_packed_data.resize(safe_mul<size_t>(w, h, 4));

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
        int real_w = static_cast<int>(safe_mul<float>(_width, w));
        int real_h = static_cast<int>(safe_mul<float>(_height, h));

        resize_absolute(real_w, real_h);
    }

    packed_image image::to_packed_image()
    {
        auto packed_data = _data.pack_data();
        packed_image result(_width, _height);
        std::copy(packed_data.begin(), packed_data.end(), result.data());
        return result;
    }

    std::string image::to_png_base64(int comp_level)
    {
        ien::fixed_vector<uint8_t> data = save_to_memory_png(comp_level);
        return ien::base64::encode(data.cdata(), data.size());
    }

    image& image::operator=(const image& cp_src)
    {
        size_t len = cp_src.pixel_count();

        // Needs realloc ?
        if(_data._size == 0)
            _data = image_unpacked_data(len);
        else if(_data._size != cp_src._data._size)
            _data.resize(len);

        std::memcpy(_data.data_r(), cp_src._data.cdata_r(), len);
        std::memcpy(_data.data_g(), cp_src._data.cdata_g(), len);
        std::memcpy(_data.data_b(), cp_src._data.cdata_b(), len);
        std::memcpy(_data.data_a(), cp_src._data.cdata_b(), len);

        this->_width = cp_src._width;
        this->_height = cp_src._height;

        return *this;
    }

    image& image::operator=(image&& mv_src) noexcept
    {
        _data = std::move(mv_src._data);
        _width = mv_src._width;
        _height = mv_src._height;

        mv_src._width = 0;
        mv_src._height = 0;

        return *this;
    }
}
