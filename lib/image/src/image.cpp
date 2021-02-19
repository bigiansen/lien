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

    std::unique_ptr<ien::image> read_image(const std::string& imgpath, image_type type)
    {
        if (type == image_type::INTERLEAVED)        
            return std::make_unique<ien::interleaved_image>(imgpath);
        else        
            return std::make_unique<ien::planar_image>(imgpath);
    }
}