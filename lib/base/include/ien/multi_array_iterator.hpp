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
        size_t _total_len = 0;

    public:
        void append_view(T* ptr, size_t len)
        {
            _views.push_back({ptr, len});
            _total_len += len;
        }

        size_t total_length() const
        {
            return _total_len;
        }

        bool operator++()
        {
            if(_current_view >= _views.size()) { return false; }

            auto& [ptr, len] = _views[_current_view];
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

            auto& [ptr, len] = _views[_current_view];
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
            auto& [ptr, len] = _views[_current_view];
            return ptr[_current_view_offset];
        }
    };
}