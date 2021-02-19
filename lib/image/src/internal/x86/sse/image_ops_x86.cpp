#include <ien/internal/x86/image_ops_x86.hpp>

#include <ien/platform.hpp>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
#include <ien/internal/std/image_ops_std.hpp>
#include <ien/internal/image_ops_args.hpp>

#include <ien/arithmetic.hpp>
#include <algorithm>
#include <immintrin.h>

#define SSE_ALIGNMENT 16

#define DEBUG_ASSERT_RGBA_ALIGNED(r, g, b, a) \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(r, SSE_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(g, SSE_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(b, SSE_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(a, SSE_ALIGNMENT))

#define DEBUG_ASSERT_RGB_ALIGNED(r, g, b) \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(r, SSE_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(g, SSE_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(b, SSE_ALIGNMENT))    

#define BIND_CHANNELS(args, r, g, b, a) \
    uint8_t* r = args.ch_r; \
    uint8_t* g = args.ch_g; \
    uint8_t* b = args.ch_b; \
    uint8_t* a = args.ch_a; \
    DEBUG_ASSERT_RGBA_ALIGNED(r, g, b, a)

#define BIND_CHANNELS_RGBA_CONST(args, r, g, b, a) \
    const uint8_t* r = args.ch_r; \
    const uint8_t* g = args.ch_g; \
    const uint8_t* b = args.ch_b; \
    const uint8_t* a = args.ch_a; \
    DEBUG_ASSERT_RGBA_ALIGNED(r, g, b, a)

#define BIND_CHANNELS_RGB_CONST(args, r, g, b) \
    const uint8_t* r = args.ch_r; \
    const uint8_t* g = args.ch_g; \
    const uint8_t* b = args.ch_b; \
    DEBUG_ASSERT_RGB_ALIGNED(r, g, b)

#define LOAD_SI128(addr) \
    _mm_load_si128(reinterpret_cast<__m128i*>(addr));

#define STORE_SI128(addr, v) \
    _mm_store_si128(reinterpret_cast<__m128i*>(addr), v);

#define STOREU_SI128(addr, v) \
    _mm_storeu_si128(reinterpret_cast<__m128i*>(addr), v);

#define LOAD_SI128_CONST(addr) \
    _mm_load_si128(reinterpret_cast<const __m128i*>(addr));

namespace ien::image_ops::_internal
{
    const uint32_t trunc_and_table[8] = {
        0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
        0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
    };

    struct vec4x4xf32
    {
        __m128 data[4];
    };

    inline vec4x4xf32 extract_4x4xf32_from_8xu8(__m128i v)
    {
        const __m128i vand_mask = _mm_set1_epi32(0x000000FF);

        __m128i vi32_0 = _mm_and_si128(v, vand_mask);
        __m128i vi32_1 = _mm_and_si128(_mm_srli_si128(v, 1), vand_mask);
        __m128i vi32_2 = _mm_and_si128(_mm_srli_si128(v, 2), vand_mask);
        __m128i vi32_3 = _mm_and_si128(_mm_srli_si128(v, 3), vand_mask);

        return {{
            _mm_cvtepi32_ps(vi32_0),
            _mm_cvtepi32_ps(vi32_1),
            _mm_cvtepi32_ps(vi32_2),
            _mm_cvtepi32_ps(vi32_3)
        }};
    };

    void truncate_channel_data_sse2(const truncate_channel_args& args)
    {
        const size_t img_sz = args.len;
        if (img_sz < SSE_ALIGNMENT)
        {
            truncate_channel_data_std(args);
            return;
        }

        BIND_CHANNELS(args, r, g, b, a);

        const __m128i vmask_r = _mm_set1_epi32(trunc_and_table[args.bits_r]);
        const __m128i vmask_g = _mm_set1_epi32(trunc_and_table[args.bits_g]);
        const __m128i vmask_b = _mm_set1_epi32(trunc_and_table[args.bits_b]);
        const __m128i vmask_a = _mm_set1_epi32(trunc_and_table[args.bits_a]);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
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

    inline __m128i emulate_mm_srli_epi8(__m128i v, int bits)
    {
        const __m128i mask = _mm_set1_epi8(0xFFu >> bits);
        return _mm_and_si128(_mm_srli_epi16(v, bits), mask);
    }

    fixed_vector<uint8_t> rgba_average_sse2(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgba_average_std(args);
        }

        fixed_vector<uint8_t> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        const __m128i vchk_odd = _mm_set1_epi8(0x01);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
        {
            __m128i vseg_r = LOAD_SI128_CONST(r + i);
            __m128i vseg_g = LOAD_SI128_CONST(g + i);
            __m128i vseg_b = LOAD_SI128_CONST(b + i);
            __m128i vseg_a = LOAD_SI128_CONST(a + i);

            __m128i vavg_rg = _mm_avg_epu8(vseg_r, vseg_g);
            __m128i vavg_ba = _mm_avg_epu8(vseg_b, vseg_a);
            __m128i vavg_rgba = _mm_avg_epu8(vavg_rg, vavg_ba);

            // Compensate upwards rounding
            __m128i vcar_rg = _mm_and_si128(_mm_sub_epi8(vseg_r, vseg_g), vchk_odd);
            __m128i vcar_ba = _mm_and_si128(_mm_sub_epi8(vseg_b, vseg_a), vchk_odd);
            __m128i vcar_rgba = _mm_avg_epu8(vcar_rg, vcar_ba);

            vavg_rgba = _mm_sub_epi8(vavg_rgba, vcar_rgba);

            STOREU_SI128((result.data() + i), vavg_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = average<uint8_t>(r[i], g[i], b[i], a[i]);
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_max_sse2(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgba_max_std(args);
        }

        fixed_vector<uint8_t> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
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

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgba_min_std(args);
        }

        fixed_vector<uint8_t> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
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

    fixed_vector<float> rgb_average_sse2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgb_average_std(args);
        }

        fixed_vector<float> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        const __m128i lo_4i_mask_0 = _mm_set1_epi32(0x000000FF);

        const __m128 vmul_div3 = _mm_set1_ps(0.333334F);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
        {
            __m128i vseg_r = LOAD_SI128_CONST(r + i);
            __m128i vseg_g = LOAD_SI128_CONST(g + i);
            __m128i vseg_b = LOAD_SI128_CONST(b + i);

            vec4x4xf32 vlr = extract_4x4xf32_from_8xu8(vseg_r);
            vec4x4xf32 vlg = extract_4x4xf32_from_8xu8(vseg_g);
            vec4x4xf32 vlb = extract_4x4xf32_from_8xu8(vseg_b);

            vlr.data[0] = _mm_add_ps(_mm_add_ps(vlr.data[0], vlg.data[0]), vlb.data[0]);
            vlr.data[1] = _mm_add_ps(_mm_add_ps(vlr.data[1], vlg.data[1]), vlb.data[1]);
            vlr.data[2] = _mm_add_ps(_mm_add_ps(vlr.data[2], vlg.data[2]), vlb.data[2]);
            vlr.data[3] = _mm_add_ps(_mm_add_ps(vlr.data[3], vlg.data[3]), vlb.data[3]);

            vlr.data[0] = _mm_mul_ps(vlr.data[0], vmul_div3);
            vlr.data[1] = _mm_mul_ps(vlr.data[1], vmul_div3);
            vlr.data[2] = _mm_mul_ps(vlr.data[2], vmul_div3);
            vlr.data[3] = _mm_mul_ps(vlr.data[3], vmul_div3);

            _mm_store_ps(result.data() + i + (0 * 4), vlr.data[0]);
            _mm_store_ps(result.data() + i + (1 * 4), vlr.data[1]);
            _mm_store_ps(result.data() + i + (2 * 4), vlr.data[2]);
            _mm_store_ps(result.data() + i + (3 * 4), vlr.data[3]);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = ien::safe_add<float>(r[i], g[i], b[i]) / 3;
        }
        return result;
    }

    fixed_vector<float> rgb_average_sse41(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgb_average_std(args);
        }

        fixed_vector<float> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        struct vec4x4xf32
        {
            __m128 data[4];
        };

        auto extract_4x4xf32_from_16xu8 = [](__m128i v) -> vec4x4xf32
        {
            __m128i vi0 = _mm_cvtepu8_epi32(v);
            __m128i vi1 = _mm_cvtepu8_epi32(_mm_srli_si128(v, 4));
            __m128i vi2 = _mm_cvtepu8_epi32(_mm_srli_si128(v, 8));
            __m128i vi3 = _mm_cvtepu8_epi32(_mm_srli_si128(v, 12));

            return {
                _mm_cvtepi32_ps(vi0),
                _mm_cvtepi32_ps(vi1),
                _mm_cvtepi32_ps(vi2),
                _mm_cvtepi32_ps(vi3)
            };
        };

        const __m128 vmul_div3 = _mm_set1_ps(0.333334F);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
        {
            __m128i vseg_r = LOAD_SI128_CONST(r + i);
            __m128i vseg_g = LOAD_SI128_CONST(g + i);
            __m128i vseg_b = LOAD_SI128_CONST(b + i);

            vec4x4xf32 vlr = extract_4x4xf32_from_16xu8(vseg_r);
            vec4x4xf32 vlg = extract_4x4xf32_from_16xu8(vseg_g);
            vec4x4xf32 vlb = extract_4x4xf32_from_16xu8(vseg_b);

            for (size_t vidx = 0; vidx < 4; ++vidx)
            {
                __m128 vrf = vlr.data[vidx];
                __m128 vgf = vlg.data[vidx];
                __m128 vbf = vlb.data[vidx];

                __m128 sum = _mm_add_ps(_mm_add_ps(vrf, vgf), vbf);
                __m128 avg = _mm_mul_ps(sum, vmul_div3);

                _mm_store_ps(result.data() + i + (vidx * 4), avg);
            }
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = ien::safe_add<float>(r[i], g[i], b[i]) / 3;
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_max_sse2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgb_max_std(args);
        }

        fixed_vector<uint8_t> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
        {
            __m128i vseg_r = LOAD_SI128_CONST(r + i);
            __m128i vseg_g = LOAD_SI128_CONST(g + i);
            __m128i vseg_b = LOAD_SI128_CONST(b + i);

            __m128i vmax_rg = _mm_max_epu8(vseg_r, vseg_g);
            __m128i vmax_rgb = _mm_max_epu8(vmax_rg, vseg_b);

            STOREU_SI128((result.data() + i), vmax_rgb);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::max({ r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_min_sse2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgb_min_std(args);
        }

        fixed_vector<uint8_t> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
        {
            __m128i vseg_r = LOAD_SI128_CONST(r + i);
            __m128i vseg_g = LOAD_SI128_CONST(g + i);
            __m128i vseg_b = LOAD_SI128_CONST(b + i);

            __m128i vmax_rg = _mm_min_epu8(vseg_r, vseg_g);
            __m128i vmax_rgb = _mm_min_epu8(vmax_rg, vseg_b);

            STOREU_SI128((result.data() + i), vmax_rgb);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::min({ r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_sum_saturated_sse2(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgba_sum_saturated_std(args);
        }

        fixed_vector<uint8_t> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
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

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgb_saturation_std(args);
        }

        fixed_vector<float> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        __m128i fpcast_mask = _mm_set1_epi32(0x000000FF);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
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

            alignas(SSE_ALIGNMENT) float aux_result[SSE_ALIGNMENT];

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

        if (img_sz < SSE_ALIGNMENT)
        {
            return rgb_luminance_std(args);
        }

        fixed_vector<float> result(args.len, SSE_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        const __m128 vlum_mul_r = _mm_set1_ps(0.2126F);
        const __m128 vlum_mul_g = _mm_set1_ps(0.7152F);
        const __m128 vlum_mul_b = _mm_set1_ps(0.0722F);
        const __m128 vlum_div_255 = _mm_set1_ps(1.0F / 255);

        size_t last_v_idx = img_sz - (img_sz % SSE_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
        {
            __m128i vseg_r = LOAD_SI128_CONST(r + i);
            __m128i vseg_g = LOAD_SI128_CONST(g + i);
            __m128i vseg_b = LOAD_SI128_CONST(b + i);

            vec4x4xf32 vfr = extract_4x4xf32_from_8xu8(vseg_r);
            vec4x4xf32 vfg = extract_4x4xf32_from_8xu8(vseg_g);
            vec4x4xf32 vfb = extract_4x4xf32_from_8xu8(vseg_b);

            vfr.data[0] = _mm_mul_ps(vfr.data[0], vlum_mul_r);
            vfr.data[1] = _mm_mul_ps(vfr.data[1], vlum_mul_r);
            vfr.data[2] = _mm_mul_ps(vfr.data[2], vlum_mul_r);
            vfr.data[3] = _mm_mul_ps(vfr.data[3], vlum_mul_r);

            vfg.data[0] = _mm_mul_ps(vfg.data[0], vlum_mul_g);
            vfg.data[1] = _mm_mul_ps(vfg.data[1], vlum_mul_g);
            vfg.data[2] = _mm_mul_ps(vfg.data[2], vlum_mul_g);
            vfg.data[3] = _mm_mul_ps(vfg.data[3], vlum_mul_g);

            vfb.data[0] = _mm_mul_ps(vfb.data[0], vlum_mul_b);
            vfb.data[1] = _mm_mul_ps(vfb.data[1], vlum_mul_b);
            vfb.data[2] = _mm_mul_ps(vfb.data[2], vlum_mul_b);
            vfb.data[3] = _mm_mul_ps(vfb.data[3], vlum_mul_b);

            vec4x4xf32 vlum;
            vlum.data[0] = _mm_add_ps(_mm_add_ps(vfr.data[0], vfg.data[0]), vfb.data[0]);
            vlum.data[1] = _mm_add_ps(_mm_add_ps(vfr.data[1], vfg.data[1]), vfb.data[1]);
            vlum.data[2] = _mm_add_ps(_mm_add_ps(vfr.data[2], vfg.data[2]), vfb.data[2]);
            vlum.data[3] = _mm_add_ps(_mm_add_ps(vfr.data[3], vfg.data[3]), vfb.data[3]);

            vlum.data[0] = _mm_mul_ps(vlum.data[0], vlum_div_255);
            vlum.data[1] = _mm_mul_ps(vlum.data[1], vlum_div_255);
            vlum.data[2] = _mm_mul_ps(vlum.data[2], vlum_div_255);
            vlum.data[3] = _mm_mul_ps(vlum.data[3], vlum_div_255);

            _mm_store_ps(result.data() + i + 0, vlum.data[0]);
            _mm_store_ps(result.data() + i + 4, vlum.data[1]);
            _mm_store_ps(result.data() + i + 8, vlum.data[2]);
            _mm_store_ps(result.data() + i + 12, vlum.data[3]);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = (r[i] * 0.2126F / 255) + (g[i] * 0.7152F / 255) + (b[i] * 0.0722F / 255);
        }

        return result;
    }

    image_planar_data unpack_image_data_ssse3(const uint8_t* data, size_t len)
    {
        if (len < (SSE_ALIGNMENT * 4))
        {
            return unpack_image_data_std(data, len);
        }

        image_planar_data result(len / 4);
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

        size_t last_v_idx = len - (len % (SSE_ALIGNMENT * 4));
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT * 4)
        {
            __m128i vdata0 = LOAD_SI128_CONST(data + i + (SSE_ALIGNMENT * 0));
            __m128i vdata1 = LOAD_SI128_CONST(data + i + (SSE_ALIGNMENT * 1));
            __m128i vdata2 = LOAD_SI128_CONST(data + i + (SSE_ALIGNMENT * 2));
            __m128i vdata3 = LOAD_SI128_CONST(data + i + (SSE_ALIGNMENT * 3));

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

    fixed_vector<uint8_t> channel_compare_sse2(const channel_compare_args& args)
    {
        const size_t len = args.len;
        if (len < SSE_ALIGNMENT)
        {
            return channel_compare_std(args);
        }

        fixed_vector<uint8_t> result(len, SSE_ALIGNMENT);

        const __m128i vthreshold = _mm_set1_epi8(args.threshold);

        size_t last_v_idx = len - (len % (SSE_ALIGNMENT));
        for (size_t i = 0; i < last_v_idx; i += SSE_ALIGNMENT)
        {
            __m128i vseg = LOAD_SI128_CONST(args.ch + i);
            __m128i vcmp = _mm_cmpeq_epi8(vseg, _mm_max_epu8(vseg, vthreshold));
            STORE_SI128(result.data() + i, vcmp);
        }

        for (size_t i = last_v_idx; i < len; ++i)
        {
            result[i] = args.ch[i] >= args.threshold;
        }

        return result;
    }
}
#endif