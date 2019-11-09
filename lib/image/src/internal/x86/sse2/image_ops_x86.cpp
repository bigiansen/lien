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

#define BIND_CHANNELS_RGBA_CONST(args, r, g, b, a) \
    const uint8_t* r = args.ch_r; \
    const uint8_t* g = args.ch_g; \
    const uint8_t* b = args.ch_b; \
    const uint8_t* a = args.ch_a

#define BIND_CHANNELS_RGB_CONST(args, r, g, b) \
    const uint8_t* r = args.ch_r; \
    const uint8_t* g = args.ch_g; \
    const uint8_t* b = args.ch_b;

#define LOAD_SI128(addr) \
    _mm_load_si128(reinterpret_cast<__m128i*>(addr));

#define STORE_SI128(addr, v) \
    _mm_store_si128(reinterpret_cast<__m128i*>(addr), v);

#define STOREU_SI128(addr, v) \
    _mm_storeu_si128(reinterpret_cast<__m128i*>(addr), v);

#define LOAD_SI128_CONST(addr) \
    _mm_load_si128(reinterpret_cast<const __m128i*>(addr));

namespace ien::img::_internal
{
	const uint32_t trunc_and_table[8] = {
		0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
		0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
	};

	constexpr size_t SSE_STRIDE = 16;

	void truncate_channel_data_sse2(const truncate_channel_args& args)
	{
		const size_t img_sz = args.len;
		if (img_sz < SSE_STRIDE)
		{
			truncate_channel_data_std(args);
			return;
		}

		BIND_CHANNELS(args, r, g, b, a);

		const __m128i vmask_r = _mm_set1_epi32(trunc_and_table[args.bits_r]);
		const __m128i vmask_g = _mm_set1_epi32(trunc_and_table[args.bits_g]);
		const __m128i vmask_b = _mm_set1_epi32(trunc_and_table[args.bits_b]);
		const __m128i vmask_a = _mm_set1_epi32(trunc_and_table[args.bits_a]);

		size_t last_v_idx = img_sz - (img_sz % SSE_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE)
		{
			__m128i seg_r = LOAD_SI128(r + i);
			__m128i seg_g = LOAD_SI128(g + i);
			__m128i seg_b = LOAD_SI128(b + i);
			__m128i seg_a = LOAD_SI128(a + i);

			seg_r = _mm_and_si128(seg_r, vmask_r);
			seg_g = _mm_and_si128(seg_g, vmask_g);
			seg_b = _mm_and_si128(seg_b, vmask_b);
			seg_a = _mm_and_si128(seg_a, vmask_a);

			STORE_SI128((r + i), seg_r);
			STORE_SI128((g + i), seg_g);
			STORE_SI128((b + i), seg_b);
			STORE_SI128((a + i), seg_a);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			r[i] &= trunc_and_table[args.bits_r];
			g[i] &= trunc_and_table[args.bits_g];
			b[i] &= trunc_and_table[args.bits_b];
			a[i] &= trunc_and_table[args.bits_a];
		}
	}

	fixed_vector<uint8_t> rgba_average_sse2(const channel_info_extract_args_rgba& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < SSE_STRIDE)
		{
			return rgba_average_std(args);
		}

		fixed_vector<uint8_t> result(args.len, SSE_STRIDE);

		BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % SSE_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE)
		{
			__m128i vseg_r = LOAD_SI128_CONST(r + i);
			__m128i vseg_g = LOAD_SI128_CONST(g + i);
			__m128i vseg_b = LOAD_SI128_CONST(b + i);
			__m128i vseg_a = LOAD_SI128_CONST(a + i);

			__m128i vavg_rg = _mm_avg_epu8(vseg_r, vseg_g);
			__m128i vavg_ba = _mm_avg_epu8(vseg_b, vseg_a);

			__m128i vagv_rgba = _mm_avg_epu8(vavg_rg, vavg_ba);

			STOREU_SI128((result.data() + i), vagv_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
			result[i] = static_cast<uint8_t>(sum / 4);
		}
		return result;
	}

	fixed_vector<uint8_t> rgba_max_sse2(const channel_info_extract_args_rgba& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < SSE_STRIDE)
		{
			return rgba_max_std(args);
		}

		fixed_vector<uint8_t> result(args.len, SSE_STRIDE);

		BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % SSE_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE)
		{
			__m128i vseg_r = LOAD_SI128_CONST(r + i);
			__m128i vseg_g = LOAD_SI128_CONST(g + i);
			__m128i vseg_b = LOAD_SI128_CONST(b + i);
			__m128i vseg_a = LOAD_SI128_CONST(a + i);

			__m128i vmax_rg = _mm_max_epu8(vseg_r, vseg_g);
			__m128i vmax_ba = _mm_max_epu8(vseg_b, vseg_a);
			__m128i vmax_rgba = _mm_max_epu8(vmax_rg, vmax_ba);

			STOREU_SI128((result.data() + i), vmax_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			result[i] = std::max({ r[i], g[i], b[i], a[i] });
		}
		return result;
	}

	fixed_vector<uint8_t> rgba_min_sse2(const channel_info_extract_args_rgba& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < SSE_STRIDE)
		{
			return rgba_min_std(args);
		}

		fixed_vector<uint8_t> result(args.len, SSE_STRIDE);

		BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % SSE_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE)
		{
			__m128i vseg_r = LOAD_SI128_CONST(r + i);
			__m128i vseg_g = LOAD_SI128_CONST(g + i);
			__m128i vseg_b = LOAD_SI128_CONST(b + i);
			__m128i vseg_a = LOAD_SI128_CONST(a + i);

			__m128i vmax_rg = _mm_min_epu8(vseg_r, vseg_g);
			__m128i vmax_ba = _mm_min_epu8(vseg_b, vseg_a);
			__m128i vmax_rgba = _mm_min_epu8(vmax_rg, vmax_ba);

			STOREU_SI128((result.data() + i), vmax_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			result[i] = std::min({ r[i], g[i], b[i], a[i] });
		}
		return result;
	}

	fixed_vector<uint8_t> rgba_sum_saturated_sse2(const channel_info_extract_args_rgba& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < SSE_STRIDE)
		{
			return rgba_sum_saturated_std(args);
		}

		fixed_vector<uint8_t> result(args.len, SSE_STRIDE);

		BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

		size_t last_v_idx = img_sz - (img_sz % SSE_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE)
		{
			__m128i vseg_r = LOAD_SI128_CONST(r + i);
			__m128i vseg_g = LOAD_SI128_CONST(g + i);
			__m128i vseg_b = LOAD_SI128_CONST(b + i);
			__m128i vseg_a = LOAD_SI128_CONST(a + i);

			__m128i vsum_rg = _mm_adds_epu8(vseg_r, vseg_g);
			__m128i vsum_ba = _mm_adds_epu8(vseg_b, vseg_a);
			__m128i vsum_rgba = _mm_adds_epu8(vsum_rg, vsum_ba);

			STOREU_SI128((result.data() + i), vsum_rgba);
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
			result[i] = static_cast<uint8_t>(std::min(static_cast<uint16_t>(0x00FFu), sum));
		}
		return result;
	}

	fixed_vector<float> rgb_saturation_sse2(const channel_info_extract_args_rgb& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < SSE_STRIDE)
		{
			return rgb_saturation_std(args);
		}

		fixed_vector<float> result(args.len, SSE_STRIDE);

		BIND_CHANNELS_RGB_CONST(args, r, g, b);

		__m128i fpcast_mask = _mm_set1_epi32(0x000000FF);

		size_t last_v_idx = img_sz - (img_sz % SSE_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE)
		{
			__m128i vseg_r = LOAD_SI128_CONST(r + i);
			__m128i vseg_g = LOAD_SI128_CONST(g + i);
			__m128i vseg_b = LOAD_SI128_CONST(b + i);

			__m128i vmax_rg = _mm_max_epu8(vseg_r, vseg_g);
			__m128i vmax_rgb = _mm_max_epu8(vseg_b, vmax_rg);

			__m128i vmin_rg = _mm_min_epu8(vseg_r, vseg_g);
			__m128i vmin_rgb = _mm_min_epu8(vseg_b, vmin_rg);

			__m128i vmax_aux0 = _mm_and_si128(vmax_rgb, fpcast_mask);
			__m128i vmax_aux1 = _mm_and_si128(_mm_srli_epi32(vmax_rgb, 8), fpcast_mask);
			__m128i vmax_aux2 = _mm_and_si128(_mm_srli_epi32(vmax_rgb, 16), fpcast_mask);
			__m128i vmax_aux3 = _mm_and_si128(_mm_srli_epi32(vmax_rgb, 24), fpcast_mask);

			__m128i vmin_aux0 = _mm_and_si128(vmin_rgb, fpcast_mask);
			__m128i vmin_aux1 = _mm_and_si128(_mm_srli_epi32(vmin_rgb, 8), fpcast_mask);
			__m128i vmin_aux2 = _mm_and_si128(_mm_srli_epi32(vmin_rgb, 16), fpcast_mask);
			__m128i vmin_aux3 = _mm_and_si128(_mm_srli_epi32(vmin_rgb, 24), fpcast_mask);

			__m128 vfmax0 = _mm_cvtepi32_ps(vmax_aux0);
			__m128 vfmax1 = _mm_cvtepi32_ps(vmax_aux1);
			__m128 vfmax2 = _mm_cvtepi32_ps(vmax_aux2);
			__m128 vfmax3 = _mm_cvtepi32_ps(vmax_aux3);

			__m128 vfmin0 = _mm_cvtepi32_ps(vmin_aux0);
			__m128 vfmin1 = _mm_cvtepi32_ps(vmin_aux1);
			__m128 vfmin2 = _mm_cvtepi32_ps(vmin_aux2);
			__m128 vfmin3 = _mm_cvtepi32_ps(vmin_aux3);

			__m128 vsat0 = _mm_div_ps(_mm_sub_ps(vfmax0, vfmin0), vfmax0);
			__m128 vsat1 = _mm_div_ps(_mm_sub_ps(vfmax1, vfmin1), vfmax1);
			__m128 vsat2 = _mm_div_ps(_mm_sub_ps(vfmax2, vfmin2), vfmax2);
			__m128 vsat3 = _mm_div_ps(_mm_sub_ps(vfmax3, vfmin3), vfmax3);

			alignas(SSE_STRIDE) float aux_result[SSE_STRIDE];

			_mm_store_ps(aux_result + 0, vsat0);
			_mm_store_ps(aux_result + 4, vsat1);
			_mm_store_ps(aux_result + 8, vsat2);
			_mm_store_ps(aux_result + 12, vsat3);

			for (size_t k = 0; k < 4u; ++k)
			{
				size_t offset = i + (k * 4u);
				result[offset + 0] = aux_result[0 + k];
				result[offset + 1] = aux_result[4 + k];
				result[offset + 2] = aux_result[8 + k];
				result[offset + 3] = aux_result[12 + k];
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

	fixed_vector<float> rgb_luminance_sse2(const channel_info_extract_args_rgb& args)
	{
		const size_t img_sz = args.len;

		if (img_sz < SSE_STRIDE)
		{
			return rgb_luminance_std(args);
		}

		fixed_vector<float> result(args.len, SSE_STRIDE);

		BIND_CHANNELS_RGB_CONST(args, r, g, b);

		__m128i fpcast_mask = _mm_set1_epi32(0x000000FF);
		__m128 vhalfmul = _mm_set_ps1(0.5F);
		__m128 vzeroonediv = _mm_set_ps1(255.F);

		size_t last_v_idx = img_sz - (img_sz % SSE_STRIDE);
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE)
		{
			__m128i vseg_r = LOAD_SI128_CONST(r + i);
			__m128i vseg_g = LOAD_SI128_CONST(g + i);
			__m128i vseg_b = LOAD_SI128_CONST(b + i);

			__m128i vmax_rg = _mm_max_epu8(vseg_r, vseg_g);
			__m128i vmax_rgb = _mm_max_epu8(vseg_b, vmax_rg);

			__m128i vmin_rg = _mm_min_epu8(vseg_r, vseg_g);
			__m128i vmin_rgb = _mm_min_epu8(vseg_b, vmin_rg);

			__m128i vmax_aux0 = _mm_and_si128(vmax_rgb, fpcast_mask);
			__m128i vmax_aux1 = _mm_and_si128(_mm_srli_epi32(vmax_rgb, 8), fpcast_mask);
			__m128i vmax_aux2 = _mm_and_si128(_mm_srli_epi32(vmax_rgb, 16), fpcast_mask);
			__m128i vmax_aux3 = _mm_and_si128(_mm_srli_epi32(vmax_rgb, 24), fpcast_mask);

			__m128i vmin_aux0 = _mm_and_si128(vmin_rgb, fpcast_mask);
			__m128i vmin_aux1 = _mm_and_si128(_mm_srli_epi32(vmin_rgb, 8), fpcast_mask);
			__m128i vmin_aux2 = _mm_and_si128(_mm_srli_epi32(vmin_rgb, 16), fpcast_mask);
			__m128i vmin_aux3 = _mm_and_si128(_mm_srli_epi32(vmin_rgb, 24), fpcast_mask);

			__m128 vfmax0 = _mm_cvtepi32_ps(vmax_aux0);
			__m128 vfmax1 = _mm_cvtepi32_ps(vmax_aux1);
			__m128 vfmax2 = _mm_cvtepi32_ps(vmax_aux2);
			__m128 vfmax3 = _mm_cvtepi32_ps(vmax_aux3);

			__m128 vfmin0 = _mm_cvtepi32_ps(vmin_aux0);
			__m128 vfmin1 = _mm_cvtepi32_ps(vmin_aux1);
			__m128 vfmin2 = _mm_cvtepi32_ps(vmin_aux2);
			__m128 vfmin3 = _mm_cvtepi32_ps(vmin_aux3);

			__m128 vlum0 = _mm_mul_ps(_mm_add_ps(vfmax0, vfmin0), vhalfmul);
			__m128 vlum1 = _mm_mul_ps(_mm_add_ps(vfmax1, vfmin1), vhalfmul);
			__m128 vlum2 = _mm_mul_ps(_mm_add_ps(vfmax2, vfmin2), vhalfmul);
			__m128 vlum3 = _mm_mul_ps(_mm_add_ps(vfmax3, vfmin3), vhalfmul);

			vlum0 = _mm_div_ps(vlum0, vzeroonediv);
			vlum1 = _mm_div_ps(vlum1, vzeroonediv);
			vlum2 = _mm_div_ps(vlum2, vzeroonediv);
			vlum3 = _mm_div_ps(vlum3, vzeroonediv);

			alignas(SSE_STRIDE) float aux_result[SSE_STRIDE];

			_mm_store_ps(aux_result + 0, vlum0);
			_mm_store_ps(aux_result + 4, vlum1);
			_mm_store_ps(aux_result + 8, vlum2);
			_mm_store_ps(aux_result + 12, vlum3);

			for (size_t k = 0; k < 4u; ++k)
			{
				size_t offset = i + (k * 4u);
				result[offset + 0] = aux_result[0 + k];
				result[offset + 1] = aux_result[4 + k];
				result[offset + 2] = aux_result[8 + k];
				result[offset + 3] = aux_result[12 + k];
			}
		}

		for (size_t i = last_v_idx; i < img_sz; ++i)
		{
			float vmax = static_cast<float>(std::max({ r[i], g[i], b[i] }));
			float vmin = static_cast<float>(std::min({ r[i], g[i], b[i] }));
			float vsum = (vmax + vmin) * 0.5F;
			result[i] = (vsum / 255.0F);
		}

		return result;
	}

	image_unpacked_data unpack_image_data_ssse3(const uint8_t* data, size_t len)
	{
		if (len < SSE_STRIDE * 4)
		{
			return unpack_image_data_std(data, len);
		}

		image_unpacked_data result(len / 4);
		uint8_t* r = result.data_r();
		uint8_t* g = result.data_g();
		uint8_t* b = result.data_b();
		uint8_t* a = result.data_a();

		const __m128i vshufmask = _mm_set_epi8(
			15, 11, 7, 3,
			14, 10, 6, 2,
			13, 9, 5, 1,
			12, 8, 4, 0
		);

		size_t last_v_idx = len - (len % (SSE_STRIDE * 4));
		for (size_t i = 0; i < last_v_idx; i += SSE_STRIDE * 4)
		{
			__m128i vdata0 = LOAD_SI128_CONST(data + i + (SSE_STRIDE * 0));
			__m128i vdata1 = LOAD_SI128_CONST(data + i + (SSE_STRIDE * 1));
			__m128i vdata2 = LOAD_SI128_CONST(data + i + (SSE_STRIDE * 2));
			__m128i vdata3 = LOAD_SI128_CONST(data + i + (SSE_STRIDE * 3));

			__m128i v_di_data0 = _mm_shuffle_epi8(vdata0, vshufmask);
			__m128i v_di_data1 = _mm_shuffle_epi8(vdata1, vshufmask);
			__m128i v_di_data2 = _mm_shuffle_epi8(vdata2, vshufmask);
			__m128i v_di_data3 = _mm_shuffle_epi8(vdata3, vshufmask);

			__m128i v_r0r1g0g1 = _mm_unpacklo_epi32(v_di_data0, v_di_data1);
			__m128i v_r2r3g2g3 = _mm_unpacklo_epi32(v_di_data2, v_di_data3);
			__m128i v_r0r1r2r3 = _mm_unpacklo_epi64(v_r0r1g0g1, v_r2r3g2g3);
			__m128i v_g0g1g2g3 = _mm_unpackhi_epi64(v_r0r1g0g1, v_r2r3g2g3);

			__m128i v_b0b1a0a1 = _mm_unpackhi_epi32(v_di_data0, v_di_data1);
			__m128i v_b2b3a2a3 = _mm_unpackhi_epi32(v_di_data2, v_di_data3);
			__m128i v_b0b1b2b3 = _mm_unpacklo_epi64(v_b0b1a0a1, v_b2b3a2a3);
			__m128i v_a0a1a2a3 = _mm_unpackhi_epi64(v_b0b1a0a1, v_b2b3a2a3);

			STORE_SI128(r + (i / 4), v_r0r1r2r3);
			STORE_SI128(g + (i / 4), v_g0g1g2g3);
			STORE_SI128(b + (i / 4), v_b0b1b2b3);
			STORE_SI128(a + (i / 4), v_a0a1a2a3);
		}

		for (size_t i = last_v_idx; i < len; ++i)
		{
			r[i / 4] = data[i + 0];
			g[i / 4] = data[i + 1];
			b[i / 4] = data[i + 2];
			a[i / 4] = data[i + 3];
		}

		return result;
	}
}
#endif