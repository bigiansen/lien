#pragma once

#include <type_traits>
#include <utility>
#include <vector>

namespace ien
{
    template<typename T>
    class multi_array_bit_iterator
    {
        static_assert(std::is_integral_v<T>, "Only integral template types are supported!");

    private:
        std::vector<std::pair<const T*, size_t>> _views;
        size_t _current_view = 0;
        size_t _current_item = 0;
        size_t _current_bit = 0;
        size_t _total_items = 0;

    public:
        void append_view(const T* ptr, size_t len)
        {
            _views.push_back({ptr, len});
            _total_items += len;
        }

        size_t total_items()
        {
            return _total_items;
        }

        size_t total_bits()
        {
            return _total_items * sizeof(T) * 8;
        }

        bool operator++()
        {
            if(_current_bit == ((sizeof(T) * 8) - 1))
            {
                const auto& [view_ptr, view_len] = _views[_current_view];
                if(_current_item == (view_len - 1))
                {
                    if(_current_view == _views.size() - 1)
                    {
                        return false;
                    }
                    else
                    {
                        _current_bit = 0;
                        _current_item = 0;
                        ++_current_view;
                    }
                }
                else
                {
                    _current_bit = 0;
                    ++_current_item;
                }
            }
            else
            {
                ++_current_bit;
            }
            return true;
        }

        bool operator++(int)
        {
            return operator++();
        }

        bool operator*()
        {
            const T& item = _views[_current_view].first[_current_item];
            return ien::get_bit(item, _current_bit);
        }
    };
}