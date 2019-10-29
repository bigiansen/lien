#include <catch2/catch.hpp>

#include <ien/allocators/aligned_allocator.hpp>
#include <vector>

using namespace ien::allocators;

TEST_CASE("std::vector with aligned_allocator")
{
    // 8 bits
    for(int i = 0; i < 1000; ++i)
    {
        std::vector<float, aligned_allocator<float, 8>> vec;
        vec.reserve(i);
        ptrdiff_t ptrval = reinterpret_cast<ptrdiff_t>(vec.data());
        REQUIRE(ptrval % 8 == 0);
    }

    // 16 bits
    for(int i = 0; i < 1000; ++i)
    {
        std::vector<float, aligned_allocator<float, 16>> vec;
        vec.reserve(i);
        ptrdiff_t ptrval = reinterpret_cast<ptrdiff_t>(vec.data());
        REQUIRE(ptrval % 16 == 0);
    }

    // 32 bits
    for(int i = 0; i < 1000; ++i)
    {
        std::vector<float, aligned_allocator<float, 32>> vec;
        vec.reserve(i);
        ptrdiff_t ptrval = reinterpret_cast<ptrdiff_t>(vec.data());
        REQUIRE(ptrval % 32 == 0);
    }

    // 64 bits
    for(int i = 0; i < 1000; ++i)
    {
        std::vector<float, aligned_allocator<float, 64>> vec;
        vec.reserve(i);
        ptrdiff_t ptrval = reinterpret_cast<ptrdiff_t>(vec.data());
        REQUIRE(ptrval % 64 == 0);
    }

    // 128 bits
    for(int i = 0; i < 1000; ++i)
    {
        std::vector<float, aligned_allocator<float, 128>> vec;
        vec.reserve(i);
        ptrdiff_t ptrval = reinterpret_cast<ptrdiff_t>(vec.data());
        REQUIRE(ptrval % 128 == 0);
    }

    // 256 bits
    for(int i = 0; i < 1000; ++i)
    {
        std::vector<float, aligned_allocator<float, 256>> vec;
        vec.reserve(i);
        ptrdiff_t ptrval = reinterpret_cast<ptrdiff_t>(vec.data());
        REQUIRE(ptrval % 256 == 0);
    }
};