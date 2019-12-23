#include <ien/bit_view.hpp>

#include <ien/assert.hpp>

namespace ien
{
    bit_view::bit_view(uint8_t* data_ptr, size_t len)
    {
        _data_ptr = data_ptr;
        _len = len;
    }

    bool bit_view::operator[](size_t index) const
    {
        ien::debug_assert((index < (_len * 8)), "Out of range!");

        size_t byte_idx = index / 8;
        size_t bit_idx = index % 8;
        uint8_t mask = (uint8_t)1 << (7 - bit_idx);
        uint8_t byte = _data_ptr[byte_idx];

        return (byte | mask) == byte;
    }

    size_t bit_view::size() const
    {
        return _len * 8;
    }

    ien::fixed_vector<bool> bit_view::get_bits_at(size_t index, size_t count) const
    {
        ien::fixed_vector<bool> result(count);
        for(size_t i = 0; i < count; ++i)
        {
            result[i] = this->operator[](index++);
        }
        return result;
    }
}