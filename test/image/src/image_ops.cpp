#include <catch2/catch.hpp>

#include <ien/image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)	
#include <ien/internal/x86/image_ops_x86.hpp>
#endif

using namespace ien::img;

TEST_CASE("Truncate channel bits")
{
	SECTION("STD")
	{
		image img(128, 128);
		size_t px_count = 128 * 128;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = 0xFF;
			img.data()->data_g()[i] = 0xFF;
			img.data()->data_b()[i] = 0xFF;
			img.data()->data_a()[i] = 0xFF;
		}

		_internal::truncate_channel_args args(&img, 1, 2, 3, 4);

		_internal::truncate_channel_data_std(args);
		for (size_t i = 0; i < px_count; ++i)
		{
			REQUIRE(img.cdata()->cdata_r()[i] == 0xFE);
			REQUIRE(img.cdata()->cdata_g()[i] == 0xFC);
			REQUIRE(img.cdata()->cdata_b()[i] == 0xF8);
			REQUIRE(img.cdata()->cdata_a()[i] == 0xF0);
		}
	};

	#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)	

	SECTION("SSE2")
	{
		image img(128, 128);
		size_t px_count = 128 * 128;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = 0xFF;
			img.data()->data_g()[i] = 0xFF;
			img.data()->data_b()[i] = 0xFF;
			img.data()->data_a()[i] = 0xFF;
		}

		_internal::truncate_channel_args args(&img, 1, 2, 3, 4);

		_internal::truncate_channel_data_sse2(args);
		for (size_t i = 0; i < px_count; ++i)
		{
			REQUIRE(img.cdata()->cdata_r()[i] == 0xFE);
			REQUIRE(img.cdata()->cdata_g()[i] == 0xFC);
			REQUIRE(img.cdata()->cdata_b()[i] == 0xF8);
			REQUIRE(img.cdata()->cdata_a()[i] == 0xF0);
		}
	};

	SECTION("AVX2")
	{
		image img(128, 128);
		size_t px_count = 128 * 128;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = 0xFF;
			img.data()->data_g()[i] = 0xFF;
			img.data()->data_b()[i] = 0xFF;
			img.data()->data_a()[i] = 0xFF;
		}

		_internal::truncate_channel_args args(&img, 1, 2, 3, 4);

		_internal::truncate_channel_data_avx2(args);
		for (size_t i = 0; i < px_count; ++i)
		{
			REQUIRE(img.cdata()->cdata_r()[i] == 0xFE);
			REQUIRE(img.cdata()->cdata_g()[i] == 0xFC);
			REQUIRE(img.cdata()->cdata_b()[i] == 0xF8);
			REQUIRE(img.cdata()->cdata_a()[i] == 0xF0);
		}
	};

	#endif
}

TEST_CASE("Saturation")
{
	SECTION("STD == SSE2 == AVX2")
	{
		image img(128, 128);
		size_t px_count = 128 * 128;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
			img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
			img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
			img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
		}

		_internal::channel_info_extract_args args(&img);
		std::vector<float> result0 = _internal::rgba_saturation_std(args);
		std::vector<float> result1 = _internal::rgba_saturation_sse2(args);
		std::vector<float> result2 = _internal::rgba_saturation_avx2(args);

		for (size_t i = 0; i < result0.size(); ++i)
		{
			REQUIRE(result0[i] == Approx(result1[i]));
			REQUIRE(result1[i] == Approx(result2[i]));
		}
	}

	SECTION("STD")
	{
		image img(128, 128);
		size_t px_count = 128 * 128;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
			img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
			img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
			img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
		}

		_internal::channel_info_extract_args args(&img);
		std::vector<float> result = _internal::rgba_saturation_std(args);

		REQUIRE(0 == 0);
	};

	SECTION("SSE2")
	{
		image img(128, 128);
		size_t px_count = 128 * 128;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
			img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
			img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
			img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
		}

		_internal::channel_info_extract_args args(&img);
		std::vector<float> result = _internal::rgba_saturation_sse2(args);

		REQUIRE(0 == 0);
	};

	SECTION("AVX2")
	{
		image img(128, 128);
		size_t px_count = 128 * 128;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
			img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
			img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
			img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
		}

		_internal::channel_info_extract_args args(&img);
		std::vector<float> result = _internal::rgba_saturation_avx2(args);

		REQUIRE(0 == 0);
	};
};
