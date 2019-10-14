#include <ien/internal/x86/image_ops_x86.hpp>

#include <ien/platform.hpp>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
#include <ien/internal/std/image_ops_std.hpp>
#include <ien/internal/image_ops_args.hpp>

#include <algorithm>
#include <immintrin.h>

#define BIND_CHANNELS(args, r, g, b, a) \
	uint8_t* r = args.ch_r; \
	uint8_t* g = args.ch_g; \
	uint8_t* b = args.ch_b; \
	uint8_t* a = args.ch_a

#define BIND_CHANNELS_CONST(args, r, g, b, a) \
	const uint8_t* r = args.ch_r; \
	const uint8_t* g = args.ch_g; \
	const uint8_t* b = args.ch_b; \
	const uint8_t* a = args.ch_a

#define LOAD_SI256(addr) \
	_mm256_load_si256(reinterpret_cast<__m256i*>(addr));

#define STORE_SI256(addr, v) \
	_mm256_store_si256(reinterpret_cast<__m256i*>(addr), v);

#define LOAD_SI256_CONST(addr) \
	_mm256_load_si256(reinterpret_cast<const __m256i*>(addr));

namespace ien::img::_internal
{
	const uint32_t trunc_and_table[8] = {
		0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
		0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
	};

	const size_t AVX2_STRIDE = 32;

	void truncate_channel_data_avx2(const truncate_channel_args& args)
	{
		const size_t img_sz = args.len;
		if (img_sz < AVX2_STRIDE)
		{
			truncate_channel_data_std(args);
			return;
		}

		BIND_CHANNELS(args, r, g, b, a);

		const __m256i vmask_r = _mm256_set1_epi8(trunc_and_table[args.bits_r]);
		const __m256i vmask_g = _mm256_set1_epi8(trunc_and_table[args.bits_g]);
		const __m256i vmask_b = _mm256_set1_epi8(trunc_and_table[args.bits_b]);
		const __m256i vmask_a = _mm256_set1_epi8(trunc_and_table[args.bits_a]);

		size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
		{
			__m256i seg_r = LOAD_SI256(r + i);
			__m256i seg_g = LOAD_SI256(g + i);
			__m256i seg_b = LOAD_SI256(b + i);
			__m256i seg_a = LOAD_SI256(a + i);

			seg_r = _mm256_and_si256(seg_r, vmask_r);
			seg_g = _mm256_and_si256(seg_g, vmask_g);
			seg_b = _mm256_and_si256(seg_b, vmask_b);
			seg_a = _mm256_and_si256(seg_a, vmask_a);

			STORE_SI256((r + i), seg_r);
			STORE_SI256((g + i), seg_g);
			STORE_SI256((b + i), seg_b);
			STORE_SI256((a + i), seg_a);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			r[i] &= trunc_and_table[args.bits_r];
			g[i] &= trunc_and_table[args.bits_g];
			b[i] &= trunc_and_table[args.bits_b];
			a[i] &= trunc_and_table[args.bits_a];
		}
	}

	std::vector<uint8_t> rgba_average_avx2(const channel_info_extract_args& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < AVX2_STRIDE)
		{
			return rgba_average_std(args);
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		BIND_CHANNELS_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
		{
			__m256i vseg_r = LOAD_SI256_CONST(r + i);
			__m256i vseg_g = LOAD_SI256_CONST(g + i);
			__m256i vseg_b = LOAD_SI256_CONST(b + i);
			__m256i vseg_a = LOAD_SI256_CONST(a + i);

			__m256i vavg_rg = _mm256_avg_epu8(vseg_r, vseg_g);
			__m256i vavg_ba = _mm256_avg_epu8(vseg_b, vseg_a);

			__m256i vagv_rgba = _mm256_avg_epu8(vavg_rg, vavg_ba);

			STORE_SI256((result.data() + i), vagv_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
			result[i] = static_cast<uint8_t>(sum / 4);
		}
		return result;
	}

	std::vector<uint8_t> rgba_max_avx2(const channel_info_extract_args& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < AVX2_STRIDE)
		{
			return rgba_max_std(args);
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		BIND_CHANNELS_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
		{
			__m256i vseg_r = LOAD_SI256_CONST(r + i);
			__m256i vseg_g = LOAD_SI256_CONST(g + i);
			__m256i vseg_b = LOAD_SI256_CONST(b + i);
			__m256i vseg_a = LOAD_SI256_CONST(a + i);

			__m256i vmax_rg = _mm256_max_epu8(vseg_r, vseg_g);
			__m256i vmax_ba = _mm256_max_epu8(vseg_b, vseg_a);
			__m256i vmax_rgba = _mm256_max_epu8(vmax_rg, vmax_ba);

			STORE_SI256((result.data() + i), vmax_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			result[i] = std::max({ r[i], g[i], b[i], a[i] });
		}
		return result;
	}

	std::vector<uint8_t> rgba_min_avx2(const channel_info_extract_args& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < AVX2_STRIDE)
		{
			return rgba_min_std(args);
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		BIND_CHANNELS_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
		{
			__m256i vseg_r = LOAD_SI256_CONST(r + i);
			__m256i vseg_g = LOAD_SI256_CONST(g + i);
			__m256i vseg_b = LOAD_SI256_CONST(b + i);
			__m256i vseg_a = LOAD_SI256_CONST(a + i);

			__m256i vmax_rg = _mm256_min_epu8(vseg_r, vseg_g);
			__m256i vmax_ba = _mm256_min_epu8(vseg_b, vseg_a);
			__m256i vmax_rgba = _mm256_min_epu8(vmax_rg, vmax_ba);

			STORE_SI256((result.data() + i), vmax_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			result[i] = std::min({ r[i], g[i], b[i], a[i] });
		}
		return result;
	}

	std::vector<uint8_t> rgba_sum_saturated_avx2(const channel_info_extract_args& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < AVX2_STRIDE)
		{
			return rgba_sum_saturated_std(args);
		}

		std::vector<uint8_t> result;
		result.resize(args.len);

		BIND_CHANNELS_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
		{
			__m256i vseg_r = LOAD_SI256_CONST(r + i);
			__m256i vseg_g = LOAD_SI256_CONST(g + i);
			__m256i vseg_b = LOAD_SI256_CONST(b + i);
			__m256i vseg_a = LOAD_SI256_CONST(a + i);

			__m256i vsum_rg = _mm256_adds_epu8(vseg_r, vseg_g);
			__m256i vsum_ba = _mm256_adds_epu8(vseg_b, vseg_a);
			__m256i vsum_rgba = _mm256_adds_epu8(vsum_rg, vsum_ba);

			STORE_SI256((result.data() + i), vsum_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			uint16_t aux = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
			aux = std::min(static_cast<uint16_t>(0x00FFu), aux);
			result[i] = static_cast<uint8_t>(aux);
		}
		return result;
	}

	std::vector<float> rgba_saturation_avx2(const channel_info_extract_args& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < AVX2_STRIDE)
		{
			return rgba_saturation_std(args);
		}

		std::vector<float> result;
		result.resize(args.len);

		BIND_CHANNELS_CONST(args, r, g, b, a);

		__m256i fpcast_mask = _mm256_set1_epi32(0x000000FF);

		size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
		{
			__m256i vseg_r = LOAD_SI256_CONST(r + i);
			__m256i vseg_g = LOAD_SI256_CONST(g + i);
			__m256i vseg_b = LOAD_SI256_CONST(b + i);

			__m256i vmax_rg = _mm256_max_epu8(vseg_r, vseg_g);
			__m256i vmax_rgb = _mm256_max_epu8(vseg_b, vmax_rg);

			__m256i vmin_rg = _mm256_min_epu8(vseg_r, vseg_g);
			__m256i vmin_rgb = _mm256_min_epu8(vseg_b, vmin_rg);

			__m256i vmax_aux0 = _mm256_and_si256(vmax_rgb, fpcast_mask);
			__m256i vmax_aux1 = _mm256_and_si256(_mm256_srli_epi32(vmax_rgb, 8), fpcast_mask);
			__m256i vmax_aux2 = _mm256_and_si256(_mm256_srli_epi32(vmax_rgb, 16), fpcast_mask);
			__m256i vmax_aux3 = _mm256_and_si256(_mm256_srli_epi32(vmax_rgb, 24), fpcast_mask);

			__m256i vmin_aux0 = _mm256_and_si256(vmin_rgb, fpcast_mask);
			__m256i vmin_aux1 = _mm256_and_si256(_mm256_srli_epi32(vmin_rgb, 8), fpcast_mask);
			__m256i vmin_aux2 = _mm256_and_si256(_mm256_srli_epi32(vmin_rgb, 16), fpcast_mask);
			__m256i vmin_aux3 = _mm256_and_si256(_mm256_srli_epi32(vmin_rgb, 24), fpcast_mask);

			__m256 vfmax0 = _mm256_cvtepi32_ps(vmax_aux0);
			__m256 vfmax1 = _mm256_cvtepi32_ps(vmax_aux1);
			__m256 vfmax2 = _mm256_cvtepi32_ps(vmax_aux2);
			__m256 vfmax3 = _mm256_cvtepi32_ps(vmax_aux3);

			__m256 vfmin0 = _mm256_cvtepi32_ps(vmin_aux0);
			__m256 vfmin1 = _mm256_cvtepi32_ps(vmin_aux1);
			__m256 vfmin2 = _mm256_cvtepi32_ps(vmin_aux2);
			__m256 vfmin3 = _mm256_cvtepi32_ps(vmin_aux3);

			__m256 vsat0 = _mm256_div_ps(_mm256_sub_ps(vfmax0, vfmin0), vfmax0);
			__m256 vsat1 = _mm256_div_ps(_mm256_sub_ps(vfmax1, vfmin1), vfmax1);
			__m256 vsat2 = _mm256_div_ps(_mm256_sub_ps(vfmax2, vfmin2), vfmax2);
			__m256 vsat3 = _mm256_div_ps(_mm256_sub_ps(vfmax3, vfmin3), vfmax3);

			float aux_result[32];

			_mm256_store_ps(aux_result + 0, vsat0);
			_mm256_store_ps(aux_result + 8, vsat1);
			_mm256_store_ps(aux_result + 16, vsat2);
			_mm256_store_ps(aux_result + 24, vsat3);

			for (int k = 0; k < 8; ++k)
			{
				result[0 + i + (k * 4)] = aux_result[0 + k];
				result[1 + i + (k * 4)] = aux_result[8 + k];
				result[2 + i + (k * 4)] = aux_result[16 + k];
				result[3 + i + (k * 4)] = aux_result[24 + k];
			}
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			float vmax = static_cast<float>(std::max({ r[i], g[i], b[i] })) / 255.0F;
			float vmin = static_cast<float>(std::min({ r[i], g[i], b[i] })) / 255.0F;
			result[i] = (vmax - vmin) / vmax;
		}

		return result;
	}
}
#endif