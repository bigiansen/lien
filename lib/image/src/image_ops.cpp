#include <ien/image_ops.hpp>

#include <ien/platform.hpp>
#include <ien/internal/image_ops_args.hpp>
#include <ien/internal/std/image_ops_std.hpp>
#include <algorithm>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
#include <ien/internal/x86/image_ops_x86.hpp>

constexpr size_t SSE2_STRIDE = 16;
constexpr size_t AVX2_STRIDE = 32;
#endif

namespace ien::img
{
    void truncate_channel_data(image_unpacked_data* img, int bits_r, int bits_g, int bits_b, int bits_a)
    {
        typedef void(*func_ptr_t)(const _internal::truncate_channel_args& args);

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        static func_ptr_t func = 
            platform::x86::get_feature(platform::x86::feature::AVX2)
            ? &_internal::truncate_channel_data_avx2
            : platform::x86::get_feature(platform::x86::feature::SSE2)
            ? &_internal::truncate_channel_data_sse2
            : &_internal::truncate_channel_data_std;
        #else
        static func_ptr_t func = &_internal::truncate_channel_data_std;
        #endif

        _internal::truncate_channel_args args(img, bits_r, bits_g, bits_b, bits_a);

        func(args);
    }

    std::vector<uint8_t> rgba_average(const image* img)
    {
        typedef std::vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        static func_ptr_t func = 
            platform::x86::get_feature(platform::x86::feature::AVX2)
            ? &_internal::rgba_average_avx2
            : platform::x86::get_feature(platform::x86::feature::SSE2)
            ? &_internal::rgba_average_sse2
            : &_internal::rgba_average_std;

        #else
        static func_ptr_t func = &_internal::rgba_average_std;

        #endif

		_internal::channel_info_extract_args_rgba args(img);

        return func(args);
    }    

    std::vector<uint8_t> rgba_max(const image* img)
    {
        typedef std::vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        static func_ptr_t func = 
            platform::x86::get_feature(platform::x86::feature::AVX2)
            ? &_internal::rgba_max_avx2
            : platform::x86::get_feature(platform::x86::feature::SSE2)
            ? &_internal::rgba_max_sse2
            : &_internal::rgba_max_std;
        #else
		static func_ptr_t func = &_internal::rgba_max_std;
        #endif

		_internal::channel_info_extract_args_rgba args(img);

		return func(args);
    }

	std::vector<uint8_t> rgba_min(const image* img)
	{
		typedef std::vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

		#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
		static func_ptr_t func =
			platform::x86::get_feature(platform::x86::feature::AVX2)
			? &_internal::rgba_min_avx2
			: platform::x86::get_feature(platform::x86::feature::SSE2)
			? &_internal::rgba_min_sse2
			: &_internal::rgba_min_std;
		#else
		static func_ptr_t func = &_internal::rgba_min_std;
		#endif

		_internal::channel_info_extract_args_rgba args(img);

		return func(args);
	}

	std::vector<uint8_t> rgba_sum_saturated(const image* img)
	{
		typedef std::vector<uint8_t>(*func_ptr_t)(const _internal::channel_info_extract_args_rgba&);

		#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
		static func_ptr_t func =
			platform::x86::get_feature(platform::x86::feature::AVX2)
			? &_internal::rgba_sum_saturated_avx2
			: platform::x86::get_feature(platform::x86::feature::SSE2)
			? &_internal::rgba_sum_saturated_sse2
			: &_internal::rgba_sum_saturated_std;
		#else
		static func_ptr_t func = &_internal::rgba_sum_saturated_std;
		#endif

		_internal::channel_info_extract_args_rgba args(img);

		return func(args);
	}

	std::vector<float> rgba_saturation(const image* img)
	{
		typedef std::vector<float>(*func_ptr_t)(const _internal::channel_info_extract_args_rgb&);

		#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
		static func_ptr_t func = 
			platform::x86::get_feature(platform::x86::feature::AVX2)
			? &_internal::rgb_saturation_avx2
			: platform::x86::get_feature(platform::x86::feature::SSE2)
			? &_internal::rgb_saturation_sse2
			: &_internal::rgb_saturation_std;

		#else
		static func_ptr_t func = &_internal::rgba_saturation_std;
		#endif

		_internal::channel_info_extract_args_rgb args(img);
		return func(args);
	}

	std::vector<float> rgba_luminance(const image* img)
	{
		typedef std::vector<float>(*func_ptr_t)(const _internal::channel_info_extract_args_rgb&);

		#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
		static func_ptr_t func = 
			platform::x86::get_feature(platform::x86::feature::AVX2)
			? &_internal::rgb_luminance_avx2
			: platform::x86::get_feature(platform::x86::feature::SSE2)
			? &_internal::rgb_luminance_sse2
			: &_internal::rgb_luminance_std;

		#else
		static func_ptr_t func = &_internal::rgba_luminance_std;
		#endif

		_internal::channel_info_extract_args_rgb args(img);
		return func(args);
	}
}