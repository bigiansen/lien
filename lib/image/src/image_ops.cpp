#include <ien/image_ops.hpp>

#include <ien/assert.hpp>
#include <ien/platform.hpp>

#include <algorithm>

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
#include <immintrin.h>
#include <tmmintrin.h>
#endif

namespace ien::img
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// TRUNCATE
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	void truncate_channel_bits(const truncate_args& args)
	{
		using fptr_t = void (*)(uint8_t*, size_t, int, int, int, int);

		#ifdef defined(LIEN_ARCH_X86_64)
		static fptr_t internal_func = &_internal::truncate_channel_bits_sse2;

		#elif defined(LIEN_ARCH_X86)
		static fptr_t internal_func = platform::x86::get_feature(platform::x86::feature::SSE2)
			? &_internal::truncate_channel_bits_sse2
			: &_internal::truncate_channel_bits_std;

		#else
		static fptr_t internal_func = &_internal::truncate_channel_bits_std;

		#endif

		if (args.bits_r || args.bits_g || args.bits_b || args.bits_a)
		{
			auto img_datalen = (args.image_width * args.image_height * args.image_channels);
			internal_func(args.image_data, img_datalen, args.bits_r, args.bits_g, args.bits_b, args.bits_a);
		}
	}

	namespace _internal
	{
		void truncate_channel_bits_std(uint8_t* data, size_t size, int r, int g, int b, int a)
		{
			if (size % 4 != 0)
			{
				throw std::invalid_argument("Specified size must be a multiple of 4");
			}

			uint32_t trunc_mask = 0;
			trunc_mask |= (1 << r) << 24;
			trunc_mask |= (1 << g) << 16;
			trunc_mask |= (1 << b) << 8;
			trunc_mask |= (1 << a);

			for (size_t i = 0; i < size; i += 4)
			{
				uint32_t* iptr = reinterpret_cast<uint32_t*>(data);
				*iptr &= trunc_mask;
			}
		}

		#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
		void truncate_channel_bits_sse2(uint8_t* data, size_t size, int r, int g, int b, int a)
		{
			if (size % 4 != 0)
			{
				throw std::invalid_argument("Specified size must be a multiple of 4");
			}

			uint8_t trunc_mask[16] = {
				(uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
				(uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
				(uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
				(uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
			};

			__m128i vtrunc_mask = _mm_loadu_si32(trunc_mask);
			for (size_t i = 0; i < size; i += 16)
			{
				__m128i vdata = _mm_loadu_si32(data + i);
				vdata = _mm_and_si128(vdata, vtrunc_mask);
				_mm_store_si128(reinterpret_cast<__m128i*>(data + i), vdata);
			}
		}
		#endif
	}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// MAX CHANNEL RGBA
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	std::vector<uint8_t> max_channel_rgba(const uint8_t* data, size_t len)
	{
		using fptr_t = std::vector<uint8_t> (*)(const uint8_t*, size_t);

		#if defined(LIEN_ARCH_X86_64)
		static fptr_t internal_func = &_internal::max_channel_rgba_ssse3;

		#elif defined(LIEN_ARCH_X86)
		static fptr_t internal_func = platform::x86::get_feature(platform::x86::feature::SSSE3)
			? &_internal::max_channel_rgba_ssse3
			: platform::x86::get_feature(platform::x86::feature::SSE2)
				? &_internal::max_channel_rgba_sse2
				: &_internal::max_channel_rgba_std

		#else 
		static fptr_t internal_func = _internal::max_channel_rgba_str;

		#endif

		return internal_func(data, len);
	}

	namespace _internal
	{
		std::vector<uint8_t> max_channel_rgba_std(const uint8_t* data, size_t len)
		{
			debug_assert(len % 4 == 0, "Length must be a multiple of 4 (4-bytes per channel)");
			std::vector<uint8_t> result;
			result.resize(len / 4);
			for (size_t i = 0; i < len; i += 4)
			{
				std::initializer_list<uint8_t> quad = {
					data[i + 0], data[i + 1], data[i + 2], data[i + 3]
				};
				result[i / 4] = std::max(quad);
			}
			return result;
		}

		#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)

		std::vector<uint8_t> max_channel_rgba_sse2(const uint8_t* data, size_t len)
		{
			debug_assert(len % 4 == 0, "Length must be a multiple of 4 (4-bytes per channel)");
			std::vector<uint8_t> result;
			result.resize(len / 4);
			for (size_t i = 0; i < len; i += 16)
			{
				__m128i v0 = _mm_load_si128(reinterpret_cast<const __m128i*>(data + i));
				__m128i v1 = _mm_srai_epi32(v0, 8);
				__m128i v2 = _mm_srai_epi32(v1, 8);
				__m128i v3 = _mm_srai_epi32(v2, 8);

				// Compute max
				v0 = _mm_max_epu8(v0, v1);
				v0 = _mm_max_epu8(v0, v2);
				v0 = _mm_max_epu8(v0, v3);

				static uint8_t aux[16];
				_mm_store_si128(reinterpret_cast<__m128i*>(aux), v0);

				result[(i / 4) + 0] = aux[0];
				result[(i / 4) + 1] = aux[4];
				result[(i / 4) + 2] = aux[8];
				result[(i / 4) + 3] = aux[12];
			}

			// Non SSE-able remainder
			for (size_t i = len - (len % 16); i < len; i += 4)
			{
				std::initializer_list<uint8_t> quad = {
					data[i + 0], data[i + 1], data[i + 2], data[i + 3]
				};
				result[i] = std::max(quad);
			}

			return result;
		}

		std::vector<uint8_t> max_channel_rgba_ssse3(const uint8_t* data, size_t len)
		{
			debug_assert(len % 4 == 0, "Length must be a multiple of 4 (4-bytes per channel)");
			std::vector<uint8_t> result;
			result.resize(len / 4);
			for (size_t i = 0; i < len; i += 16)
			{				
				static __m128i shuf_mask = _mm_set_epi8(12, 15, 14, 13, 8, 11, 10, 9, 4, 7, 6, 5, 0, 3, 2, 1);
				__m128i v0 = _mm_load_si128(reinterpret_cast<const __m128i*>(data + i));
				__m128i v1 = _mm_shuffle_epi8(v0, shuf_mask);
				__m128i v2 = _mm_shuffle_epi8(v1, shuf_mask);
				__m128i v3 = _mm_shuffle_epi8(v2, shuf_mask);

				// Compute max
				v0 = _mm_max_epu8(v0, v1);
				v0 = _mm_max_epu8(v0, v2);
				v0 = _mm_max_epu8(v0, v3);

				static uint8_t aux[16];
				_mm_store_si128(reinterpret_cast<__m128i*>(aux), v0);

				result[(i / 4) + 0] = aux[0];
				result[(i / 4) + 1] = aux[4];
				result[(i / 4) + 2] = aux[8];
				result[(i / 4) + 3] = aux[12];
			}

			// Non SSE-able remainder
			for (size_t i = len - (len % 16); i < len; i += 4)
			{
				std::initializer_list<uint8_t> quad = {
					data[i + 0], data[i + 1], data[i + 2], data[i + 3]
				};
				result[i] = std::max(quad);
			}
			
			return result;
		}
		#endif
	}
}