#pragma once

#include <ien/image_unpacked_data.hpp>
#include <ien/rect.hpp>

namespace ien::img
{
    class image_rect_view
    {
    private:
        const image_unpacked_data* _ptr;
        rect<int> _view_rect;
        
    public:
        image_rect_view(const image_unpacked_data*, const rect<int>& view_rect);
    };
}