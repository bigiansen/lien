#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <ien/image_truncate.hpp>

TEST_CASE("Image SSE")
{
    SECTION("STD EQUAL SSE2")
    {
        const int sz = (1024 * 1024 * 4);
        std::vector<uint8_t> std;
        std::vector<uint8_t> sse;
        std.resize(1024 * 1024 * 4);
        sse = std;

        ien::img::_internal::truncate_channel_bits_std(std.data(), sz, 1, 2, 3, 4);
        ien::img::_internal::truncate_channel_bits_sse2(sse.data(), sz, 1, 2, 3, 4);

        for (int i = 0; i < (1024 * 1024); ++i)
        {
            REQUIRE(std[i] == sse[i]);
        }
    };

    BENCHMARK_ADVANCED("STANDARD")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<uint8_t> data;
        data.resize(1024 * 1024 * 4);
        std::transform(data.begin(), data.end(), data.begin(), [](uint8_t b)
        {
            static uint8_t roll = 0;
            return roll++;
        });
        meter.measure([&data]
        {
            ien::img::_internal::truncate_channel_bits_std(data.data(), (1024 * 1024 * 4), 1, 2, 3, 4);
        });
        return data;
    };

    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<uint8_t> data;
        data.resize(1024 * 1024 * 4);
        std::transform(data.begin(), data.end(), data.begin(), [](uint8_t b)
        {
            static uint8_t roll = 0;
            return roll++;
        });
        meter.measure([&data]
        {
            ien::img::_internal::truncate_channel_bits_sse2(data.data(), (1024 * 1024 * 4), 1, 2, 3, 4);
        });
        return data;
    };
};