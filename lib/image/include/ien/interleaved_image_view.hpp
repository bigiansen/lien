#pragma once

#include <ien/interleaved_image.hpp>
#include <ien/rect.hpp>

namespace ien
{
    class interleaved_image_view
    {
    private:
        const uint8_t* _ptr;
        rect<size_t> _view_rect;
        rect<size_t> _image_rect;
    
    public:
        interleaved_image_view(const interleaved_image*, const rect<size_t>& view_rect);

        uint32_t read_pixel(size_t index) const;
        uint32_t read_pixel(size_t x, size_t y) const;

        interleaved_image build_interleaved_image() const;
    };
}