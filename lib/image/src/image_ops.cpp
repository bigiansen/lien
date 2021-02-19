#include <ien/image_ops.hpp>

#include <ien/fixed_vector.hpp>
#include <ien/platform.hpp>
#include <ien/internal/image_ops_args.hpp>
#include <ien/internal/std/image_ops_std.hpp>
#include <algorithm>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
    #include <ien/internal/x86/image_ops_x86.hpp>
#elif (defined(LIEN_ARCH_ARM) || defined(LIEN_ARCH_ARM64)) && defined(LIEN_ARM_NEON)
    #include <ien/internal/arm/neon/image_ops_neon.hpp>
#endif

namespace ien::image_ops
{
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    #define HAS_SSE2()  platform::x86::get_feature(platform::x86::feature::SSE2)
    #define HAS_SSE3()  platform::x86::get_feature(platform::x86::feature::SSE3)
    #define HAS_SSSE3() platform::x86::get_feature(platform::x86::feature::SSSE3)
    #define HAS_SSE41() platform::x86::get_feature(platform::x86::feature::SSE41)
    #define HAS_SSE42() platform::x86::get_feature(platform::x86::feature::SSE42)
    #define HAS_AVX()   platform::x86::get_feature(platform::x86::feature::AVX)
    #define HAS_AVX2()  platform::x86::get_feature(platform::x86::feature::AVX2)

    template<typename TFuncPtr>
    TFuncPtr ARCH_X86_OVERLOAD_SELECT(TFuncPtr def, TFuncPtr sse2, TFuncPtr avx2)
    {
        #if defined(LIEN_ARCH_X86_64) // on x86-64 SSE2 is guaranteed
            return HAS_AVX2() ? avx2 : sse2;
        #elif defined(LIEN_ARCH_X86)
            return HAS_AVX2() ? avx2
                 : HAS_SSE2() ? sse2 : def;
        #else
            #error "Unable to select x86 overload on non-x86 platform!"
        #endif
    }
#endif

    void truncate_channel_data(image_planar_data* img, int bits_r, int bits_g, int bits_b, int bits_a)
    {
        typedef void(*func_ptr_t)(const _internal::truncate_channel_args& args);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::truncate_channel_data_std,
                &_internal::truncate_channel_data_sse2,
                &_internal::truncate_channel_data_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::truncate_channel_data_neon;
        #else
            static func_ptr_t func = &_internal::truncate_channel_data_std;
        #endif

        _internal::truncate_channel_args args(*img, bits_r, bits_g, bits_b, bits_a);
        func(args);
    }

    fixed_vector<uint8_t> rgba_average(const planar_image& img)
    {
        typedef fixed_vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgba_average_std,
                &_internal::rgba_average_sse2,
                &_internal::rgba_average_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgba_average_neon;
        #else
            static func_ptr_t func = &_internal::rgba_average_std;
        #endif

        _internal::channel_info_extract_args_rgba args(img);
        return func(args);
    }    

    fixed_vector<uint8_t> rgba_max(const planar_image& img)
    {
        typedef fixed_vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgba_max_std,
                &_internal::rgba_max_sse2,
                &_internal::rgba_max_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgba_max_neon;
        #else
            static func_ptr_t func = &_internal::rgba_max_std;
        #endif

        _internal::channel_info_extract_args_rgba args(img);
        return func(args);
    }

    fixed_vector<uint8_t> rgba_min(const planar_image& img)
    {
        typedef fixed_vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgba_min_std,
                &_internal::rgba_min_sse2,
                &_internal::rgba_min_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgba_min_neon;
        #else
            static func_ptr_t func = &_internal::rgba_min_std;
        #endif

        _internal::channel_info_extract_args_rgba args(img);
        return func(args);
    }

    fixed_vector<float> rgb_average(const planar_image& img)
    {
        typedef fixed_vector<float>(*func_ptr_t)(const _internal::channel_info_extract_args_rgb&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
        static func_ptr_t func = 
            HAS_AVX2()
            ? &_internal::rgb_average_avx2
            : 
            HAS_SSSE3()
            ? &_internal::rgb_average_sse41
            : 
            HAS_SSE2()
            ? &_internal::rgb_average_sse2
            : &_internal::rgb_average_std;
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgb_average_neon;
        #else
            static func_ptr_t func = &_internal::rgb_average_std;
        #endif

        _internal::channel_info_extract_args_rgb args(img);
        return func(args);
    }

    fixed_vector<uint8_t> rgb_max(const planar_image& img)
    {
        typedef fixed_vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgb&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgb_max_std,
                &_internal::rgb_max_sse2,
                &_internal::rgb_max_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgb_max_neon;
        #else
            static func_ptr_t func = &_internal::rgb_max_std;
        #endif

        _internal::channel_info_extract_args_rgb args(img);
        return func(args);
    }

    fixed_vector<uint8_t> rgb_min(const planar_image& img)
    {
        typedef fixed_vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgb&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgb_min_std,
                &_internal::rgb_min_sse2,
                &_internal::rgb_min_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgb_min_neon;
        #else
            static func_ptr_t func = &_internal::rgb_min_std;
        #endif

        _internal::channel_info_extract_args_rgb args(img);
        return func(args);
    }

    fixed_vector<uint8_t> rgba_sum_saturated(const planar_image& img)
    {
        typedef fixed_vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgba_sum_saturated_std,
                &_internal::rgba_sum_saturated_sse2,
                &_internal::rgba_sum_saturated_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgba_sum_saturated_neon;
        #else
            static func_ptr_t func = &_internal::rgba_sum_saturated_std;
        #endif

        _internal::channel_info_extract_args_rgba args(img);
        return func(args);
    }

    fixed_vector<float> rgb_saturation(const planar_image& img)
    {
        typedef fixed_vector<float>(*func_ptr_t)(const _internal::channel_info_extract_args_rgb&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgb_saturation_std,
                &_internal::rgb_saturation_sse2,
                &_internal::rgb_saturation_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgb_saturation_neon;
        #else
            static func_ptr_t func = &_internal::rgb_saturation_std;
        #endif

        _internal::channel_info_extract_args_rgb args(img);
        return func(args);
    }

    fixed_vector<float> rgb_luminance(const planar_image& img)
    {
        typedef fixed_vector<float>(*func_ptr_t)(const _internal::channel_info_extract_args_rgb&);

        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                &_internal::rgb_luminance_std,
                &_internal::rgb_luminance_sse2,
                &_internal::rgb_luminance_avx2
            );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::rgb_luminance_neon;
        #else
            static func_ptr_t func = &_internal::rgb_luminance_std;
        #endif

        _internal::channel_info_extract_args_rgb args(img);
        return func(args);
    }

	image_planar_data unpack_image_data(const uint8_t* data, size_t len)
	{
		typedef image_planar_data(*func_ptr_t)(const uint8_t*, size_t len);

		#if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = 
                HAS_AVX2()
                    ? &_internal::unpack_image_data_avx2
                    :
                HAS_SSSE3()
                    ? &_internal::unpack_image_data_ssse3
                    : &_internal::unpack_image_data_std;
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::unpack_image_data_neon;
		#else
            static func_ptr_t func = &_internal::unpack_image_data_std;
		#endif

		return func(data, len);
	}

    fixed_vector<uint8_t> channel_compare(const planar_image& img, rgba_channel channel, uint8_t threshold)
    {
        typedef fixed_vector<uint8_t>(*func_ptr_t)(const _internal::channel_compare_args& args);
        
        #if (defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = ARCH_X86_OVERLOAD_SELECT(
                    &_internal::channel_compare_avx2,
                    &_internal::channel_compare_sse2,
                    &_internal::channel_compare_std
                );
        #elif defined(LIEN_ARM_NEON) && defined(LIEN_USE_CUSTOM_SIMD)
            static func_ptr_t func = &_internal::channel_compare_neon;
		#else
            static func_ptr_t func = &_internal::channel_compare_std;
		#endif

        _internal::channel_compare_args args(img, channel, threshold);
		return func(args);
    }
}
