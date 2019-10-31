#include <catch2/catch.hpp>

#include <ien/image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

using namespace ien::img;

TEST_CASE("[STD] Channel byte truncation")
{
    SECTION("STD")
    {
        image img(41, 41);
        size_t px_count = 41 * 41;

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

TEST_CASE("[STD] Channel average RGBA")
{
    SECTION("STD")
    {
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_average_std(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 7);
        }
    };
};

TEST_CASE("[STD] Channel max RGBA")
{
    SECTION("STD")
    {
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_max_std(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 15);
        }
    };
};

TEST_CASE("[STD] Channel min RGBA")
{
    SECTION("STD")
    {
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_min_std(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };
};

TEST_CASE("[STD] Channel sum saturated RGBA")
{
    SECTION("Below limit")
    {
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_sum_saturated_std(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 19);
        }
    };

    SECTION("Above limit")
    {
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 77;
            img.data()->data_g()[i] = 55;
            img.data()->data_b()[i] = 73;
            img.data()->data_a()[i] = 184;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_sum_saturated_std(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 255u);
        }
    };
};

TEST_CASE("[STD] Saturation")
{
    SECTION("STD")
    {
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }
 
        _internal::channel_info_extract_args_rgb args(&img);
        ien::fixed_vector<float> result = _internal::rgb_saturation_std(args);

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
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }

        _internal::channel_info_extract_args_rgb args(&img);
        ien::fixed_vector<float> result = _internal::rgb_luminance_std(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            REQUIRE(result[i] == Approx(0.007843137254902F));
        }
    };
};