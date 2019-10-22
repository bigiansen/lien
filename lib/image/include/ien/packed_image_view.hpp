#pragma once

#include <ien/packed_image.hpp>
#include <ien/rect.hpp>

namespace ien::img
{
    class packed_image_view
    {
    private:
        const uint8_t* _ptr;
        rect<int> _view_rect;
        rect<int> _image_rect;
    
    public:
        packed_image_view(const packed_image*, const rect<int>& view_rect);

        std::array<uint8_t, 4> read_pixel(int index) const;
        std::array<uint8_t, 4> read_pixel(int x, int y) const;

        packed_image build_packed_image() const;
    };
}