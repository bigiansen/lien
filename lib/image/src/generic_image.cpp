#include <ien/generic_image.hpp>

namespace ien
{
    size_t generic_image::pixel_count() const noexcept
    {
        return _width * _height;
    }

    size_t generic_image::size() const noexcept
    {
        return _width * _height * 4;
    }

    std::vector<uint32_t> generic_image::get_chunk(const rect<size_t>& r) const
    {
        const size_t chunk_size = r.w * r.h;
        std::vector<uint32_t> result;
        result.reserve(chunk_size);

        for(size_t y = r.y; y < (r.y + r.h); ++y)
        {
            for(size_t x = r.x; x < (r.x + r.w); ++x)
            {
                result.push_back(get_pixel(x, y));
            }
        }

        return result;
    }
}