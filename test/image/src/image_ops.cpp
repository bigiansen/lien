#include <catch2/catch.hpp>

#include <ien/image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

using namespace ien::img;

TEST_CASE("[STD] Channel byte truncation")
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
}

TEST_CASE("[STD] Saturation")
{
	SECTION("STD")
	{
		image img(34, 34);
		size_t px_count = 34 * 34;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(1);
			img.data()->data_g()[i] = static_cast<uint8_t>(2);
			img.data()->data_b()[i] = static_cast<uint8_t>(3);
			img.data()->data_a()[i] = static_cast<uint8_t>(4);
		}

		_internal::channel_info_extract_args args(&img);
		std::vector<float> result = _internal::rgba_saturation_std(args);

		REQUIRE(result.size() == img.pixel_count());
		for(size_t i = 0; i < result.size(); ++i)
        {
            REQUIRE(result[i] == Approx(0.6666666F));
        }
	};
};

TEST_CASE("[STD] Luminance")
{
	SECTION("STD")
	{
		image img(34, 34);
		size_t px_count = 34 * 34;

		for (size_t i = 0; i < px_count; ++i)
		{
			img.data()->data_r()[i] = static_cast<uint8_t>(1);
			img.data()->data_g()[i] = static_cast<uint8_t>(2);
			img.data()->data_b()[i] = static_cast<uint8_t>(3);
			img.data()->data_a()[i] = static_cast<uint8_t>(4);
		}

		_internal::channel_info_extract_args args(&img);
		std::vector<float> result = _internal::rgba_luminance_std(args);

		REQUIRE(result.size() == img.pixel_count());
		for(size_t i = 0; i < result.size(); ++i)
        {
			REQUIRE(result[i] == Approx(0.007843137254902F));
        }
	};
};