#pragma once

#include <ien/platform.hpp>
#include <cstddef>
#include <iterator>

namespace ien
{
    template<typename T, bool Const>
    class fixed_vector_iterator_base
    {
        friend class fixed_vector<T>;
        using enable_if_const = std::enable_if_t<Const>;

    private:
        T* _data = nullptr;

    public:
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = std::conditional_t<Const, const T*, T*>;
        using reference = std::conditional_t<Const, const T&, T&>;
        using iterator_category = std::random_access_iterator_tag;
        using offset_t = long long;

        constexpr fixed_vector_iterator() noexcept { }

        constexpr fixed_vector_iterator(T* ptr) noexcept
            : _data(ptr)
        { }

        // LegacyIterator
        reference operator*() { return *_data; }
        
        template<typename = enable_if_const>
        fixed_vector_iterator<T, Const>& operator++() noexcept 
        { ++_data; return *this; }

        // LegacyForwardIterator
        fixed_vector_iterator<T, Const> operator++(int) const noexcept 
        { return fixed_vector_iterator<T>(_data + 1); }

        // LegacyBidirectionalIterator
        template<typename = enable_if_const>
        fixed_vector_iterator<T, Const>& operator--() noexcept { --_data; return *this; }

        fixed_vector_iterator<T, Const> operator--(int) const noexcept 
        { return fixed_vector_iterator<T>(_data - 1); }

        // LegacyRandomAccessIterator
        template<typename = enable_if_const>
        fixed_vector_iterator<T, Const>& operator+=(offset_t offset) noexcept 
        { _data += offset; return *this; }

        fixed_vector_iterator<T, Const> operator+(offset_t offset) const noexcept 
        { return fixed_vector_iterator<T>(_data + offset); }

        friend operator+(offset_t, const fixed_vector_iterator<T, Const>&);

        template<typename = enable_if_const>
        fixed_vector_iterator<T, Const>& operator-=(offset_t offset) noexcept 
        { _data -= offset; return *this; }

        fixed_vector_iterator<T, Const> operator-(offset_t offset) const noexcept 
        { return fixed_vector_iterator<T>(_data - offset); }

        friend operator-(offset_t, const fixed_vector_iterator<T, Const>&);

        reference operator[](std::size_t index) noexcept { return _data[index]; }

        bool operator>(const fixed_vector_iterator<T, Const>& other) { return _data > other._data; }
        bool operator<(const fixed_vector_iterator<T, Const>& other) { return _data < other._data; }
        bool operator>=(const fixed_vector_iterator<T, Const>& other) { return _data >= other._data; }
        bool operator<=(const fixed_vector_iterator<T, Const>& other) { return _data <= other._data; }
    };

    template<typename T, bool Const>
    fixed_vector_iterator<T, Const> operator+(
        fixed_vector_iterator<T, Const>::offset_t offset, 
        const fixed_vector_iterator<T, Const>& it) noexcept
    {
        return fixed_vector_iterator<T, Const>(it._data + offset);
    }

    template<typename T, bool Const>
    fixed_vector_iterator<T, Const> operator-(
        fixed_vector_iterator<T, Const>::offset_t offset, 
        const fixed_vector_iterator<T, Const>& it) noexcept
    {
        return fixed_vector_iterator<T, Const>(it._data - offset);
    }

    template<typename T>
    class fixed_vector
    {
    private:
        T* _data = nullptr;
        const std::size_t _len;
        const std::size_t _alignment;

    public:
        fixed_vector(std::size_t len, std::size_t alignment = alignof(T))
            : _len(len)
            , _alignment(alignment)
        { 
            if (alignment == alignof(T))
            {
                _data = new T[len];
            }
            else
            {
                _data = LIEN_ALIGNED_ALLOCV(len, alignment);
            }
        }

        virtual ~fixed_vector()
        {
            if constexpr(_alignment == alignof(T))
            {
                delete[] _data;
            }
            else
            {
                LIEN_ALIGNED_FREE(_data);
            }
        }

        std::size_t size() const noexcept { return _len; }
        std::size_t alignment() const noexcept { return _alignment; }

        T* data() noexcept { return _data; }
        const T* cdata() const noexcept { return _data; }

        T& operator[](std::size_t index)
        {
            return _data[index];
        }

        fixed_vector_iterator<T> begin()
        {
            return fixed_vector_iterator<T>(_data);
        }

        fixed_vector_iterator<T> end()
        {
            return fixed_vector_iterator<T>(_data + _len);
        }

        fixed_vector_iterator<T> cbegin()
        {
            return fixed_vector_iterator<T>(_data);
        }

        fixed_vector_iterator<T> cend()
        {
            return fixed_vector_iterator<T>(_data + _len);
        }
    };
}