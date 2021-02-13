#include <ien/planar_image_view.hpp>

#include <ien/planar_image.hpp>
#include <cstring>

namespace ien
{
    void planar_mutable_image_view::set_pixel(int index, uint8_t* rgba)
    {
        int idx = cvt2_real_idx(index);
        _ptr->data_r()[idx] = rgba[0];
        _ptr->data_g()[idx] = rgba[1];
        _ptr->data_b()[idx] = rgba[2];
        _ptr->data_a()[idx] = rgba[3];
    }

    void planar_mutable_image_view::set_pixel(int x, int y, uint8_t* rgba)
    {
        int idx = cvt2_real_idx(xy_2_abs_idx(x, y));
        _ptr->data_r()[idx] = rgba[0];
        _ptr->data_g()[idx] = rgba[1];
        _ptr->data_b()[idx] = rgba[2];
        _ptr->data_a()[idx] = rgba[3];
    }
}