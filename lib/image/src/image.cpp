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

    void truncate_channel_bits_sse2(uint8_t* data, long size, int r, int g, int b, int a);
    void truncate_channel_bits_std(uint8_t* data, long size, int r, int g, int b, int a);

    void image::truncate_channel_bits(int r, int g, int b, int a)
    {
    #if defined(LIEN_ARCH_X86_64)
        static void (*fptr)(uint8_t*, long, int, int, int, int) = 
            &truncate_channel_bits_sse2;
    #elif defined(LIEN_ARCH_X86)
        static void (*fptr)(uint8_t*, long, int, int, int, int) = 
            ien::platform::x86::get_feature(ien::platform::x86::feature::SSE2)
                ? &truncate_channel_bits_sse2
                : &truncate_channel_bits_std;
    #else
        static void (*fptr)(uint8_t*, long, int, int, int, int) = 
            &truncate_channel_bits_std;
    #endif
        if(r || g || b || a)
        {
            fptr(_data_ptr, (_width * _height * _channels), r, g, b, a);
        }
    }

    void truncate_channel_bits_std(uint8_t* data, long size, int r, int g, int b, int a)
    {
        if(size % 4 != 0)
        {
            throw std::invalid_argument("Specified size must be a multiple of 4");
        }

        uint32_t trunc_mask = 0;
        trunc_mask |= (1 << r) << 24;
        trunc_mask |= (1 << g) << 16;
        trunc_mask |= (1 << b) << 8;
        trunc_mask |= (1 << a);

        for(long i = 0; i < size; i += 4)
        {
            uint32_t* iptr = reinterpret_cast<uint32_t*>(data);
            *iptr &= trunc_mask;
        }
    }

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)

#include <immintrin.h>

    void truncate_channel_bits_sse2(uint8_t* data, long size, int r, int g, int b, int a)
    {
        if(size % 4 != 0)
        {
            throw std::invalid_argument("Specified size must be a multiple of 4");
        }

        uint8_t trunc_mask[16] = {
            (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
            (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
            (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
            (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
        };

        __m128i vtrunc_mask = _mm_loadu_si32(trunc_mask);
        for(long i = 0; i < size; i += 16)
        {
            __m128i vdata = _mm_loadu_si32(data + i);
            vdata = _mm_and_si128(vdata, vtrunc_mask);
            _mm_store_si128(reinterpret_cast<__m128i*>(data + i), vdata);
        }
    }

#endif
}