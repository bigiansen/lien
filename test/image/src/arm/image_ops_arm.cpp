#include <catch2/catch.hpp>

#if defined(LIEN_ARM_NEON)

#include <ien/image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>
#include <ien/internal/arm/neon/image_ops_neon.hpp>

#include <iostream>

using namespace ien;

TEST_CASE("[ARM] Channel byte truncation")
{
    SECTION("NEON")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 0xFF;
            img.data()->data_g()[i] = 0xFF;
            img.data()->data_b()[i] = 0xFF;
            img.data()->data_a()[i] = 0xFF;
        }

        image_ops::_internal::truncate_channel_args args(img, 1, 2, 3, 4);

        image_ops::_internal::truncate_channel_data_neon(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(img.cdata()->cdata_r()[i] == 0xFE);
            REQUIRE(img.cdata()->cdata_g()[i] == 0xFC);
            REQUIRE(img.cdata()->cdata_b()[i] == 0xF8);
            REQUIRE(img.cdata()->cdata_a()[i] == 0xF0);
        }
    };    
};

TEST_CASE("[ARM] Channel average RGBA")
{
    SECTION("NEON")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_average_neon(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE((result[i] == 8 || result[i] == 7));
        }
    };
};

TEST_CASE("[ARM] Channel max RGBA")
{
    SECTION("NEON")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_max_neon(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 15);
        }
    };    
};

TEST_CASE("[ARM] Channel min RGBA")
{
    SECTION("NEON")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_min_neon(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };    
};

TEST_CASE("[ARM] Channel sum saturated RGBA")
{
    SECTION("NEON - Below limit")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_neon(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 19);
        }
    };

    SECTION("NEON - Above limit")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 77;
            img.data()->data_g()[i] = 55;
            img.data()->data_b()[i] = 73;
            img.data()->data_a()[i] = 184;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_neon(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 255u);
        }
    };
};

TEST_CASE("[ARM] Saturation")
{
    SECTION("STD == NEON")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
            img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
            img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
            img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result0 = image_ops::_internal::rgb_saturation_std(args);
        ien::fixed_vector<float> result1 = image_ops::_internal::rgb_saturation_neon(args);

        REQUIRE(result0.size() == img.pixel_count());
        REQUIRE(result1.size() == img.pixel_count());
        for (size_t i = 0; i < result0.size(); ++i)
        {
            REQUIRE(result0[i] == Approx(result1[i]));
        }
    };

    SECTION("NEON")
    {
        planar_image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_saturation_neon(args);

        REQUIRE(result.size() == img.pixel_count());
        for(const float& f : result)
        {
            REQUIRE(f == Approx(0.6666666F));
        }
    };
};

TEST_CASE("[ARM] Luminance")
{
    SECTION("NEON")
    {
        planar_image img(39, 39);
        size_t px_count = 39 * 39;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_luminance_neon(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            float v = ((img.data()->data_r()[i] * 0.2126F) / 255)
                + ((img.data()->data_g()[i] * 0.7152F) / 255)
                + ((img.data()->data_b()[i] * 0.0722F) / 255);
            REQUIRE(result[i] == Approx(v).margin(0.001F));
        }
    };
};

TEST_CASE("[ARM] Unpack Image Data")
{
    SECTION("NEON")
    {
        ien::fixed_vector<uint8_t> data(1024, 16);

        for (size_t i = 0; i < 256; ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        ien::image_planar_data result = image_ops::_internal::unpack_image_data_neon(data.data(), data.size());

        for(size_t i = 0; i < result.size(); ++i)
        {
            REQUIRE(result.cdata_r()[i] == 1);
            REQUIRE(result.cdata_g()[i] == 2);
            REQUIRE(result.cdata_b()[i] == 3);
            REQUIRE(result.cdata_a()[i] == 4);
        }
    };
};

#endif
