#include <ien/image.hpp>

#include <ien/interleaved_image.hpp>
#include <ien/planar_image.hpp>

namespace ien
{
    size_t image::pixel_count() const noexcept
    {
        return _width * _height;
    }

    size_t image::size() const noexcept
    {
        return _width * _height * 4;
    }

    std::vector<uint32_t> image::get_chunk(const rect<size_t>& r) const
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

    std::unique_ptr<ien::image> read_image(const std::string& imgpath, image_type type)
    {
        if (type == image_type::INTERLEAVED)        
            return std::make_unique<ien::interleaved_image>(imgpath);
        else        
            return std::make_unique<ien::planar_image>(imgpath);
    }
}