#pragma once

#include <ien/image_unpacked_data.hpp>
#include <ien/rect.hpp>
#include <array>
#include <cinttypes>
#include <vector>

namespace ien::img
{
    namespace _internal
    {
        template<bool Mutable>
        class image_view_base
        {
        protected:
            using dataptr_t = std::conditional_t<Mutable, image_unpacked_data*, const image_unpacked_data*>;
            using imgptr_t = std::conditional_t<Mutable, image*, const image*>;

            dataptr_t _ptr;
            rect<int> _view_rect;
            rect<int> _image_rect;

            constexpr int cvt2_real_idx(int index)
            {
                int sub_y = index / _view_rect.w;
                int sub_x = index % _view_rect.w;

                int real_y = _view_rect.y + sub_y;
                int real_x = _view_rect.x + sub_x;

                return (real_y * _image_rect.w) + real_x;
            }

            constexpr int xy_2_abs_idx(int x, int y)
            {
                return (y * _view_rect.w) + x;
            }
            
    	public:
            image_view_base(imgptr_t img, const rect<int>& view_rect)
                : _ptr(ptr)
                , _view_rect(view_rect)
                , _image_rect(0, 0, img->width(), img->height())
            { }

            std::array<uint8_t, 4> read_pixel(int index) const
            {
                return _ptr->read_pixel(cvt2_real_idx(index));
            }

            std::array<uint8_t, 4> read_pixel(int x, int y) const
            {
                return _ptr->read_pixel(cvt2_real_idx(xy_2_abs_idx(x, y)));
            }

            image build_image()
            {
                image result(_view_rect.w, _view_rect.h);

                int start_idx = (_view_rect.y * _image_rect.w) + _view_rect.x;
                for(int i = 0; i < _view_rect.h; ++i)
                {
                    int line_idx = (start_idx) * (i * _image_rect.w);

                    ptrdiff_t dst_offset = static_cast<ptrdiff_t>(i) * (_view_rect.w);
                    size_t copylen = _view_rect.w;

                    std::memcpy(
                        result.data()->data_r() + dst_offset,
                        _ptr->cdata_r() + line_idx, 
                        copylen
                    );
                    std::memcpy(
                        result.data()->data_g() + dst_offset,
                        _ptr->cdata_g() + line_idx, 
                        copylen
                    );
                    std::memcpy(
                        result.data()->data_b() + dst_offset,
                        _ptr->cdata_b() + line_idx, 
                        copylen
                    );
                    std::memcpy(
                        result.data()->data_a() + dst_offset,
                        _ptr->cdata_a() + line_idx, 
                        copylen
                    );
                }

                return result;
            }
        };
    }

    class image_view : public _internal::image_view_base<false>
    { };

    class mutable_image_view : public _internal::image_view_base<true>
    {
        void set_pixel(int index, uint8_t* rgba);
        void set_pixel(int x, int y, uint8_t* rgba);
    };
}