#pragma once

#include <ien/alloc.hpp>
#include <ien/assert.hpp>
#include <ien/platform.hpp>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <numeric>

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

        difference_type operator-(iterator_type other) const noexcept
        { return _data - other._data; }

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
    protected:
        T* _data = nullptr;
        std::size_t _len;
        std::size_t _alignment;

    public:
        using iterator = fixed_vector_iterator<T, false>;
        using const_iterator = fixed_vector_iterator<T, true>;

        constexpr fixed_vector()
            : _len(0)
            , _alignment(alignof(T))
        { }

        fixed_vector(std::size_t len)
            : _len(len)
            , _alignment(alignof(T))
        {
            if(len == 0)
                return;

            _data = reinterpret_cast<T*>(
                ien::aligned_alloc(len * sizeof(T), _alignment)
            );

            if(_data == nullptr)
                throw std::bad_alloc();

            LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(_data, _alignment));
        }

        fixed_vector(std::size_t len, std::size_t alignment)
            : _len(len)
            , _alignment(alignment)
        {
            if(len == 0)
                return;

            _data = reinterpret_cast<T*>(
                ien::aligned_alloc(len * sizeof(T), _alignment)
            );

            if(_data == nullptr)
                throw std::bad_alloc();

            LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(_data, _alignment));
        }

        fixed_vector(const fixed_vector& cp_src)
            : _data(reinterpret_cast<T*>(ien::aligned_alloc(cp_src._len, cp_src._alignment)))
            , _len(cp_src._len)
            , _alignment(cp_src._alignment)
        {
            std::memcpy(_data, cp_src._data, _len);
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

            ien::aligned_free(_data);
        }

        std::size_t size() const noexcept { return _len; }
        std::size_t alignment() const noexcept { return _alignment; }

        T* data() noexcept { return _data; }
        const T* cdata() const noexcept { return _data; }

        T& operator[](std::size_t index)
        {
            return _data[index];
        }

        const T& operator[](std::size_t index) const
        {
            return _data[index];
        }

        T& at(std::size_t index)
        {
            return _data[index];
        }

        const T& at(std::size_t index) const
        {
            return _data[index];
        }

        void operator=(const fixed_vector<T>& cp_src)
        {
            _data = reinterpret_cast<T*>(LIEN_ALIGNED_REALLOC(_data, cp_src._len, cp_src._alignment));
            std::memcpy(_data, cp_src._data, cp_src.size());
            
            _alignment = cp_src._alignment;
            _len = cp_src._len;

            LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(_data, _alignment));
        }

        void operator=(fixed_vector<T>&& mv_src) noexcept
        {
            this->_data = mv_src._data;
            this->_alignment = mv_src._alignment;
            this->_len = mv_src._len;
            
            mv_src._data = nullptr;
            mv_src._len = 0;
        }

        fixed_vector::iterator begin()
        {
            return fixed_vector_iterator<T, false>(_data);
        }

        fixed_vector::iterator end()
        {
            return fixed_vector_iterator<T, false>(_data + _len - 1);
        }

        fixed_vector::const_iterator begin() const
        {
            return cbegin();
        }

        fixed_vector::const_iterator end() const
        {
            return cend();
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