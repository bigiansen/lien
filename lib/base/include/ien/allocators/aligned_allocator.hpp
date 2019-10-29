#pragma once

#include <ien/platform.hpp>

namespace ien::allocators
{
    template<typename T, int Alignment>
    struct aligned_allocator
    {
        typedef T         value_type;
        typedef T*        pointer;
        typedef const T*  const_pointer;
        typedef T&        reference;
        typedef const T&  const_reference;
        typedef size_t    size_type;
        typedef ptrdiff_t difference_type;

        constexpr aligned_allocator() noexcept {}

        template <class U>
        constexpr aligned_allocator(const aligned_allocator<U, Alignment>&) noexcept { }

        // Needed, since std::allocator_traits does not work with non type template parameters
        template <typename U>
        struct rebind
        {
            typedef aligned_allocator<U, Alignment> other;
        };

        [[nodiscard]] T* allocate(size_t n, const void* hint = 0)
        {
            void* ptr = LIEN_ALIGNED_ALLOCV(sizeof(T) * n, Alignment);
            if(ptr != nullptr) 
            {
                return reinterpret_cast<T*>(ptr);
            }
            throw std::bad_alloc();
        }

        void deallocate(T* p, size_t n)
        {
            LIEN_ALIGNED_FREE(p);
        }
    };

    template<typename T, typename U, std::size_t Alignment>
    constexpr bool operator==(
        const aligned_allocator<T, Alignment>&, 
        const aligned_allocator<U, Alignment>&) noexcept 
    { return true; }

    template<typename T, typename U, std::size_t Alignment>
    constexpr bool operator!=(
        const aligned_allocator<T, Alignment>&, 
        const aligned_allocator<U, Alignment>&) noexcept 
    { return false; }
}