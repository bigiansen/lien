#pragma once

#include <ien/platform.hpp>
#include <cstddef>
#include <iterator>

namespace ien
{
    template<typename T, bool Const>
    class fixed_vector_iterator
    {     
    private:
        T* _data = nullptr;

    public:
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = std::conditional_t<Const, const T*, T*>;
        using reference = std::conditional_t<Const, const T&, T&>;
        using iterator_category = std::random_access_iterator_tag;
        using offset_t = long long;
        using iterator_type = fixed_vector_iterator<T, Const>;

        constexpr fixed_vector_iterator() noexcept { }

        constexpr fixed_vector_iterator(T* ptr) noexcept
            : _data(ptr)
        { }

        // LegacyIterator
        reference operator*() { return *_data; }
        
        iterator_type& operator++() noexcept
        { ++_data; return *this; }

        // LegacyForwardIterator
        iterator_type operator++(int) const noexcept
        { return iterator_type(_data + 1); }

        // LegacyBidirectionalIterator
        iterator_type& operator--() noexcept { --_data; return *this; }

        iterator_type operator--(int) const noexcept
        { return iterator_type(_data - 1); }

        // LegacyRandomAccessIterator
        iterator_type& operator+=(offset_t offset) noexcept
        { _data += offset; return *this; }

        iterator_type operator+(offset_t offset) const noexcept
        { return iterator_type(_data + offset); }

        iterator_type& operator-=(offset_t offset) noexcept
        { _data -= offset; return *this; }

        iterator_type operator-(offset_t offset) const noexcept
        { return iterator_type(_data - offset); }

        reference operator[](std::size_t index) noexcept { return _data[index]; }

        bool operator>(const iterator_type& other) const noexcept { return _data > other._data; }
        bool operator<(const iterator_type& other) const noexcept { return _data < other._data; }
        bool operator>=(const iterator_type& other) const noexcept { return _data >= other._data; }
        bool operator<=(const iterator_type& other) const noexcept { return _data <= other._data; }

        bool operator==(const iterator_type& other) const noexcept { return _data == other._data; }
        bool operator!=(const iterator_type& other) const noexcept { return _data != other._data; }

        template<typename X, bool C>
        friend fixed_vector_iterator<X, C> operator+(const fixed_vector_iterator<X, C>& it, offset_t offset) noexcept;

        template<typename X, bool C>
        friend fixed_vector_iterator<X, C> operator-(const fixed_vector_iterator<X, C>& it, offset_t offset) noexcept;
    };

    template<typename T, bool Const>
    fixed_vector_iterator<T, Const> operator+(
        const fixed_vector_iterator<T, Const>& it,
        long long offset) noexcept
    {
        return fixed_vector_iterator<T, Const>(it._data + offset);
    }

    template<typename T, bool Const>
    fixed_vector_iterator<T, Const> operator-(
        const fixed_vector_iterator<T, Const>& it,
        long long offset) noexcept
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
        using iterator = fixed_vector_iterator<T, false>;
        using const_iterator = fixed_vector_iterator<T, true>;

        fixed_vector(std::size_t len, std::size_t alignment = alignof(T))
            : _len(len)
            , _alignment(alignment)
        { 
            _data = reinterpret_cast<T*>(
                LIEN_ALIGNED_ALLOCV(len * sizeof(T), alignment)
            );
        }

        fixed_vector(fixed_vector&& mv_src) noexcept
            : _data(mv_src._data)
            , _len(mv_src._len)
            , _alignment(mv_src._alignment)
        { 
            mv_src._data = nullptr;
        }

        virtual ~fixed_vector()
        {
            if (_data == nullptr)
                return;

            LIEN_ALIGNED_FREE(_data);
        }

        std::size_t size() const noexcept { return _len; }
        std::size_t alignment() const noexcept { return _alignment; }

        T* data() noexcept { return _data; }
        const T* cdata() const noexcept { return _data; }

        T& operator[](std::size_t index)
        {
            return _data[index];
        }

        fixed_vector::iterator begin()
        {
            return fixed_vector_iterator<T, false>(_data);
        }

        fixed_vector::iterator end()
        {
            return fixed_vector_iterator<T, false>(_data + _len - 1);
        }

        fixed_vector::const_iterator cbegin() const
        {
            return fixed_vector_iterator<T, true>(_data);
        }

        fixed_vector::const_iterator cend() const
        {
            return fixed_vector_iterator<T, true>(_data + _len - 1);
        }
    };
}