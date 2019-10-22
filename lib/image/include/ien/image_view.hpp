#pragma once

#include <ien/image_unpacked_data.hpp>
#include <ien/rect.hpp>
#include <array>
#include <cinttypes>
#include <vector>

namespace ien::img
{
    class image_view
    {
    private:
        const image_unpacked_data* _ptr;
        rect<int> _view_rect;
        rect<int> _image_rect;
        
    public:
        image_view(const image*, const rect<int>& view_rect);

        std::array<uint8_t, 4> read_pixel(int index) const;
        std::array<uint8_t, 4> read_pixel(int x, int y) const;

        image build_image();
    };
}