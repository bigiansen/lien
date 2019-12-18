#pragma once

#include <utility>
#include <vector>

namespace ien
{
    template<typename T>
    class multi_array_iterator
    {
    private:
        std::vector<std::pair<T*, size_t>> _views;
        size_t _current_view = 0;
        size_t _current_view_offset = 0;

    public:
        void append_view(T* ptr, size_t len)
        {
            _views.push_back({ptr, len});
        }

        bool operator++()
        {
            if(_current_view >= _views.size()) { return false; }

            [ptr, len] = _views[_current_view];
            if(++_current_view_offset == len)
            {
                ++_current_view;
                _current_view_offset = 0;
            }
            return true;
        }

        bool operator--()
        {
            if(_current_view == 0 && _current_view_offset == 0) { return false; }

            [ptr, len] = _views[_current_view];
            if(_current_view_offset == 0)
            {
                size_t prev_len = _views[--_current_view].second();
                _current_view_offset = prev_len - 1;
            }
            return true;
        }

        bool operator++(int) { return operator++(); }

        bool operator--(int) { return operator--(); }

        T& operator*()
        {
            return _views[_current_view].first()[_current_view_offset];
        }

        const T& operator*() const
        {
            return _views[_current_view].first()[_current_view_offset];
        }
    };
}