#pragma once

#include <ien/assert.hpp>
#include <ien/debug.hpp>
#include <ien/platform.hpp>
#include <cstddef>
#include <stdexcept>

namespace ien
{
    template<typename T>
    constexpr bool is_ptr_aligned(const T* ptr, size_t alignment)
    {
        auto ptrval = 
            reinterpret_cast<const char*>(ptr) -
            reinterpret_cast<const char*>(nullptr);

        return (ptrval % alignment) == 0;
    }
    
    template<typename ... TArgs>
    LIEN_RELEASE_CONSTEXPR void debug_assert_ptr_aligned(size_t alignment, const TArgs&... ptrs)
    {
        static_assert((std::is_pointer_v<TArgs> && ...), "Argument list contains non-pointer type(s)");
        debug_assert((alignment & (alignment - 1)) == 0, "Alignment value must be a power of two");

        #ifndef NDEBUG
        bool aligned = (((reinterpret_cast<const char*>(ptrs) - reinterpret_cast<const char*>(nullptr)) % alignment == 0) && ...);
        if(!aligned)
        {
            throw std::logic_error("One or more pointers are not aligned to expected alignment value");
        }
        #endif
    }
}