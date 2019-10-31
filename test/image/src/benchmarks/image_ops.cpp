#ifdef NDEBUG

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <ien/image.hpp>
#include <ien/platform.hpp>
#include <ien/internal/std/image_ops_std.hpp>

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
#include <ien/internal/x86/image_ops_x86.hpp>
#endif

using namespace ien::img;

void fill_image_random(image* img)
{
    size_t imgsz = img->pixel_count();
    for (size_t i = 0; i < imgsz; ++i)
    {
        img->data()->data_r()[i] = static_cast<uint8_t>(rand());
        img->data()->data_g()[i] = static_cast<uint8_t>(rand());
        img->data()->data_b()[i] = static_cast<uint8_t>(rand());
        img->data()->data_a()[i] = static_cast<uint8_t>(rand());
    }
}

#define TRUNCATE_CHANNEL_BITS_SETUP(args) \
    image img(IMG_DIM, IMG_DIM); \
    fill_image_random(&img);\
    _internal::truncate_channel_args args(&img, 1, 2, 3, 4)



const size_t IMG_DIM = 256;

TEST_CASE("Benchmark truncate channel bits")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        TRUNCATE_CHANNEL_BITS_SETUP(args);
        meter.measure([&]
        {
            _internal::truncate_channel_data_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        TRUNCATE_CHANNEL_BITS_SETUP(args);
        meter.measure([&]
        {
            _internal::truncate_channel_data_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        TRUNCATE_CHANNEL_BITS_SETUP(args);
        meter.measure([&]
        {
            _internal::truncate_channel_data_avx2(args);
        });
    };
#endif
}

#define EXTRACT_CHANNEL_DATA_RGBA_SETUP(args) \
    image img(IMG_DIM, IMG_DIM); \
    fill_image_random(&img);\
    _internal::channel_info_extract_args_rgba args(&img)

#define EXTRACT_CHANNEL_DATA_RGB_SETUP(args) \
    image img(IMG_DIM, IMG_DIM); \
    fill_image_random(&img);\
    _internal::channel_info_extract_args_rgb args(&img)

TEST_CASE("Benchmark rgba average")
{
    BENCHMARK_ADVANCED("STD")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_average_std(args);
        });
    };
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_average_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_average_avx2(args);
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
            return _internal::rgba_max_std(args);
        });
    };
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_max_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_max_avx2(args);
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
            return _internal::rgba_min_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_min_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_min_avx2(args);
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
            return _internal::rgba_sum_saturated_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_sum_saturated_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGBA_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgba_sum_saturated_avx2(args);
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
            return _internal::rgb_saturation_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgb_saturation_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgb_saturation_avx2(args);
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
            return _internal::rgb_luminance_std(args);
        });
    };

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    BENCHMARK_ADVANCED("SSE2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgb_luminance_sse2(args);
        });
    };

    BENCHMARK_ADVANCED("AVX2")(Catch::Benchmark::Chronometer meter)
    {
        EXTRACT_CHANNEL_DATA_RGB_SETUP(args);
        meter.measure([&]
        {
            return _internal::rgb_luminance_avx2(args);
        });
    };
#endif
};

#endif