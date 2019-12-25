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

#define LOAD_SI256(addr) \
    _mm256_load_si256(reinterpret_cast<__m256i*>(addr));

#define STORE_SI256(addr, v) \
    _mm256_store_si256(reinterpret_cast<__m256i*>(addr), v);

    #define STOREU_SI256(addr, v) \
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(addr), v);

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

    fixed_vector<uint8_t> rgba_average_avx2(const channel_info_extract_args_rgba& args)
    {
        return rgba_average_std(args);
        // Not implemented...
    }

    fixed_vector<uint8_t> rgba_max_avx2(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX2_STRIDE)
        {
            return rgba_max_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX2_STRIDE);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

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

            STOREU_SI256((result.data() + i), vmax_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::max({ r[i], g[i], b[i], a[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_min_avx2(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX2_STRIDE)
        {
            return rgba_min_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX2_STRIDE);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

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

            STOREU_SI256((result.data() + i), vmax_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::min({ r[i], g[i], b[i], a[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_average_avx2(const channel_info_extract_args_rgb& args)
    {
        return rgb_average_std(args);
        // Not implemented...
    }

    fixed_vector<uint8_t> rgb_max_avx2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX2_STRIDE)
        {
            return rgb_max_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX2_STRIDE);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
        {
            __m256i vseg_r = LOAD_SI256_CONST(r + i);
            __m256i vseg_g = LOAD_SI256_CONST(g + i);
            __m256i vseg_b = LOAD_SI256_CONST(b + i);

            __m256i vmax_rg = _mm256_max_epu8(vseg_r, vseg_g);
            __m256i vmax_rgb = _mm256_max_epu8(vmax_rg, vseg_b);

            STOREU_SI256((result.data() + i), vmax_rgb);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::max({ r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_min_avx2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX2_STRIDE)
        {
            return rgb_min_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX2_STRIDE);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        size_t last_v_idx = img_sz - (img_sz % AVX2_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
        {
            __m256i vseg_r = LOAD_SI256_CONST(r + i);
            __m256i vseg_g = LOAD_SI256_CONST(g + i);
            __m256i vseg_b = LOAD_SI256_CONST(b + i);

            __m256i vmax_rg = _mm256_min_epu8(vseg_r, vseg_g);
            __m256i vmax_rgb = _mm256_min_epu8(vmax_rg, vseg_b);

            STOREU_SI256((result.data() + i), vmax_rgb);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::min({ r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_sum_saturated_avx2(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX2_STRIDE)
        {
            return rgba_sum_saturated_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX2_STRIDE);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

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

            STOREU_SI256((result.data() + i), vsum_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            uint16_t aux = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
            aux = std::min(static_cast<uint16_t>(0x00FFu), aux);
            result[i] = static_cast<uint8_t>(aux);
        }
        return result;
    }

    fixed_vector<float> rgb_saturation_avx2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX2_STRIDE)
        {
            return rgb_saturation_std(args);
        }

        fixed_vector<float> result(args.len, AVX2_STRIDE);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

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

            alignas(AVX2_STRIDE) float aux_result[AVX2_STRIDE];

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

    fixed_vector<float> rgb_luminance_avx2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX2_STRIDE)
        {
            return rgb_luminance_std(args);
        }

        fixed_vector<float> result(args.len, AVX2_STRIDE);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        __m256i vfpcast_mask = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF);
        __m256 vlum_mulv = _mm256_set1_ps(0.00392156862F / 2);

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

            __m256i vspread_shufmask = _mm256_set_epi8(
                -1,-1,-1,7,-1,-1,-1,6,
                -1,-1,-1,5,-1,-1,-1,4,
                -1,-1,-1,3,-1,-1,-1,2,
                -1,-1,-1,1,-1,-1,-1,0
            );

            // Unpack max bytes into 4 float vectors
            __m256i vmax_0 = _mm256_and_si256(vmax_rgb, vfpcast_mask);
            __m256i vmax_1 = _mm256_and_si256(_mm256_srli_si256(vmax_rgb, 8), vfpcast_mask);

            __m256i vmax_rgb_rev = _mm256_permute2x128_si256(vmax_rgb, vmax_rgb, 0x00000001);

            __m256i vmax_2 = _mm256_and_si256(vmax_rgb_rev, vfpcast_mask);
            __m256i vmax_3 = _mm256_and_si256(_mm256_srli_si256(vmax_rgb_rev, 8), vfpcast_mask);

            __m256i vmax_a0 = _mm256_shuffle_epi8(vmax_0, vspread_shufmask);
            __m256i vmax_a1 = _mm256_shuffle_epi8(vmax_1, vspread_shufmask);
            __m256i vmax_a2 = _mm256_shuffle_epi8(vmax_2, vspread_shufmask);
            __m256i vmax_a3 = _mm256_shuffle_epi8(vmax_3, vspread_shufmask);

            __m256i vmax_a4 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmax_0, vmax_0, 0x00000001), vspread_shufmask);
            __m256i vmax_a5 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmax_1, vmax_1, 0x00000001), vspread_shufmask);
            __m256i vmax_a6 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmax_2, vmax_2, 0x00000001), vspread_shufmask);
            __m256i vmax_a7 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmax_3, vmax_3, 0x00000001), vspread_shufmask);

            vmax_0 = _mm256_or_si256(vmax_a0, vmax_a4);
            vmax_1 = _mm256_or_si256(vmax_a1, vmax_a5);
            vmax_2 = _mm256_or_si256(vmax_a2, vmax_a6);
            vmax_3 = _mm256_or_si256(vmax_a3, vmax_a7);

            __m256 vfmax0 = _mm256_cvtepi32_ps(vmax_0);
            __m256 vfmax1 = _mm256_cvtepi32_ps(vmax_1);
            __m256 vfmax2 = _mm256_cvtepi32_ps(vmax_2);
            __m256 vfmax3 = _mm256_cvtepi32_ps(vmax_3);

            // Unpack min bytes into 4 float vectors
            __m256i vmin_0 = _mm256_and_si256(vmin_rgb, vfpcast_mask);
            __m256i vmin_1 = _mm256_and_si256(_mm256_srli_si256(vmin_rgb, 8), vfpcast_mask);

            __m256i vmin_rgb_rev = _mm256_permute2x128_si256(vmin_rgb, vmin_rgb, 0x00000001);

            __m256i vmin_2 = _mm256_and_si256(vmin_rgb_rev, vfpcast_mask);
            __m256i vmin_3 = _mm256_and_si256(_mm256_srli_si256(vmin_rgb_rev, 8), vfpcast_mask);            

            __m256i vmin_a0 = _mm256_shuffle_epi8(vmin_0, vspread_shufmask);
            __m256i vmin_a1 = _mm256_shuffle_epi8(vmin_1, vspread_shufmask);
            __m256i vmin_a2 = _mm256_shuffle_epi8(vmin_2, vspread_shufmask);
            __m256i vmin_a3 = _mm256_shuffle_epi8(vmin_3, vspread_shufmask);

            __m256i vmin_a4 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmin_0, vmin_0, 0x00000001), vspread_shufmask);
            __m256i vmin_a5 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmin_1, vmin_1, 0x00000001), vspread_shufmask);
            __m256i vmin_a6 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmin_2, vmin_2, 0x00000001), vspread_shufmask);
            __m256i vmin_a7 = _mm256_shuffle_epi8(_mm256_permute2x128_si256(vmin_3, vmin_3, 0x00000001), vspread_shufmask);

            vmin_0 = _mm256_or_si256(vmin_a0, vmin_a4);
            vmin_1 = _mm256_or_si256(vmin_a1, vmin_a5);
            vmin_2 = _mm256_or_si256(vmin_a2, vmin_a6);
            vmin_3 = _mm256_or_si256(vmin_a3, vmin_a7);

            __m256 vfmin0 = _mm256_cvtepi32_ps(vmin_0);
            __m256 vfmin1 = _mm256_cvtepi32_ps(vmin_1);
            __m256 vfmin2 = _mm256_cvtepi32_ps(vmin_2);
            __m256 vfmin3 = _mm256_cvtepi32_ps(vmin_3);

            // Calculate luminance = (max + min) / 2

            __m256 vlum0 = _mm256_mul_ps(_mm256_add_ps(vfmin0, vfmax0), vlum_mulv);
            __m256 vlum1 = _mm256_mul_ps(_mm256_add_ps(vfmin1, vfmax1), vlum_mulv);
            __m256 vlum2 = _mm256_mul_ps(_mm256_add_ps(vfmin2, vfmax2), vlum_mulv);
            __m256 vlum3 = _mm256_mul_ps(_mm256_add_ps(vfmin3, vfmax3), vlum_mulv);

            // Store results

            _mm256_store_ps(result.data() + i + 0, vlum0);
            _mm256_store_ps(result.data() + i + 8, vlum1);
            _mm256_store_ps(result.data() + i + 16, vlum2);
            _mm256_store_ps(result.data() + i + 24, vlum3);

            continue;
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

    fixed_vector<uint8_t> channel_compare_avx2(const channel_compare_args& args)
    {
        const size_t len = args.len;
        if (len < AVX2_STRIDE)
        {
            return channel_compare_std(args);
        }

        fixed_vector<uint8_t> result(len, AVX2_STRIDE);

        const __m256i vthreshold = _mm256_set1_epi8(args.threshold);

        size_t last_v_idx = len - (len % (AVX2_STRIDE));
        for (size_t i = 0; i < last_v_idx; i += AVX2_STRIDE)
        {
            __m256i vseg = LOAD_SI256_CONST(args.ch + i);
            __m256i vcmp = _mm256_cmpeq_epi8(vseg, _mm256_max_epu8(vseg, vthreshold));
            STORE_SI256(result.data() + i, vcmp);
        }

        for (size_t i = last_v_idx; i < len; ++i)
        {
            result[i] = args.ch[i] >= args.threshold;
        }

        return result;
    }
}
#endif