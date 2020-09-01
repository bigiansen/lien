#pragma once

#include <ien/fixed_vector.hpp>
#include <cinttypes>

namespace ien
{
    class bit_view
    {
    private:
        uint8_t* _data_ptr;
        size_t _len;

    public:

        constexpr bit_view(uint8_t* data_ptr, size_t len)
            : _data_ptr(data_ptr)
            , _len(len)
        { }
        
        bool operator[](size_t index) const;

        ien::fixed_vector<bool> get_bits(size_t index, size_t count) const;

        size_t size() const;
    };
}