#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <ien/image_ops.hpp>

TEST_CASE("Image truncate")
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

	#ifdef NDEBUG
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
	#endif
};

TEST_CASE("Image max")
{
	SECTION("STD EQUAL SSSE3 EQUAL SSE2")
	{
		const int sz = (1024 * 1024 * 4);
		std::vector<uint8_t> std, sse2, ssse3;
		std.resize(1024 * 1024 * 4);		
		std::transform(std.begin(), std.end(), std.begin(), [](uint8_t)
		{
			return static_cast<uint8_t>(rand());
		});
		sse2 = std;
		ssse3 = std;

		std::vector<uint8_t> max_data_ssse3 =
			ien::img::_internal::max_channel_rgba_ssse3(sse2.data(), sse2.size());

		std::vector<uint8_t> max_data_std =
			ien::img::_internal::max_channel_rgba_std(std.data(), std.size());

		std::vector<uint8_t> max_data_sse2 =
			ien::img::_internal::max_channel_rgba_sse2(std.data(), std.size());

		REQUIRE(max_data_sse2.size() == max_data_ssse3.size());
		REQUIRE(max_data_sse2.size() == max_data_std.size());
		for (size_t i = 0; i < max_data_std.size(); ++i)
		{
			INFO("Index: " + std::to_string(i));
			REQUIRE(max_data_std[i] == max_data_sse2[i]);
			REQUIRE(max_data_std[i] == max_data_ssse3[i]);
		}
	};

	#ifdef NDEBUG
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
			return ien::img::_internal::max_channel_rgba_std(data.data(), data.size());
		});
		return data;
	};

	BENCHMARK_ADVANCED("SSSE3")(Catch::Benchmark::Chronometer meter)
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
			return ien::img::_internal::max_channel_rgba_ssse3(data.data(), data.size());
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
			return ien::img::_internal::max_channel_rgba_ssse3(data.data(), data.size());
		});
		return data;
	};
	#endif
};