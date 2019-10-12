#define CATCH_CONFIG_ENABLE_BENCHMARKING
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

		_internal::truncate_channel_args args;
		args.bits_r = 1;
		args.bits_g = 2;
		args.bits_b = 3;
		args.bits_a = 4;
		args.ch_r = img.data()->data_r();
		args.ch_g = img.data()->data_g();
		args.ch_b = img.data()->data_b();
		args.ch_a = img.data()->data_a();
		args.len = px_count;

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

		_internal::truncate_channel_args args;
		args.bits_r = 1;
		args.bits_g = 2;
		args.bits_b = 3;
		args.bits_a = 4;
		args.ch_r = img.data()->data_r();
		args.ch_g = img.data()->data_g();
		args.ch_b = img.data()->data_b();
		args.ch_a = img.data()->data_a();
		args.len = px_count;

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

		_internal::truncate_channel_args args;
		args.bits_r = 1;
		args.bits_g = 2;
		args.bits_b = 3;
		args.bits_a = 4;
		args.ch_r = img.data()->data_r();
		args.ch_g = img.data()->data_g();
		args.ch_b = img.data()->data_b();
		args.ch_a = img.data()->data_a();
		args.len = px_count;

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
