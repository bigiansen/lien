#include <catch2/catch.hpp>

#include <ien/fixed_vector.hpp>
#include <vector>

using namespace ien;

TEST_CASE("Fixed vector alignment")
{
    SECTION("Default alignment")
    {
        for (int i = 0; i < 1000; ++i)
        {
            fixed_vector<float> fv(i);
            uintptr_t pval = reinterpret_cast<uintptr_t>(fv.data());
            REQUIRE(pval % alignof(float) == 0);
        }
    };

    SECTION("8 Bit alignment")
    {
        for (int i = 0; i < 1000; ++i)
        {
            fixed_vector<float> fv(i, 8);
            uintptr_t pval = reinterpret_cast<uintptr_t>(fv.data());
            REQUIRE(pval % 8 == 0);
        }
    };

    SECTION("16 Bit alignment")
    {
        for (int i = 0; i < 1000; ++i)
        {
            fixed_vector<float> fv(i, 16);
            uintptr_t pval = reinterpret_cast<uintptr_t>(fv.data());
            REQUIRE(pval % 16 == 0);
        }
    };

    SECTION("32 Bit alignment")
    {
        for (int i = 0; i < 1000; ++i)
        {
            fixed_vector<float> fv(i, 32);
            uintptr_t pval = reinterpret_cast<uintptr_t>(fv.data());
            REQUIRE(pval % 32 == 0);
        }
    };

    SECTION("64 Bit alignment")
    {
        for (int i = 0; i < 1000; ++i)
        {
            fixed_vector<float> fv(i, 64);
            uintptr_t pval = reinterpret_cast<uintptr_t>(fv.data());
            REQUIRE(pval % 64 == 0);
        }
    };

    SECTION("128 Bit alignment")
    {
        for (int i = 0; i < 1000; ++i)
        {
            fixed_vector<float> fv(i, 128);
            uintptr_t pval = reinterpret_cast<uintptr_t>(fv.data());
            REQUIRE(pval % 128 == 0);
        }
    };

    SECTION("256 Bit alignment")
    {
        std::vector<fixed_vector<float>> acc;
        for (int i = 0; i < 1000; ++i)
        {
            acc.push_back(fixed_vector<float>(i, 256));
            uintptr_t pval = reinterpret_cast<uintptr_t>(acc.back().data());
            REQUIRE(pval % 256 == 0);
        }
    };
};

TEST_CASE("Fixed vector r/w")
{
    fixed_vector<int> v(1000);

    for(int i = 0; i < 1000; ++i)
    {
        v[i] = i;
    }

    for(int i = 0; i < 1000; ++i)
    {
        REQUIRE(v[i] == i);
    }
}