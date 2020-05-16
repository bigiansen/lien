#pragma once

#include <ien/assert.hpp>
#include <ien/debug.hpp>
#include <ien/platform.hpp>
#include <cstddef>

namespace ien
{
    template<typename T>
    constexpr bool is_ptr_aligned(const T* ptr, size_t alignment)
    {
        auto ptrval = 
            reinterpret_cast<const char*>(ptr) -
            reinterpret_cast<const char*>(0);

        return (ptrval % alignment) == 0;
    }
}