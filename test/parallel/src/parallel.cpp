#include <catch2/catch.hpp>
#include <ien/parallel.hpp>

#include <algorithm>
#include <limits>
#include <vector>

TEST_CASE("parallel_for")
{
    SECTION("simple increment")
    {
        static const int VEC_SZ = std::numeric_limits<uint16_t>::max();
        std::vector<int> vec, copy;
        vec.resize(VEC_SZ, 0);
        copy.resize(VEC_SZ, 0);

        std::transform(vec.begin(), vec.end(), vec.begin(), [](int i){ return std::rand(); });
        copy = vec;

        ien::parallel_for_params pfprms;
        pfprms.offset = 0;
        pfprms.count = static_cast<int>(vec.size());

        ien::parallel_for(pfprms, [&copy](int i)
        {
            copy[i] += 1;
        });

        for(size_t i = 0; i < vec.size(); ++i)
        {
            INFO("Index: " + std::to_string(i));
            REQUIRE(vec[i] == (copy[i] - 1));
        }
    }

    SECTION("size smaller than threads")
    {
        static const int VEC_SZ = std::max(std::thread::hardware_concurrency() - 1 , 1u);
        std::vector<int> vec;
        vec.resize(VEC_SZ, 0);

        ien::parallel_for_params pfprms;
        pfprms.offset = 0;
        pfprms.count = VEC_SZ;
        ien::parallel_for(pfprms, [&vec](int i)
        {
            vec[i] += 1;
        });

        for(size_t i = 0; i < vec.size(); ++i)
        {
            INFO("Index: " + std::to_string(i));
            REQUIRE(vec[i] == 1);
        }
    }
};