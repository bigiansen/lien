#include <catch2/catch.hpp>

#include <ien/arithmetic.hpp>
#include <ien/planar_image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
#include <ien/internal/x86/image_ops_x86.hpp>
#endif

#include <cmath>
#include <iostream>

#include "utils.hpp"

using namespace ien;

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)

TEST_CASE("[x86] Channel byte truncation")
{
    SECTION("SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Channel byte truncation", return);
        planar_image img(41, 41);
        auto px_count = 41 * 41;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 0xFF;
            img.data()->data_g()[i] = 0xFF;
            img.data()->data_b()[i] = 0xFF;
            img.data()->data_a()[i] = 0xFF;
        }

        image_ops::_internal::truncate_channel_args args(img, 1, 2, 3, 4);

        image_ops::_internal::truncate_channel_data_sse2(args);
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
        LIEN_CHECK_AVX2("[x86] Channel byte truncation", return);
        planar_image img(71, 71);
        auto px_count = 71 * 71;

        for (size_t i = 0; i < px_count; ++i)
        {
            img.data()->data_r()[i] = 0xFF;
            img.data()->data_g()[i] = 0xFF;
            img.data()->data_b()[i] = 0xFF;
            img.data()->data_a()[i] = 0xFF;
        }

        image_ops::_internal::truncate_channel_args args(img, 1, 2, 3, 4);

        image_ops::_internal::truncate_channel_data_avx2(args);
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
        LIEN_CHECK_SSE2("[x86] Channel average RGBA", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 20;
            img.data()->data_g()[i] = 25;
            img.data()->data_b()[i] = 41;
            img.data()->data_a()[i] = 68;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_average_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == average<uint8_t>(20, 25, 41, 68));
        }
    };

    SECTION("AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Channel average RGBA", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_average_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == average<uint8_t>(1, 5, 10, 15));
        }
    };
};

TEST_CASE("[x86] Channel average RGB")
{
    SECTION("SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Channel average RGB", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 10;
            img.data()->data_g()[i] = 50;
            img.data()->data_b()[i] = 200;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_average_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == Approx(safe_add<float>(10, 50, 200) / 3));
        }
    };

    SECTION("SSE41")
    {
        LIEN_CHECK_SSE2("[x86] Channel average RGB", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 10;
            img.data()->data_g()[i] = 50;
            img.data()->data_b()[i] = 200;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_average_sse41(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == Approx(safe_add<float>(10, 50, 200) / 3));
        }
    };

    SECTION("AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Channel average RGB", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_average_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == Approx(safe_add<float>(1, 5, 10) / 3));
        }
    };
};

TEST_CASE("[x86] Channel max RGBA")
{
    SECTION("SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Channel max RGBA", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_max_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 15);
        }
    };

    SECTION("AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Channel max RGBA", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_max_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 15);
        }
    };
};

TEST_CASE("[x86] Channel min RGBA")
{
    SECTION("SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Channel min RGBA", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_min_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };

    SECTION("AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Channel min RGBA", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_min_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };
};

TEST_CASE("[x86] Channel max RGB")
{
    SECTION("SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Channel max RGB", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_max_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 10);
        }
    };

    SECTION("AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Channel max RGB", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 1;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 10;
            img.data()->data_a()[i] = 15;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_max_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 10);
        }
    };
};

TEST_CASE("[x86] Channel min RGB")
{
    SECTION("SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Channel min RGB", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 1;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_min_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };

    SECTION("AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Channel min RGB", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 1;
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        auto result = image_ops::_internal::rgb_min_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 3);
        }
    };
};

TEST_CASE("[x86] Channel sum saturated RGBA")
{
    SECTION("SSE2 - Below limit")
    {
        LIEN_CHECK_SSE2("[x86] Channel sum saturated RGBA", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 19);
        }
    };

    SECTION("SSE2 - Above limit")
    {
        LIEN_CHECK_SSE2("[x86] Channel sum saturated RGBA", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 77;
            img.data()->data_g()[i] = 55;
            img.data()->data_b()[i] = 73;
            img.data()->data_a()[i] = 184;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_sse2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 255u);
        }
    };

    SECTION("AVX2 - Below limit")
    {
        LIEN_CHECK_AVX2("[x86] Channel sum saturated RGBA", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 7;
            img.data()->data_g()[i] = 5;
            img.data()->data_b()[i] = 3;
            img.data()->data_a()[i] = 4;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 19);
        }
    };

    SECTION("AVX - Above limit")
    {
        LIEN_CHECK_SSE2("[x86] Channel sum saturated RGBA", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = 77;
            img.data()->data_g()[i] = 55;
            img.data()->data_b()[i] = 73;
            img.data()->data_a()[i] = 184;
        }

        image_ops::_internal::channel_info_extract_args_rgba args(img);
        auto result = image_ops::_internal::rgba_sum_saturated_avx2(args);
        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            REQUIRE(result[i] == 255u);
        }
    };
};

TEST_CASE("[x86] Saturation")
{
    SECTION("STD == SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Saturation", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
            img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
            img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
            img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result0 = image_ops::_internal::rgb_saturation_std(args);
        ien::fixed_vector<float> result1 = image_ops::_internal::rgb_saturation_sse2(args);

        REQUIRE(result0.size() == img.pixel_count());
        REQUIRE(result1.size() == img.pixel_count());
        for (size_t i = 0; i < result0.size(); ++i)
        {
            REQUIRE(result0[i] == Approx(result1[i]));
        }
    };

    SECTION("STD == AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Saturation", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(i + 1);
            img.data()->data_g()[i] = static_cast<uint8_t>(i + 2);
            img.data()->data_b()[i] = static_cast<uint8_t>(i + 3);
            img.data()->data_a()[i] = static_cast<uint8_t>(i + 4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result0 = image_ops::_internal::rgb_saturation_std(args);
        ien::fixed_vector<float> result1 = image_ops::_internal::rgb_saturation_avx2(args);

        REQUIRE(result0.size() == img.pixel_count());
        REQUIRE(result1.size() == img.pixel_count());
        for (size_t i = 0; i < result0.size(); ++i)
        {
            REQUIRE(result0[i] == Approx(result1[i]));
        }
    };

    SECTION("SSE2")
    {
        LIEN_CHECK_SSE2("[x86] Saturation", return);
        planar_image img(41, 41);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_saturation_sse2(args);

        REQUIRE(result.size() == img.pixel_count());
        for(const float& f : result)
        {
            REQUIRE(f == Approx(0.6666666F));
        }
    };

    SECTION("AVX2")
    {
        LIEN_CHECK_AVX2("[x86] Saturation", return);
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_saturation_avx2(args);

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
        planar_image img(39, 39);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1);
            img.data()->data_g()[i] = static_cast<uint8_t>(2);
            img.data()->data_b()[i] = static_cast<uint8_t>(3);
            img.data()->data_a()[i] = static_cast<uint8_t>(4);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_luminance_sse2(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            float v = ((img.data()->data_r()[i] * 0.2126F) / 255)
                + ((img.data()->data_g()[i] * 0.7152F) / 255)
                + ((img.data()->data_b()[i] * 0.0722F) / 255);
            REQUIRE(result[i] == Approx(v).margin(0.001F));
        }
    };

    SECTION("AVX2")
    {
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(i % 32);
            img.data()->data_g()[i] = static_cast<uint8_t>(i % 32);
            img.data()->data_b()[i] = static_cast<uint8_t>(i % 32);
            img.data()->data_a()[i] = static_cast<uint8_t>(i % 32);
        }

        image_ops::_internal::channel_info_extract_args_rgb args(img);
        ien::fixed_vector<float> result = image_ops::_internal::rgb_luminance_avx2(args);

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

TEST_CASE("[x86] Unpack Image Data")
{
    SECTION("AVX2")
    {
        std::vector<uint8_t> data(1024 * 1024);

        for (size_t i = 0; i < data.size() / 4; ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        ien::image_planar_data result = image_ops::_internal::unpack_image_data_avx2(data.data(), data.size());

        for (size_t i = 0; i < result.size(); ++i)
        {
            REQUIRE(result.cdata_r()[i] == 1);
            REQUIRE(result.cdata_g()[i] == 2);
            REQUIRE(result.cdata_b()[i] == 3);
            REQUIRE(result.cdata_a()[i] == 4);
        }
    };

    SECTION("SSSE3")
    {
        std::vector<uint8_t> data(1024*1024);

        for (size_t i = 0; i < data.size() / 4; ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        ien::image_planar_data result = image_ops::_internal::unpack_image_data_ssse3(data.data(), data.size());

        for(size_t i = 0; i < result.size(); ++i)
        {
            REQUIRE(result.cdata_r()[i] == 1);
            REQUIRE(result.cdata_g()[i] == 2);
            REQUIRE(result.cdata_b()[i] == 3);
            REQUIRE(result.cdata_a()[i] == 4);
        }
    };
};

TEST_CASE("[X86] Channel compare")
{
    SECTION("SSE2")
    {
        planar_image img(39, 39);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1 + i);
            img.data()->data_g()[i] = static_cast<uint8_t>(2 + i);
            img.data()->data_b()[i] = static_cast<uint8_t>(3 + i);
            img.data()->data_a()[i] = static_cast<uint8_t>(4 + i);
        }

        image_ops::_internal::channel_compare_args args(img, rgba_channel::R, 107);
        ien::fixed_vector<uint8_t> result = image_ops::_internal::channel_compare_sse2(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            bool res = static_cast<bool>(result[i]);
            bool cmp = (static_cast<uint8_t>(1 + i) >= 107);
            REQUIRE(res == cmp);
        }
    };

    SECTION("AVX2")
    {
        planar_image img(71, 71);

        for (size_t i = 0; i < img.pixel_count(); ++i)
        {
            img.data()->data_r()[i] = static_cast<uint8_t>(1 + i);
            img.data()->data_g()[i] = static_cast<uint8_t>(2 + i);
            img.data()->data_b()[i] = static_cast<uint8_t>(3 + i);
            img.data()->data_a()[i] = static_cast<uint8_t>(4 + i);
        }

        image_ops::_internal::channel_compare_args args(img, rgba_channel::R, 107);
        ien::fixed_vector<uint8_t> result = image_ops::_internal::channel_compare_avx2(args);

        REQUIRE(result.size() == img.pixel_count());
        for(size_t i = 0; i < result.size(); ++i)
        {
            bool res = static_cast<bool>(result[i]);
            bool cmp = (static_cast<uint8_t>(1 + i) >= 107);
            REQUIRE(res == cmp);
        }
    };
};

#endif