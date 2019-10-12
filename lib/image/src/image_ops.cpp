#include <ien/image_ops.hpp>

#include <ien/platform.hpp>
#include <ien/internal/image_ops_args.hpp>
#include <ien/internal/std/image_ops_std.hpp>
#include <algorithm>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
#include <ien/internal/x86/image_ops_x86.hpp>

const size_t SSE2_STRIDE = 16;
const size_t AVX2_STRIDE = 32;
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

        _internal::truncate_channel_args args = { 
            img->size(),
            img->data_r(), img->data_g(), img->data_b(), img->data_a(),
            bits_r, bits_g, bits_b, bits_a
        };

        func(args);
    }

    std::vector<uint8_t> channel_average(const image* img)
    {
        typedef std::vector<uint8_t>(*func_ptr_t)(const _internal::channel_average_args&);

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        static func_ptr_t func = 
            platform::x86::get_feature(platform::x86::feature::AVX2)
            ? &_internal::channel_average_avx2
            : platform::x86::get_feature(platform::x86::feature::SSE2)
            ? &_internal::channel_average_sse2
            : &_internal::channel_average_std;

        #else
        static func_ptr_t func = &_internal::chanchannel_average_std;

        #endif

        _internal::channel_average_args args = {
            img->pixel_count(),
            img->cdata()->cdata_r(), 
            img->cdata()->cdata_g(), 
            img->cdata()->cdata_b(), 
            img->cdata()->cdata_a()
        };

        return func(args);
    }    

    std::vector<uint8_t> channel_max(const image* img)
    {
        typedef std::vector<uint8_t>(*func_ptr_t)(const _internal::channel_max_args&);

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        static func_ptr_t func = 
            platform::x86::get_feature(platform::x86::feature::AVX2)
            ? &_internal::channel_max_avx2
            : platform::x86::get_feature(platform::x86::feature::SSE2)
            ? &_internal::channel_max_sse2
            : &_internal::channel_max_std;
        #else
		static func_ptr_t func = &_internal::channel_max_std;
        #endif

		_internal::channel_max_args args = {
			img->pixel_count(),
			img->cdata()->cdata_r(),
			img->cdata()->cdata_g(),
			img->cdata()->cdata_b(),
			img->cdata()->cdata_a()
		};

		return func(args);
    }
}