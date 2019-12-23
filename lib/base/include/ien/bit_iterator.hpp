#pragma once

#include <cinttypes>
#include <type_traits>

namespace ien
{
    template<typename T>
    class bit_iterator
    {
        static_assert(std::is_integral_v<T>, "bit_iterator works only with integral types!");

    private:
        const T* _data;
        size_t _len;
        size_t _current_item = 0;
        size_t _current_bit = 0;

        using item_sz = sizeof(T);

    public:
        bit_iterator(const T* ptr, size_t len)
            : _data(ptr)
            , _len(len)
        { }

        size_t length() const { return len }

        bool operator++()
        {
            if(++_current_bit == item_sz)
            {
                _current_bit = 0;
                ++_current_item;
                if(_current_item >= _len)
                {
                    return false;
                }
            }
            return true;
        }

        bool operator++(int) { return operator++(); }

        bool operator--()
        {
            if(_current_item == 0 && _current_bit == 0)
            {
                return false;
            }
            if(_current_bit == 0)
            {
                _current_bit = item_sz - 1;
                --_current_item;
            }
            else
            {
                --_current_bit;
            }
        }

        bool operator--(int) { return operator--(); }

        bool operator*()
        {
            T item = _data[_current_item];
            return (item >> _current_bit) | 1 == item;
        }
    };
}