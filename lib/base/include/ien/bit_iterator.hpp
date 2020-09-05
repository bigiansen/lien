#pragma once

#include <cinttypes>
#include <type_traits>

#include <ien/bit_tools.hpp>

namespace ien
{
    enum class bit_iterator_mode
    {
        LSB_TO_MSB,
        MSB_TO_LSB
    };

    template<typename T, bit_iterator_mode Mode = bit_iterator_mode::LSB_TO_MSB>
    class bit_iterator
    {
        static_assert(std::is_integral_v<T>, "bit_iterator works only with integral types!");
        static_assert(sizeof(T) <= 64, "Max integral supported size is 64 bits" );

    private:
        const T* _data;
        size_t _len;
        size_t _current_item = 0;
        int8_t _current_bit = 0;        

    public:
        using data_t = T;
        constexpr size_t item_size_bits() { return sizeof(T) * 8; }

        bit_iterator(const data_t* ptr, size_t len)
            : _data(ptr)
            , _len(len)
        {
            if constexpr (Mode == bit_iterator_mode::MSB_TO_LSB)
            {
                _current_bit = static_cast<int8_t>(item_size_bits() - 1);
            }
        }

        size_t bitsize() const { return _len * sizeof(data_t); }

        [[nodiscard]] bool next()
        {
            bool result = get_bit(_data[_current_item], _current_bit);
            if constexpr (Mode == bit_iterator_mode::MSB_TO_LSB)
            {                
                if (--_current_bit == -1)
                {
                    _current_bit = static_cast<int8_t>(item_size_bits() - 1);
                    ++_current_item;
                }
            }
            else
            {
                if (++_current_bit == item_size_bits())
                {
                    _current_bit = 0;
                    ++_current_item;
                }
            }
            return result;
        }

        void reset()
        {
            _current_item = 0;
            _current_bit = 0;
        }

        void skip(size_t count)
        {
            size_t item_count = count / item_size_bits();
            size_t bit_count = count % item_size_bits();

            _current_item += item_count;
            
            if constexpr (Mode == bit_iterator_mode::MSB_TO_LSB)
            {
                _current_bit -= bit_count;
                if (_current_bit < 0)
                {
                    ++_current_item;
                    _current_bit = item_size_bits() - _current_bit;
                }
            }
            else
            {
                _current_bit += bit_count;
                if (_current_bit >= item_size_bits())
                {
                    ++_current_item;
                    _current_bit = _current_bit - item_size_bits();
                }
            }
        }
    };
}