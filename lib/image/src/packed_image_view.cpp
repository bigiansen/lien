#include <ien/packed_image_view.hpp>

#include <cstring>

namespace ien::img
{
    packed_image_view::packed_image_view(const packed_image* img, const rect<int>& view_rect)
        : _ptr(img->cdata())
        , _view_rect(view_rect)
        , _image_rect(0, 0, img->width(), img->height())
    { }

    std::array<uint8_t, 4> packed_image_view::read_pixel(int index) const
    {
        int sub_y = index / _view_rect.w;
        int sub_x = index % _view_rect.w;

        int real_y = _view_rect.y + sub_y;
        int real_x = _view_rect.x + sub_x;

        int real_index = (real_y * _image_rect.w) + real_x;
        return { 
            _ptr[real_index + 0],
            _ptr[real_index + 1],
            _ptr[real_index + 2],
            _ptr[real_index + 3] 
        };
    }

    std::array<uint8_t, 4> packed_image_view::read_pixel(int x, int y) const
    {
        int index = (y * _view_rect.w) + x;
        return read_pixel(index);
    }

    packed_image packed_image_view::build_packed_image() const
    {
        packed_image result(_view_rect.w, _view_rect.h);
        
        for(int i = 0; i < _view_rect.h; ++i)
        {
            int line_idx = ((_image_rect.w * 4) * (_view_rect.y + i)) + (_view_rect.x * 4);

			ptrdiff_t dst_offset = static_cast<ptrdiff_t>(i) * 4 * _view_rect.w;
			size_t copylen = static_cast<size_t>(_view_rect.w) * 4;

            std::memcpy(result.data() + dst_offset, _ptr + (line_idx), copylen);
        }

        return result;
    }
}