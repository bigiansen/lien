#include <ien/image_unpacked_data.hpp>

#include <ien/assert.hpp>
#include <ien/platform.hpp>

namespace ien::img
{
    image_unpacked_data::image_unpacked_data(size_t pixel_count)
        : _r(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _g(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _b(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _a(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _size(pixel_count)
    { }

    image_unpacked_data::~image_unpacked_data()
    {
        LIEN_ALIGNED_FREE(_r);
        LIEN_ALIGNED_FREE(_g);
        LIEN_ALIGNED_FREE(_b);
        LIEN_ALIGNED_FREE(_a);
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
        LIEN_ALIGNED_REALLOC(_r, pixel_count);
        LIEN_ALIGNED_REALLOC(_g, pixel_count);
        LIEN_ALIGNED_REALLOC(_b, pixel_count);
        LIEN_ALIGNED_REALLOC(_a, pixel_count);
    }

    std::vector<uint8_t> image_unpacked_data::pack_data()
    {
        std::vector<uint8_t> result;
        result.resize(_size * 4);
        for(size_t i = 0; i < _size; ++i)
        {
            result[(i * 4) + 0] = _r[i];
            result[(i * 4) + 1] = _g[i];
            result[(i * 4) + 2] = _b[i];
            result[(i * 4) + 3] = _a[i];
        }
        return result;
    }

    image_unpacked_data unpack_image_data(uint8_t* data, size_t len)
    {
        runtime_assert(len % 4 == 0, "Packed image data len must be a multiple of 4!");
        image_unpacked_data result(len / 4);

        uint8_t* r = result.data_r();
        uint8_t* g = result.data_g();
        uint8_t* b = result.data_b();
        uint8_t* a = result.data_a();

        for(size_t i = 0; i < len; i += 4)
        {
            r[i / 4] = data[i + 0];
            g[i / 4] = data[i + 1];
            b[i / 4] = data[i + 2];
            a[i / 4] = data[i + 3];
        }
        return result;
    }
}