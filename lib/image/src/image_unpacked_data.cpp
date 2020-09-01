#include <ien/image_unpacked_data.hpp>

#include <ien/assert.hpp>
#include <ien/platform.hpp>

#include <array>
#include <cstring>

namespace ien
{
    image_unpacked_data::image_unpacked_data(size_t pixel_count)
        : _r(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count, LIEN_DEFAULT_ALIGNMENT)))
        , _g(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count, LIEN_DEFAULT_ALIGNMENT)))
        , _b(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count, LIEN_DEFAULT_ALIGNMENT)))
        , _a(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count, LIEN_DEFAULT_ALIGNMENT)))
        , _size(pixel_count)
    { }

    image_unpacked_data::~image_unpacked_data()
    {
        if(!_moved)
        {
            LIEN_ALIGNED_FREE(_r);
            LIEN_ALIGNED_FREE(_g);
            LIEN_ALIGNED_FREE(_b);
            LIEN_ALIGNED_FREE(_a);
        }
    }

    image_unpacked_data::image_unpacked_data(const image_unpacked_data& cp_src)
        : _r(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(cp_src._size, LIEN_DEFAULT_ALIGNMENT)))
        , _g(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(cp_src._size, LIEN_DEFAULT_ALIGNMENT)))
        , _b(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(cp_src._size, LIEN_DEFAULT_ALIGNMENT)))
        , _a(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(cp_src._size, LIEN_DEFAULT_ALIGNMENT)))
        , _size(cp_src._size)
        , _moved(cp_src._moved)
    {
        std::memcpy(_r, cp_src._r, cp_src._size);
        std::memcpy(_g, cp_src._g, cp_src._size);
        std::memcpy(_b, cp_src._b, cp_src._size);
        std::memcpy(_a, cp_src._a, cp_src._size);
    }

	image_unpacked_data::image_unpacked_data(image_unpacked_data&& mv_src) noexcept
        : _r(mv_src._r)
        , _g(mv_src._g)
        , _b(mv_src._b)
        , _a(mv_src._a)
        , _size(mv_src._size)
        , _moved(false)
    {
        mv_src._moved = true;
		mv_src._r = nullptr;
		mv_src._g = nullptr;
		mv_src._b = nullptr;
		mv_src._a = nullptr;
		mv_src._size = 0;
    }

    void image_unpacked_data::operator=(image_unpacked_data&& mv_src)
    {
        _r = mv_src._r;
        _g = mv_src._g;
        _b = mv_src._b;
        _a = mv_src._a;
        _size = mv_src._size;
        _moved = mv_src._moved;
        mv_src._moved = true;
    }

    uint8_t* image_unpacked_data::data_r() noexcept { return _r; }
    uint8_t* image_unpacked_data::data_g() noexcept { return _g; }
    uint8_t* image_unpacked_data::data_b() noexcept { return _b; }
    uint8_t* image_unpacked_data::data_a() noexcept { return _a; }

    const uint8_t* image_unpacked_data::cdata_r() const noexcept { return _r; }
    const uint8_t* image_unpacked_data::cdata_g() const noexcept { return _g; }
    const uint8_t* image_unpacked_data::cdata_b() const noexcept { return _b; }
    const uint8_t* image_unpacked_data::cdata_a() const noexcept { return _a; }

    size_t image_unpacked_data::size() const noexcept { return _size; }

    void image_unpacked_data::resize(size_t pixel_count)
    {
        _r = reinterpret_cast<uint8_t*>(LIEN_ALIGNED_REALLOC(_r, pixel_count, LIEN_DEFAULT_ALIGNMENT));
        _g = reinterpret_cast<uint8_t*>(LIEN_ALIGNED_REALLOC(_g, pixel_count, LIEN_DEFAULT_ALIGNMENT));
        _b = reinterpret_cast<uint8_t*>(LIEN_ALIGNED_REALLOC(_b, pixel_count, LIEN_DEFAULT_ALIGNMENT));
        _a = reinterpret_cast<uint8_t*>(LIEN_ALIGNED_REALLOC(_a, pixel_count, LIEN_DEFAULT_ALIGNMENT));
    }

    uint32_t image_unpacked_data::get_pixel(size_t index) const
    {
        return static_cast<uint32_t>(_r[index]) >> 24
            | static_cast<uint32_t>(_g[index]) >> 16
            | static_cast<uint32_t>(_b[index]) >> 8
            | static_cast<uint32_t>(_a[index]);
    }

    void image_unpacked_data::set_pixel(size_t index, uint32_t rgba)
    {
        _r[index] = static_cast<uint8_t>(rgba >> 24);
        _g[index] = static_cast<uint8_t>(rgba >> 16);
        _b[index] = static_cast<uint8_t>(rgba >> 8);
        _a[index] = static_cast<uint8_t>(rgba);
    }

    ien::fixed_vector<uint8_t> image_unpacked_data::pack_data() const
    {
        ien::fixed_vector<uint8_t> result(this->size() * 4, LIEN_DEFAULT_ALIGNMENT);

        for(size_t i = 0; i < _size; ++i)
        {
            result[(i * 4) + 0] = _r[i];
            result[(i * 4) + 1] = _g[i];
            result[(i * 4) + 2] = _b[i];
            result[(i * 4) + 3] = _a[i];
        }
        return result;
    }
}
