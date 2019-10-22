#include <ien/image_rect_view.hpp>

namespace ien::img
{
    image_rect_view::image_rect_view(const image_unpacked_data* data, const rect<int>& view_rect)
        : _ptr(data)
        , _view_rect(view_rect)
    { }
}