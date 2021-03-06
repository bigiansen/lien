#if defined(LIEN_BENCHMARK) && defined(NDEBUG)

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <ien/image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    #include <ien/internal/x86/image_ops_x86.hpp>
#elif defined(LIEN_ARM_NEON)
    #include <ien/internal/arm/neon/image_ops_neon.hpp>
#endif

using namespace ien;

void fill_image_random(planar_image& img)
{
    size_t imgsz = img.pixel_count();
    for (size_t i = 0; i < imgsz; ++i)
    {
        img.data()->data_r()[i] = static_cast<uint8_t>(rand());
        img.data()->data_g()[i] = static_cast<uint8_t>(rand());
        img.data()->data_b()[i] = static_cast<uint8_t>(rand());
        img.data()->data_a()[i] = static_cast<uint8_t>(rand());
    }
}

#define TRUNCATE_CHANNEL_BITS_SETUP(args) \
    planar_image img(IMG_DIM, IMG_DIM); \
    fill_image_random(img);\
    image_ops::_internal::truncate_channel_args args(img, 1, 2, 3, 4)

const size_t IMG_DIM = 200;

TEST_CASE("Benchmark truncate channel bits")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        TRUNCATE_CHANNEL_BITS_SETUP(args);
        meter.measure([&]
        {
            image_ops::_internal::truncate_channel_data_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        TRUNCATE_CHANNEL_BITS_SETUP(args);
        meter.measure([&]
        {
            image_ops::_internal::truncate_channel_data_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        TRUNCATE_CHANNEL_BITS_SETUP(args);
        meter.measure([&]
        {
            image_ops::_internal::truncate_channel_data_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        TRUNCATE_CHANNEL_BITS_SETUP(args);
        meter.measure([&]
        {
            image_ops::_internal::truncate_channel_data_neon(args);
        });
    };
#endif
}

#define EXTRACT_CHANNEL_DATA_RGBA_SETUP(args) \
    planar_image img(IMG_DIM, IMG_DIM); \
    fill_image_random(img);\
    image_ops::_internal::channel_info_extract_args_rgba args(img)

#define EXTRACT_CHANNEL_DATA_RGB_SETUP(args) \
    planar_image img(IMG_DIM, IMG_DIM); \
    fill_image_random(img);\
    image_ops::_internal::channel_info_extract_args_rgb args(img)

TEST_CASE("Benchmark rgba average")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_average_std(args);
        });
    };
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_average_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_average_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_average_neon(args);
        });
    };
#endif
};

TEST_CASE("Benchmark rgb average")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_average_std(args);
        });
    };
    #if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_average_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("SSE41")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_average_sse41(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_average_avx2(args);
        });
    };

    #elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_average_neon(args);
        });
    };
    #endif
};

TEST_CASE("Benchmark rgba max")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_max_std(args);
        });
    };
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_max_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_max_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_max_neon(args);
        });
    };
#endif
};

TEST_CASE("Benchmark rgba min")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_min_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_min_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_min_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_min_neon(args);
        });
    };
#endif
};

TEST_CASE("Benchmark rgb max")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_max_std(args);
        });
    };
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_max_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_max_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_max_neon(args);
        });
    };
#endif
};

TEST_CASE("Benchmark rgb min")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_min_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_min_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_min_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_min_neon(args);
        });
    };
#endif
};

TEST_CASE("Benchmark rgba sum saturated")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_sum_saturated_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_sum_saturated_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_sum_saturated_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgba_sum_saturated_neon(args);
        });
    };
#endif
};

TEST_CASE("Benchmark rgb saturation")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_saturation_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_saturation_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_saturation_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_saturation_neon(args);
        });
    };
#endif
};

TEST_CASE("Benchmark rgba luminance")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_luminance_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_luminance_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_luminance_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::rgb_luminance_neon(args);
        });
    };
#endif
};

#define IMG_DIM_UNPACK 128

TEST_CASE("Benchmark unpack image data")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<uint8_t> data(IMG_DIM_UNPACK * IMG_DIM_UNPACK);
        for (size_t i = 0; i < ((IMG_DIM_UNPACK * IMG_DIM_UNPACK) / 4); ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        meter.measure([&]
        {
            return image_ops::_internal::unpack_image_data_std(data.data(), data.size());
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSSE3")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<uint8_t> data(IMG_DIM_UNPACK * IMG_DIM_UNPACK);
        for (size_t i = 0; i < ((IMG_DIM_UNPACK * IMG_DIM_UNPACK) / 4); ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        meter.measure([&]
        {
            return image_ops::_internal::unpack_image_data_ssse3(data.data(), data.size());
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<uint8_t> data(IMG_DIM_UNPACK * IMG_DIM_UNPACK);
        for (size_t i = 0; i < ((IMG_DIM_UNPACK * IMG_DIM_UNPACK) / 4); ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        meter.measure([&]
        {
            return image_ops::_internal::unpack_image_data_avx2(data.data(), data.size());
        });
    };
    
#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        std::vector<uint8_t> data(IMG_DIM_UNPACK * IMG_DIM_UNPACK);
        for (size_t i = 0; i < ((IMG_DIM_UNPACK * IMG_DIM_UNPACK) / 4); ++i)
        {
            data[(i * 4) + 0] = 1;
            data[(i * 4) + 1] = 2;
            data[(i * 4) + 2] = 3;
            data[(i * 4) + 3] = 4;
        }

        meter.measure([&]
        {
            return image_ops::_internal::unpack_image_data_neon(data.data(), data.size());
        });
    };
#endif
};

#define COMPARE_CHANNEL_SETUP(args) \
    planar_image img(IMG_DIM, IMG_DIM); \
    fill_image_random(img);\
    image_ops::_internal::channel_compare_args args(img, rgba_channel::R, 123)

TEST_CASE("Benchmark channel compare")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        COMPARE_CHANNEL_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::channel_compare_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        COMPARE_CHANNEL_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::channel_compare_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        COMPARE_CHANNEL_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::channel_compare_avx2(args);
        });
    };

#elif defined(LIEN_ARM_NEON)
    BENCHMARK_ADVANCED("NEON")(Catch::Benchmark::Chronometer meter)
    {
        COMPARE_CHANNEL_SETUP(args);
        meter.measure([&]
        {
            return image_ops::_internal::channel_compare_neon(args);
        });
    };
#endif
};

#endif
