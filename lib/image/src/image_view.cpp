#include <ien/image_view.hpp>

#include <ien/image.hpp>

namespace ien::img
{
    image_view::image_view(const image* img, const rect<int>& view_rect)
        : _ptr(img->cdata())
        , _view_rect(view_rect)
        , _image_rect(0, 0, img->width(), img->height())
    { }

    std::array<uint8_t, 4> image_view::read_pixel(int index) const
    {
        int sub_y = index / _view_rect.w;
        int sub_x = index % _view_rect.w;

        int real_y = _view_rect.y + sub_y;
        int real_x = _view_rect.x + sub_x;

        int real_index = (real_y * _image_rect.w) + real_x;
        return _ptr->read_pixel(real_index);
    }

    std::array<uint8_t, 4> image_view::read_pixel(int x, int y) const
    {
        int index = (y * _view_rect.w) + x;
        return read_pixel(index);
    }

    image image_view::build_image()
    {
        image result(_view_rect.w, _view_rect.h);

        int start_idx = (_view_rect.y * _image_rect.w) + _view_rect.x;
        for(int i = 0; i < _view_rect.h; ++i)
        {
            int line_idx = (start_idx) * (i * _image_rect.w);
            std::memcpy(
                result.data()->data_r() + (i * (_view_rect.w)), 
                _ptr->cdata_r() + line_idx, 
                _view_rect.w
            );
            std::memcpy(
                result.data()->data_g() + (i * (_view_rect.w)), 
                _ptr->cdata_g() + line_idx, 
                _view_rect.w
            );
            std::memcpy(
                result.data()->data_b() + (i * (_view_rect.w)), 
                _ptr->cdata_b() + line_idx, 
                _view_rect.w
            );
            std::memcpy(
                result.data()->data_a() + (i * (_view_rect.w)), 
                _ptr->cdata_a() + line_idx, 
                _view_rect.w
            );
        }

        return result;
    }
}