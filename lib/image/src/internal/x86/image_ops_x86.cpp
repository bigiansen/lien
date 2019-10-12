#include <ien/internal/x86/image_ops_x86.hpp>

#include <ien/platform.hpp>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
#include <ien/internal/std/image_ops_std.hpp>
#include <ien/internal/image_ops_args.hpp>

#include <algorithm>
#include <immintrin.h>

namespace ien::img::_internal
{
	const uint32_t trunc_and_table[8] = {
		0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
		0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
	};

	const size_t SSE2_STRIDE = 16;
	const size_t AVX2_STRIDE = 32;

	void truncate_channel_data_sse2(const truncate_channel_args& args)
	{
		const size_t img_sz = args.len;
		if (img_sz < SSE2_STRIDE)
		{
			truncate_channel_data_std(args);
			return;
		}

		uint8_t* r = args.ch_r;
		uint8_t* g = args.ch_g;
		uint8_t* b = args.ch_b;
		uint8_t* a = args.ch_a;

		const __m128i vmask_r = _mm_set1_epi32(trunc_and_table[args.bits_r]);
		const __m128i vmask_g = _mm_set1_epi32(trunc_and_table[args.bits_g]);
		const __m128i vmask_b = _mm_set1_epi32(trunc_and_table[args.bits_b]);
		const __m128i vmask_a = _mm_set1_epi32(trunc_and_table[args.bits_a]);

		for (size_t i = 0; i < img_sz; i += SSE2_STRIDE)
		{
			__m128i seg_r = _mm_load_si128(reinterpret_cast<__m128i*>(r + i));
			__m128i seg_g = _mm_load_si128(reinterpret_cast<__m128i*>(g + i));
			__m128i seg_b = _mm_load_si128(reinterpret_cast<__m128i*>(b + i));
			__m128i seg_a = _mm_load_si128(reinterpret_cast<__m128i*>(a + i));

			seg_r = _mm_and_si128(seg_r, vmask_r);
			seg_g = _mm_and_si128(seg_g, vmask_g);
			seg_b = _mm_and_si128(seg_b, vmask_b);
			seg_a = _mm_and_si128(seg_a, vmask_a);

			_mm_store_si128(reinterpret_cast<__m128i*>(r + i), seg_r);
			_mm_store_si128(reinterpret_cast<__m128i*>(g + i), seg_g);
			_mm_store_si128(reinterpret_cast<__m128i*>(b + i), seg_b);
			_mm_store_si128(reinterpret_cast<__m128i*>(a + i), seg_a);
		}

		size_t remainder_idx = img_sz - (img_sz % SSE2_STRIDE);
		for (size_t i = remainder_idx; i < img_sz; ++i)
		{
			r[i] &= trunc_and_table[args.bits_r];
			g[i] &= trunc_and_table[args.bits_g];
			b[i] &= trunc_and_table[args.bits_b];
			a[i] &= trunc_and_table[args.bits_a];
		}
	}

	void truncate_channel_data_avx2(const truncate_channel_args& args)
	{
		const size_t img_sz = args.len;
		if (img_sz < AVX2_STRIDE)
		{
			truncate_channel_data_std(args);
			return;
		}

		uint8_t* r = args.ch_r;
		uint8_t* g = args.ch_g;
		uint8_t* b = args.ch_b;
		uint8_t* a = args.ch_a;

		const __m256i vmask_r = _mm256_set1_epi8(trunc_and_table[args.bits_r]);
		const __m256i vmask_g = _mm256_set1_epi8(trunc_and_table[args.bits_g]);
		const __m256i vmask_b = _mm256_set1_epi8(trunc_and_table[args.bits_b]);
		const __m256i vmask_a = _mm256_set1_epi8(trunc_and_table[args.bits_a]);

		for (size_t i = 0; i < img_sz; i += AVX2_STRIDE)
		{
			__m256i seg_r = _mm256_load_si256(reinterpret_cast<__m256i*>(r + i));
			__m256i seg_g = _mm256_load_si256(reinterpret_cast<__m256i*>(g + i));
			__m256i seg_b = _mm256_load_si256(reinterpret_cast<__m256i*>(b + i));
			__m256i seg_a = _mm256_load_si256(reinterpret_cast<__m256i*>(a + i));

			seg_r = _mm256_and_si256(seg_r, vmask_r);
			seg_g = _mm256_and_si256(seg_g, vmask_g);
			seg_b = _mm256_and_si256(seg_b, vmask_b);
			seg_a = _mm256_and_si256(seg_a, vmask_a);

			_mm256_store_si256(reinterpret_cast<__m256i*>(r + i), seg_r);
			_mm256_store_si256(reinterpret_cast<__m256i*>(g + i), seg_g);
			_mm256_store_si256(reinterpret_cast<__m256i*>(b + i), seg_b);
			_mm256_store_si256(reinterpret_cast<__m256i*>(a + i), seg_a);
		}

		size_t remainder_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = remainder_idx; i < img_sz; ++i)
		{
			r[i] &= trunc_and_table[args.bits_r];
			g[i] &= trunc_and_table[args.bits_g];
			b[i] &= trunc_and_table[args.bits_b];
			a[i] &= trunc_and_table[args.bits_a];
		}
	}

	std::vector<uint8_t> channel_average_sse2(const channel_average_args& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < SSE2_STRIDE)
		{
			return channel_average_std(args);
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		const uint8_t* r = args.ch_r;
		const uint8_t* g = args.ch_g;
		const uint8_t* b = args.ch_b;
		const uint8_t* a = args.ch_a;

		for (size_t i = 0; i < img_sz; i += SSE2_STRIDE)
		{
			__m128i vseg_r = _mm_load_si128(reinterpret_cast<const __m128i*>(r + i));
			__m128i vseg_g = _mm_load_si128(reinterpret_cast<const __m128i*>(g + i));
			__m128i vseg_b = _mm_load_si128(reinterpret_cast<const __m128i*>(b + i));
			__m128i vseg_a = _mm_load_si128(reinterpret_cast<const __m128i*>(a + i));

			__m128i vavg_rg = _mm_avg_epu8(vseg_r, vseg_g);
			__m128i vavg_ba = _mm_avg_epu8(vseg_b, vseg_a);

			__m128i vagv_rgba = _mm_avg_epu8(vavg_rg, vavg_ba);

			_mm_store_si128(reinterpret_cast<__m128i*>(result.data() + i), vagv_rgba);
		}

		size_t remainder_idx = img_sz - (img_sz % SSE2_STRIDE);
		for (size_t i = remainder_idx; i < img_sz; ++i)
		{
			uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
			result[i] = static_cast<uint8_t>(sum / 4);
		}
		return result;
	}

	std::vector<uint8_t> channel_average_avx2(const channel_average_args& args)
	{
		static const size_t AVX2_STRIDE = 32;
		const size_t img_sz = args.len;

		if (img_sz < AVX2_STRIDE)
		{
			return channel_average_std(args);			
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		const uint8_t* r = args.ch_r;
		const uint8_t* g = args.ch_g;
		const uint8_t* b = args.ch_b;
		const uint8_t* a = args.ch_a;

		for (size_t i = 0; i < img_sz; i += AVX2_STRIDE)
		{
			__m256i vseg_r = _mm256_load_si256(reinterpret_cast<const __m256i*>(r + i));
			__m256i vseg_g = _mm256_load_si256(reinterpret_cast<const __m256i*>(g + i));
			__m256i vseg_b = _mm256_load_si256(reinterpret_cast<const __m256i*>(b + i));
			__m256i vseg_a = _mm256_load_si256(reinterpret_cast<const __m256i*>(a + i));

			__m256i vavg_rg = _mm256_avg_epu8(vseg_r, vseg_g);
			__m256i vavg_ba = _mm256_avg_epu8(vseg_b, vseg_a);

			__m256i vagv_rgba = _mm256_avg_epu8(vavg_rg, vavg_ba);

			_mm256_store_si256(reinterpret_cast<__m256i*>(result.data() + i), vagv_rgba);
		}

		size_t remainder_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = remainder_idx; i < img_sz; ++i)
		{
			uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
			result[i] = static_cast<uint8_t>(sum / 4);
		}
		return result;
	}

	std::vector<uint8_t> channel_max_sse2(const channel_max_args& args)
	{
		static const size_t SSE2_STRIDE = 16;
		const size_t img_sz = args.len;

		if (img_sz < SSE2_STRIDE)
		{
			return channel_max_std(args);			
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		const uint8_t* r = args.ch_r;
		const uint8_t* g = args.ch_g;
		const uint8_t* b = args.ch_b;
		const uint8_t* a = args.ch_a;

		for (size_t i = 0; i < img_sz; i += SSE2_STRIDE)
		{
			__m128i vseg_r = _mm_load_si128(reinterpret_cast<const __m128i*>(r + i));
			__m128i vseg_g = _mm_load_si128(reinterpret_cast<const __m128i*>(g + i));
			__m128i vseg_b = _mm_load_si128(reinterpret_cast<const __m128i*>(b + i));
			__m128i vseg_a = _mm_load_si128(reinterpret_cast<const __m128i*>(a + i));

			__m128i vmax_rg = _mm_max_epu8(vseg_r, vseg_g);
			__m128i vmax_ba = _mm_max_epu8(vseg_b, vseg_a);
			__m128i vmax_rgba = _mm_max_epu8(vmax_rg, vmax_ba);

			_mm_store_si128(reinterpret_cast<__m128i*>(result.data() + i), vmax_rgba);
		}

		size_t remainder_idx = img_sz - (img_sz % SSE2_STRIDE);
		for (size_t i = remainder_idx; i < img_sz; ++i)
		{
			result[i] = std::max({ r[i], g[i], b[i], a[i] });
		}
		return result;
	}

	std::vector<uint8_t> channel_max_avx2(const channel_max_args& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < AVX2_STRIDE)
		{
			return channel_max_std(args);
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		const uint8_t* r = args.ch_r;
		const uint8_t* g = args.ch_g;
		const uint8_t* b = args.ch_b;
		const uint8_t* a = args.ch_a;

		for (size_t i = 0; i < img_sz; i += AVX2_STRIDE)
		{
			__m256i vseg_r = _mm256_load_si256(reinterpret_cast<const __m256i*>(r + i));
			__m256i vseg_g = _mm256_load_si256(reinterpret_cast<const __m256i*>(g + i));
			__m256i vseg_b = _mm256_load_si256(reinterpret_cast<const __m256i*>(b + i));
			__m256i vseg_a = _mm256_load_si256(reinterpret_cast<const __m256i*>(a + i));

			__m256i vmax_rg = _mm256_max_epu8(vseg_r, vseg_g);
			__m256i vmax_ba = _mm256_max_epu8(vseg_b, vseg_a);
			__m256i vmax_rgba = _mm256_max_epu8(vmax_rg, vmax_ba);

			_mm256_store_si256(reinterpret_cast<__m256i*>(result.data() + i), vmax_rgba);
		}

		size_t remainder_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = remainder_idx; i < img_sz; ++i)
		{
			result[i] = std::max({ r[i], g[i], b[i], a[i] });
		}
		return result;
	}
}
#endif