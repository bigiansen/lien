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
        image img(71, 71);
        size_t px_count = 71 * 71;

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

TEST_CASE("[x86] Channel average RGBA")
{
    SECTION("SSE2")
    {
        CHECK_SSE2("[x86] Channel average RGBA");
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
        auto result = _internal::rgba_average_sse2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE((result[i] == 8 || result[i] == 7));
        }
    };

    SECTION("AVX2")
    {
        CHECK_AVX2("[x86] Channel average RGBA");
        image img(71, 71);
        size_t px_count = 71 * 71;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_average_avx2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE((result[i] == 8 || result[i] == 7));
        }
    };
};

TEST_CASE("[x86] Channel max RGBA")
{
    SECTION("SSE2")
    {
        CHECK_SSE2("[x86] Channel max RGBA");
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
        auto result = _internal::rgba_max_sse2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 15);
        }
    };

    SECTION("AVX2")
    {
        CHECK_AVX2("[x86] Channel max RGBA");
        image img(71, 71);
        size_t px_count = 71 * 71;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_max_avx2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 15);
        }
    };
};

TEST_CASE("[x86] Channel min RGBA")
{
    SECTION("SSE2")
    {
        CHECK_SSE2("[x86] Channel min RGBA");
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
        auto result = _internal::rgba_min_sse2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };

    SECTION("AVX2")
    {
        CHECK_AVX2("[x86] Channel min RGBA");
        image img(71, 71);
        size_t px_count = 71 * 71;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_min_avx2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };
};

TEST_CASE("[x86] Channel sum saturated RGBA")
{
    SECTION("SSE2 - Below limit")
    {
        CHECK_SSE2("[x86] Channel sum saturated RGBA");
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
        auto result = _internal::rgba_sum_saturated_sse2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 19);
        }
    };

    SECTION("SSE2 - Above limit")
    {
        CHECK_SSE2("[x86] Channel sum saturated RGBA");
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
        auto result = _internal::rgba_sum_saturated_sse2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 255u);
        }
    };

    SECTION("AVX2 - Below limit")
    {
        CHECK_AVX2("[x86] Channel sum saturated RGBA");
        image img(71, 71);
        size_t px_count = 71 * 71;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_sum_saturated_avx2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 19);
        }
    };

    SECTION("AVX - Above limit")
    {
        CHECK_SSE2("[x86] Channel sum saturated RGBA");
        image img(71, 71);
        size_t px_count = 71 * 71;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 77;
            img.data()->data_g()[i] = 55;
            img.data()->data_b()[i] = 73;
            img.data()->data_a()[i] = 184;
        }

        _internal::channel_info_extract_args_rgba args(&img);
        auto result = _internal::rgba_sum_saturated_avx2(args);
        for (size_t i = 0; i < px_count; ++i)
        {
            REQUIRE(result[i] == 255u);
        }
    };
};

TEST_CASE("[x86] Saturation")
{
    SECTION("STD == SSE2")
    {
        CHECK_SSE2("[x86] Saturation");
        image img(41, 41);
        size_t px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
            img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
            img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
            img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
        }

        _internal::channel_info_extract_args_rgb args(&img);
        ien::fixed_vector<float> result0 = _internal::rgb_saturation_std(args);
        ien::fixed_vector<float> result1 = _internal::rgb_saturation_sse2(args);

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
        image img(71, 71);
        size_t px_count = 71 * 71;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
            img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
            img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
            img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
        }

        _internal::channel_info_extract_args_rgb args(&img);
        ien::fixed_vector<float> result0 = _internal::rgb_saturation_std(args);
        ien::fixed_vector<float> result1 = _internal::rgb_saturation_avx2(args);

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
        ien::fixed_vector<float> result = _internal::rgb_saturation_sse2(args);

        REQUIRE(result.size() == img.pixel_count());
        for(const float& f : result)
        {
            REQUIRE(f == Approx(0.6666666F));
        }
    };

    SECTION("AVX2")
    {
        CHECK_AVX2("[x86] Saturation");
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
        ien::fixed_vector<float> result = _internal::rgb_saturation_avx2(args);

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
        ien::fixed_vector<float> result = _internal::rgb_luminance_sse2(args);

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
            img.data()->data_r()[i] = static_cast<uint8_t>(i % 32);
            img.data()->data_g()[i] = static_cast<uint8_t>(i % 32);
            img.data()->data_b()[i] = static_cast<uint8_t>(i % 32);
            img.data()->data_a()[i] = static_cast<uint8_t>(i % 32);
        }

        _internal::channel_info_extract_args_rgb args(&img);
        ien::fixed_vector<float> result = _internal::rgb_luminance_avx2(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            float v = static_cast<float>(std::fmod(i, 32)) / 255.0F;
            REQUIRE(result[i] == Approx(v).margin(0.001F));
        }
    };
};

TEST_CASE("[x86] Unpack Image Data")
{
    SECTION("SSSE3")
    {
        std::vector<uint8_t> data(1024);

        for (size_t i = 0; i < 256; ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        ien::img::image_unpacked_data result = _internal::unpack_image_data_ssse3(data.data(), data.size());

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