#include <ien/interleaved_image.hpp>

#include <ien/arithmetic.hpp>
#include <ien/planar_image.hpp>
#include <ien/platform.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <cstring>
#include <stdexcept>

namespace ien
{
    interleaved_image::interleaved_image(size_t width, size_t height)
        : image(width, height, image_type::INTERLEAVED)
    { 
        _data = std::make_unique<ien::fixed_vector<uint8_t>>(
            safe_mul<size_t>(_width, _height , 4),
            LIEN_DEFAULT_ALIGNMENT
        );
    }

    interleaved_image::interleaved_image(const std::string& path)
        : image(image_type::INTERLEAVED)
    {
        int ch_dummy;
        int w, h;
        uint8_t* stbdata = stbi_load(path.c_str(), &w, &h, &ch_dummy, 4);
        _width = w;
        _height = h;
        if(stbdata == nullptr)
        {
            throw std::invalid_argument("Invalid image path or file format");
        }

        _data = std::make_unique<ien::fixed_vector<uint8_t>>(
            safe_mul<size_t>(_width, _height , 4),
            LIEN_DEFAULT_ALIGNMENT
        );

        std::copy(stbdata, stbdata + safe_mul<size_t>(_width, _height, 4), _data->data());
    }

    interleaved_image::interleaved_image(const interleaved_image& cp_src)
        : _data(std::make_unique<ien::fixed_vector<uint8_t>>(*cp_src._data))
        , image(cp_src._width, cp_src._height, image_type::INTERLEAVED)
    { }

    interleaved_image::interleaved_image(interleaved_image&& mv_src) noexcept
        : _data(std::move(mv_src._data))
        , image(mv_src._width, mv_src._height, image_type::INTERLEAVED)
    {
        mv_src._width = 0;
        mv_src._height = 0;
    }

    uint8_t* interleaved_image::data() noexcept { return _data->data(); }

    const uint8_t* interleaved_image::cdata() const noexcept { return _data->cdata(); }

    void interleaved_image::set_pixel(size_t idx, uint32_t rgba)
    {
        *reinterpret_cast<uint32_t*>(_data->data() + (idx * 4)) = rgba;
    }
    
    void interleaved_image::set_pixel(size_t x, size_t y, uint32_t rgba)
    {
        *reinterpret_cast<uint32_t*>(_data->data() + (x * y * _width * 4)) = rgba;
    }

    uint32_t interleaved_image::get_pixel(size_t index) const 
    {
        return *reinterpret_cast<const uint32_t*>((*_data).cdata() + (index * 4));
    }

    uint32_t interleaved_image::get_pixel(size_t x, size_t y) const
    {
        const size_t index = (y * _width) + x;
        return *reinterpret_cast<const uint32_t*>((*_data).cdata() + (index * 4));
    }

    std::vector<uint32_t> interleaved_image::get_chunk(const rect<size_t>& r) const
    {
        std::vector<uint32_t> result;
        result.resize(r.w * r.h);

        for (size_t y = r.y; y < (r.y + r.h); ++y)
        {
            size_t offset = (y * _width) + r.x;            
            std::memcpy(
                result.data() + (y * r.w * 4), 
                _data.get()->cdata() + (offset * 4), 
                (r.w * 4)
            );
        }

        return result;
    }

    bool interleaved_image::save_to_file_png(const std::string& path, int compression_level) const
    {
        stbi_write_png_compression_level = compression_level;
        return stbi_write_png(
            path.c_str(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            _data->data(), 
            static_cast<int>(_width * 4)
        );
    }

    bool interleaved_image::save_to_file_jpeg(const std::string& path, int quality) const
    {
        return stbi_write_jpg(
            path.c_str(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            _data->data(), 
            quality
        );
    }

    bool interleaved_image::save_to_file_tga(const std::string& path) const
    {
        return stbi_write_tga(
            path.c_str(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            _data->data()
        );
    }

    static void save_to_memory_func(void* ctx, void* data, int size)
    {
        printf("STBI REPORTED LEN: %d\n", size);
        auto* vec = reinterpret_cast<ien::fixed_vector<uint8_t>*>(ctx);
        
        *vec = ien::fixed_vector<uint8_t>(size);
        std::memcpy(vec->data(), data, size);
    }

    ien::fixed_vector<uint8_t> interleaved_image::save_to_memory_png(int compression_level) const
    {
        ien::fixed_vector<uint8_t> result;
        bool ok = stbi_write_png_to_func(
            save_to_memory_func,
            reinterpret_cast<void*>(&result),
            static_cast<int>(_width),
            static_cast<int>(_height),
            4,
            _data->cdata(),
            static_cast<int>(_width * 4)
        );
        
        if(!ok) { throw std::runtime_error("Failed to write png data to memory"); }

        return result;
    }

    ien::fixed_vector<uint8_t> interleaved_image::save_to_memory_jpeg(int quality) const
    {
        ien::fixed_vector<uint8_t> result;        
        bool ok = stbi_write_jpg_to_func(
            save_to_memory_func,
            reinterpret_cast<void*>(&result), 
            static_cast<int>(_width), 
            static_cast<int>(_height),
            4,
            _data->data(),
            quality
        );

        if(!ok) { throw std::runtime_error("Failed to write jpeg data to memory"); }

        return result;
    }

    ien::fixed_vector<uint8_t> interleaved_image::save_to_memory_tga() const
    {
        ien::fixed_vector<uint8_t> result;
        bool ok = stbi_write_tga_to_func(
            save_to_memory_func,
            reinterpret_cast<void*>(&result), 
            static_cast<int>(_width), 
            static_cast<int>(_height),
            4,
            _data->data()
        );

        if(!ok) { throw std::runtime_error("Failed to write tga data to memory"); }

        return result;
    }

    void interleaved_image::resize_absolute(size_t w, size_t h)
    {
        std::unique_ptr<ien::fixed_vector<uint8_t>> resized_data = std::make_unique<ien::fixed_vector<uint8_t>>(
            safe_mul<size_t>(w, h, 4), 
            _data->alignment()
        );

        stbir_resize_uint8(
            _data->cdata(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            resized_data->data(), 
            static_cast<int>(w), 
            static_cast<int>(h), 
            4, 
            4
        );
        _data = std::move(resized_data);
    }

    void interleaved_image::resize_relative(float w, float h)
    {
        size_t real_w = static_cast<size_t>(safe_mul<float>(_width, w));
        size_t real_h = static_cast<size_t>(safe_mul<float>(_height, h));

        resize_absolute(real_w, real_h);
    }

    ien::fixed_vector<uint8_t> interleaved_image::get_rgba_buff_copy()
    {
        return *_data;
    }

    ien::planar_image interleaved_image::to_planar_image()
    {
        return ien::planar_image(this->_data->data(), _width, _height);
    }

    interleaved_image& interleaved_image::operator=(const interleaved_image& cp_src)
    {
        _data = std::make_unique<ien::fixed_vector<uint8_t>>(
            cp_src.pixel_count() * 4,
            LIEN_DEFAULT_ALIGNMENT
        );
        std::memcpy(_data->data(), cp_src.cdata(), cp_src.pixel_count() * 4);
        _width = cp_src._width;
        _height = cp_src._height;

        return *this;
    }

    interleaved_image& interleaved_image::operator=(interleaved_image&& mv_src) noexcept
    {
        _data = std::move(mv_src._data);
        _width = mv_src._width;
        _height = mv_src._height;

        mv_src._width = 0;
        mv_src._height = 0;
        
        return *this;
    }
}