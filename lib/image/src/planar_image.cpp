#include <ien/planar_image.hpp>

#include <ien/arithmetic.hpp>
#include <ien/assert.hpp>
#include <ien/base64.hpp>
#include <ien/platform.hpp>
#include <ien/image_ops.hpp>
#include <ien/interleaved_image.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <cstring>
#include <stdexcept>

namespace ien
{
    planar_image::planar_image(size_t width, size_t height)
        : image(width, height, image_type::PLANAR)
        , _data(safe_mul<size_t>(width, height))
    { }

    planar_image::planar_image(const std::string& path)
        : image(image_type::PLANAR)
    {
        int channels_dummy = 0;
        int w, h;
        uint8_t* packed_data = stbi_load(
            path.c_str(),
            &w,
            &h,
            &channels_dummy,
            4
        );
        _width = static_cast<size_t>(w);
        _height = static_cast<size_t>(h);

        if(packed_data == nullptr)
        {
            throw std::invalid_argument("Unable to load image with path: " + path);
        }
        _data = image_ops::unpack_image_data(packed_data, safe_mul<size_t>(_width, _height, 4));

        stbi_image_free(packed_data);
    }

    planar_image::planar_image(const uint8_t* rgba_buff, size_t w, size_t h)
        : image(w, h, image_type::PLANAR)
        , _data(image_ops::unpack_image_data(rgba_buff, safe_mul<size_t>(w, h, 4)))
    { }

    image_planar_data* planar_image::data() noexcept { return &_data; }

    const image_planar_data* planar_image::cdata() const noexcept { return &_data; }

    // Expensive, consider using packed_image instead
    uint32_t planar_image::get_pixel(size_t index) const
    {        
        return construct4<uint32_t>(
            _data.cdata_r()[index], 
            _data.cdata_g()[index], 
            _data.cdata_b()[index], 
            _data.cdata_a()[index]
        );
    }

    // Expensive, consider using packed_image instead
    uint32_t planar_image::get_pixel(size_t x, size_t y) const
    {        
        size_t index = (y * _width) + x;        
        return construct4<uint32_t>(
            _data.cdata_r()[index], 
            _data.cdata_g()[index], 
            _data.cdata_b()[index], 
            _data.cdata_a()[index]
        );
    }

    void planar_image::set_pixel(size_t index, uint32_t rgba)
    {
        LIEN_DEBUG_ASSERT_MSG(index < (_width * _height), "Pixel index out of range!");
        _data.set_pixel(index, rgba);
    }

    void planar_image::set_pixel(size_t x, size_t y, uint32_t rgba)
    {
        set_pixel((x * y), rgba);
    }

    std::vector<uint32_t> planar_image::get_chunk(const rect<size_t>& r) const
    {
        const size_t chunk_size = r.w * r.h;
        std::vector<uint32_t> result;
        result.reserve(chunk_size);

        for (size_t y = r.y; y < (r.y + r.h); ++y)
        {
            for (size_t x = r.x; x < (r.x + r.w); ++x)
            {
                result.push_back(get_pixel(x, y));
            }
        }

        return result;
    }

    bool planar_image::save_to_file_png(const std::string& path, int compression_level) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();

        stbi_write_png_compression_level = compression_level;
        return stbi_write_png(
            path.c_str(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            packed_data.data(), 
            static_cast<int>(_width * 4)
        );
    }

    bool planar_image::save_to_file_jpeg(const std::string& path, int quality) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        return stbi_write_jpg(
            path.c_str(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            packed_data.data(), 
            quality
        );
    }

    bool planar_image::save_to_file_tga(const std::string& path) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        return stbi_write_tga(
            path.c_str(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            packed_data.data()
        );
    }

    static void save_to_memory_func(void* ctx, void* data, int size)
    {
        printf("STBI REPORTED LEN: %d\n", size);
        auto* vec = reinterpret_cast<ien::fixed_vector<uint8_t>*>(ctx);
        
        *vec = ien::fixed_vector<uint8_t>(size);
        std::memcpy(vec->data(), data, size);
    }

    ien::fixed_vector<uint8_t> planar_image::save_to_memory_png(int compression_level) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();

        ien::fixed_vector<uint8_t> result;

        stbi_write_png_compression_level = compression_level;
        bool ok = stbi_write_png_to_func(
            save_to_memory_func, 
            reinterpret_cast<void*>(&result), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            packed_data.data(), 
            static_cast<int>(_width * 4)
        );

        if(!ok) { throw std::runtime_error("Failed to write png data to memory"); }

        return result;
    }

    ien::fixed_vector<uint8_t> planar_image::save_to_memory_jpeg(int quality) const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();

        ien::fixed_vector<uint8_t> result;
        stbi_write_jpg_to_func(
            save_to_memory_func,
            reinterpret_cast<void*>(&result), 
            static_cast<int>(_width), 
            static_cast<int>(_height),
            4,
            packed_data.data(),
            quality
        );
        return result;
    }

    ien::fixed_vector<uint8_t> planar_image::save_to_memory_tga() const
    {
        ien::fixed_vector<uint8_t> packed_data = _data.pack_data();

        ien::fixed_vector<uint8_t> result;
        stbi_write_tga_to_func(
            save_to_memory_func,
            reinterpret_cast<void*>(&result), 
            static_cast<int>(_width), 
            static_cast<int>(_height),
            4,
            packed_data.data()
        );
        return result;
    }

    void planar_image::resize_absolute(size_t w, size_t h)
    {
        const ien::fixed_vector<uint8_t> packed_data = _data.pack_data();
        _data.resize(safe_mul<size_t>(w, h)); // realloc unpacked data buffers
        
        std::vector<uint8_t> resized_packed_data;
        resized_packed_data.resize(safe_mul<size_t>(w, h, 4));

        stbir_resize_uint8(
            packed_data.cdata(), 
            static_cast<int>(_width), 
            static_cast<int>(_height), 
            4, 
            resized_packed_data.data(),
            static_cast<int>(w), 
            static_cast<int>(h), 
            4, 
            4
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

    void planar_image::resize_relative(float w, float h)
    {
        int real_w = static_cast<int>(safe_mul<float>(_width, w));
        int real_h = static_cast<int>(safe_mul<float>(_height, h));

        resize_absolute(real_w, real_h);
    }

    interleaved_image planar_image::to_interleaved_image()
    {
        auto packed_data = _data.pack_data();
        interleaved_image result(_width, _height);
        std::copy(packed_data.begin(), packed_data.end(), result.data());
        return result;
    }

    std::string planar_image::to_png_base64(int comp_level)
    {
        ien::fixed_vector<uint8_t> data = save_to_memory_png(comp_level);
        return ien::base64::encode(data.cdata(), data.size());
    }

    planar_image& planar_image::operator=(const planar_image& cp_src)
    {
        size_t len = cp_src.pixel_count();

        // Needs realloc ?
        if(_data._size == 0)
            _data = image_planar_data(len);
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

    planar_image& planar_image::operator=(planar_image&& mv_src) noexcept
    {
        _data = std::move(mv_src._data);
        _width = mv_src._width;
        _height = mv_src._height;

        mv_src._width = 0;
        mv_src._height = 0;

        return *this;
    }
}
