#include <catch2/catch.hpp>

#include <ien/image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)	
#include <ien/internal/x86/image_ops_x86.hpp>
#endif

#include <iostream>

#include "utils.hpp"

using namespace ien::img;

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)

TEST_CASE("[x86] Channel byte truncation")
{
	SECTION("SSE2")
	{
        CHECK_SSE2("[x86] Channel byte truncation");
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
        CHECK_AVX2("[x86] Channel byte truncation");
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
};

TEST_CASE("[x86] Max channel RGBA")
{

}

TEST_CASE("[x86] Saturation")
{
    SECTION("STD == SSE2")
	{
        CHECK_SSE2("[x86] Saturation");
		image img(40, 40);
		size_t px_count = 40 * 40;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
			img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
			img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
			img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
		}

		_internal::channel_info_extract_args_rgb args(&img);
		std::vector<float> result0 = _internal::rgb_saturation_std(args);
		std::vector<float> result1 = _internal::rgb_saturation_sse2(args);

        REQUIRE(result0.size() == img.pixel_count());
        REQUIRE(result1.size() == img.pixel_count());
		for (size_t i = 0; i < result0.size(); ++i)
		{
			REQUIRE(result0[i] == Approx(result1[i]));
		}
	};

    SECTION("STD == AVX2")
	{
        CHECK_AVX2("[x86] Saturation");
		image img(70, 70);
		size_t px_count = 70 * 70;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
			img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
			img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
			img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
		}

		_internal::channel_info_extract_args_rgb args(&img);
		std::vector<float> result0 = _internal::rgb_saturation_std(args);
		std::vector<float> result1 = _internal::rgb_saturation_avx2(args);

        REQUIRE(result0.size() == img.pixel_count());
        REQUIRE(result1.size() == img.pixel_count());
		for (size_t i = 0; i < result0.size(); ++i)
		{
			REQUIRE(result0[i] == Approx(result1[i]));
		}
	};

    SECTION("SSE2")
	{
        CHECK_SSE2("[x86] Saturation");
		image img(40, 40);
		size_t px_count = 40 * 40;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(1);
			img.data()->data_g()[i] = static_cast<uint8_t>(2);
			img.data()->data_b()[i] = static_cast<uint8_t>(3);
			img.data()->data_a()[i] = static_cast<uint8_t>(4);
		}

		_internal::channel_info_extract_args_rgb args(&img);
		std::vector<float> result = _internal::rgb_saturation_sse2(args);

        REQUIRE(result.size() == img.pixel_count());
		for(float& f : result)
        {
            REQUIRE(f == Approx(0.6666666F));
        }
	};

	SECTION("AVX2")
	{
        CHECK_AVX2("[x86] Saturation");
		image img(70, 70);
		size_t px_count = 70 * 70;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(1);
			img.data()->data_g()[i] = static_cast<uint8_t>(2);
			img.data()->data_b()[i] = static_cast<uint8_t>(3);
			img.data()->data_a()[i] = static_cast<uint8_t>(4);
		}

		_internal::channel_info_extract_args_rgb args(&img);
		std::vector<float> result = _internal::rgb_saturation_avx2(args);

        REQUIRE(result.size() == img.pixel_count());
		for(float& f : result)
        {
            REQUIRE(f == Approx(0.6666666F));
        }
	};
};

TEST_CASE("[x86] Luminance")
{
	SECTION("SSE2")
	{
		image img(39, 39);
		size_t px_count = 39 * 39;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(1);
			img.data()->data_g()[i] = static_cast<uint8_t>(2);
			img.data()->data_b()[i] = static_cast<uint8_t>(3);
			img.data()->data_a()[i] = static_cast<uint8_t>(4);
		}

		_internal::channel_info_extract_args_rgb args(&img);
		std::vector<float> result = _internal::rgb_luminance_sse2(args);

		REQUIRE(result.size() == img.pixel_count());
		for(size_t i = 0; i < result.size(); ++i)
        {
			REQUIRE(result[i] == Approx(0.007843137254902F));
        }
	};

	SECTION("AVX2")
	{
		image img(71, 71);
		size_t px_count = 71 * 71;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(1);
			img.data()->data_g()[i] = static_cast<uint8_t>(2);
			img.data()->data_b()[i] = static_cast<uint8_t>(3);
			img.data()->data_a()[i] = static_cast<uint8_t>(4);
		}

		_internal::channel_info_extract_args_rgb args(&img);
		std::vector<float> result = _internal::rgb_luminance_avx2(args);

		REQUIRE(result.size() == img.pixel_count());
		for(size_t i = 0; i < result.size(); ++i)
        {
			REQUIRE(result[i] == Approx(0.00784313F).margin(0.00001F));
        }
	};
};

#endif