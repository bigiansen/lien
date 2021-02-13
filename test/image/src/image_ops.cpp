#include <catch2/catch.hpp>

#include <ien/arithmetic.hpp>
#include <ien/planar_image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

using namespace ien;

TEST_CASE("[STD] Channel byte truncation")
{
    SECTION("STD")
    {
        planar_image img(41, 41);
        size_t px_count = img.pixel_count();

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 0xFF;
            img.data()->data_g()[i] = 0xFF;
            img.data()->data_b()[i] = 0xFF;
            img.data()->data_a()[i] = 0xFF;
        }

        image_ops::_internal::truncate_channel_args args(img, 1, 2, 3, 4);

        image_ops::_internal::truncate_channel_data_std(args);
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
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_average_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == (ien::safe_add<float>(1, 5, 10, 15) / 4));
        }
    };
};

TEST_CASE("[STD] Channel average RGB")
{
    SECTION("STD")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_average_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == (ien::safe_add<float>(1, 5, 10) / 3));
        }
    };
};

TEST_CASE("[STD] Channel max RGBA")
{
    SECTION("STD")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_max_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 15);
        }
    };
};

TEST_CASE("[STD] Channel min RGBA")
{
    SECTION("STD")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_min_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };
};

TEST_CASE("[STD] Channel max RGB")
{
    SECTION("STD")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_max_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 10);
        }
    };
};

TEST_CASE("[STD] Channel min RGB")
{
    SECTION("STD")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 1;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_min_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };
};

TEST_CASE("[STD] Channel sum saturated RGBA")
{
    SECTION("Below limit")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 19);
        }
    };

    SECTION("Above limit")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 77;
            img.data()->data_g()[i] = 55;
            img.data()->data_b()[i] = 73;
            img.data()->data_a()[i] = 184;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_std(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 255u);
        }
    };
};

TEST_CASE("[STD] Saturation")
{
    SECTION("STD")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }
 
        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_saturation_std(args);

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
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_luminance_std(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            REQUIRE(result[i] == Approx(0.007843137254902F));
        }
    };
};

TEST_CASE("[STD] Unpack Image Data")
{
    SECTION("STD")
    {
        std::vector<uint8_t> data(1024);

        for (size_t i = 0; i < 256; ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        ien::image_planar_data result = image_ops::_internal::unpack_image_data_std(data.data(), data.size());

        for(size_t i = 0; i < result.size(); ++i)
        {
            REQUIRE(result.cdata_r()[i] == 1);
            REQUIRE(result.cdata_g()[i] == 2);
            REQUIRE(result.cdata_b()[i] == 3);
            REQUIRE(result.cdata_a()[i] == 4);
        }
    }
};

TEST_CASE("[STD] Channel compare")
{
    SECTION("STD")
    {
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1 + i);
            img.data()->data_g()[i] = static_cast<uint8_t>(2 + i);
            img.data()->data_b()[i] = static_cast<uint8_t>(3 + i);
            img.data()->data_a()[i] = static_cast<uint8_t>(4 + i);
        }

        image_ops::_internal::channel_compare_args args(img, rgba_channel::R, 107);
        ien::fixed_vector<uint8_t> result = image_ops::_internal::channel_compare_std(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            bool res = static_cast<bool>(result[i]);
            bool cmp = (static_cast<uint8_t>(1 + i) >= 107);
            REQUIRE(res == cmp);
        }
    };
};