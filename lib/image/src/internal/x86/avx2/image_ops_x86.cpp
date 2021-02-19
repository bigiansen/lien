#include <ien/internal/x86/image_ops_x86.hpp>

#include <ien/platform.hpp>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
#include <ien/internal/std/image_ops_std.hpp>
#include <ien/internal/image_ops_args.hpp>

#include <ien/arithmetic.hpp>
#include <algorithm>
#include <immintrin.h>

#define AVX_ALIGNMENT 32

#define DEBUG_ASSERT_RGBA_ALIGNED(r, g, b, a) \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(r, AVX_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(g, AVX_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(b, AVX_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(a, AVX_ALIGNMENT))

#define DEBUG_ASSERT_RGB_ALIGNED(r, g, b) \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(r, AVX_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(g, AVX_ALIGNMENT)); \
    LIEN_DEBUG_ASSERT(ien::is_ptr_aligned(b, AVX_ALIGNMENT))    

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

#define LOAD_SI256(addr) \
    _mm256_load_si256(reinterpret_cast<__m256i*>(addr));

#define STORE_SI256(addr, v) \
    _mm256_store_si256(reinterpret_cast<__m256i*>(addr), v);

    #define STOREU_SI256(addr, v) \
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(addr), v);

#define LOAD_SI256_CONST(addr) \
    _mm256_load_si256(reinterpret_cast<const __m256i*>(addr));

namespace ien::image_ops::_internal
{
    const uint32_t trunc_and_table[8] = {
        0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
        0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
    };

    struct vec4x8xf32
    {
        __m256 data[4];
    };

    vec4x8xf32 extract_4x8xf32_from_16xu8(__m256i v)
    {
        __m128i vlh = _mm256_extracti128_si256(v, 0);
        __m128i vhh = _mm256_extracti128_si256(v, 1);

        __m256i v0i = _mm256_cvtepu8_epi32(vlh);
        __m256i v1i = _mm256_cvtepu8_epi32(_mm_srli_si128(vlh, 8));
        __m256i v2i = _mm256_cvtepu8_epi32(vhh);
        __m256i v3i = _mm256_cvtepu8_epi32(_mm_srli_si128(vhh, 8));

        return {{
            _mm256_cvtepi32_ps(v0i),
            _mm256_cvtepi32_ps(v1i),
            _mm256_cvtepi32_ps(v2i),
            _mm256_cvtepi32_ps(v3i)
        }};
    };

    void truncate_channel_data_avx2(const truncate_channel_args& args)
    {
        const size_t img_sz = args.len;
        if (img_sz < AVX_ALIGNMENT)
        {
            truncate_channel_data_std(args);
            return;
        }

        BIND_CHANNELS(args, r, g, b, a);

        const __m256i vmask_r = _mm256_set1_epi8(trunc_and_table[args.bits_r]);
        const __m256i vmask_g = _mm256_set1_epi8(trunc_and_table[args.bits_g]);
        const __m256i vmask_b = _mm256_set1_epi8(trunc_and_table[args.bits_b]);
        const __m256i vmask_a = _mm256_set1_epi8(trunc_and_table[args.bits_a]);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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
        return rgba_average_sse2(args);
        // Not implemented...
    }

    fixed_vector<uint8_t> rgba_max_avx2(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX_ALIGNMENT)
        {
            return rgba_max_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX_ALIGNMENT);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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

        if (img_sz < AVX_ALIGNMENT)
        {
            return rgba_min_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX_ALIGNMENT);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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

    fixed_vector<float> rgb_average_avx2(const channel_info_extract_args_rgb& args)
    {        
        const size_t img_sz = args.len;
        
        if (img_sz < AVX_ALIGNMENT)
        {
            return rgb_average_std(args);
        }
        
        fixed_vector<float> result(args.len, AVX_ALIGNMENT);
        
        BIND_CHANNELS_RGB_CONST(args, r, g, b);        
        
        const __m256 vmul_div3 = _mm256_set1_ps(0.333334F);
        
        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
        {
            __m256i vseg_r = LOAD_SI256_CONST(r + i);
            __m256i vseg_g = LOAD_SI256_CONST(g + i);
            __m256i vseg_b = LOAD_SI256_CONST(b + i);
        
            vec4x8xf32 vlr = extract_4x8xf32_from_16xu8(vseg_r);
            vec4x8xf32 vlg = extract_4x8xf32_from_16xu8(vseg_g);
            vec4x8xf32 vlb = extract_4x8xf32_from_16xu8(vseg_b);
        
            for (size_t vidx = 0; vidx < 4; ++vidx)
            {
                __m256 vrf = vlr.data[vidx];
                __m256 vgf = vlg.data[vidx];
                __m256 vbf = vlb.data[vidx];
                   
                __m256 sum = _mm256_add_ps(_mm256_add_ps(vrf, vgf), vbf);
                __m256 avg = _mm256_mul_ps(sum, vmul_div3);
        
                _mm256_store_ps(result.data() + i + (vidx * 8), avg);
            }
        }
        
        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = ien::safe_add<float>(r[i], g[i], b[i]) / 3;
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_max_avx2(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        if (img_sz < AVX_ALIGNMENT)
        {
            return rgb_max_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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

        if (img_sz < AVX_ALIGNMENT)
        {
            return rgb_min_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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

        if (img_sz < AVX_ALIGNMENT)
        {
            return rgba_sum_saturated_std(args);
        }

        fixed_vector<uint8_t> result(args.len, AVX_ALIGNMENT);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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

        if (img_sz < AVX_ALIGNMENT)
        {
            return rgb_saturation_std(args);
        }

        fixed_vector<float> result(args.len, AVX_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        __m256i fpcast_mask = _mm256_set1_epi32(0x000000FF);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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

            alignas(AVX_ALIGNMENT) float aux_result[AVX_ALIGNMENT];

            _mm256_store_ps(aux_result + 0, vsat0);
            _mm256_store_ps(aux_result + 8, vsat1);
            _mm256_store_ps(aux_result + 16, vsat2);
            _mm256_store_ps(aux_result + 24, vsat3);

            for (size_t k = 0; k < 8; ++k)
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

        if (img_sz < AVX_ALIGNMENT)
        {
            return rgb_luminance_std(args);
        }

        fixed_vector<float> result(args.len, AVX_ALIGNMENT);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        size_t last_v_idx = img_sz - (img_sz % AVX_ALIGNMENT);

        const __m256 vlum_mul_r = _mm256_set1_ps(0.2126F);
        const __m256 vlum_mul_g = _mm256_set1_ps(0.7152F);
        const __m256 vlum_mul_b = _mm256_set1_ps(0.0722F);
        const __m256 vlum_div_255 = _mm256_set1_ps(1.0F / 255);

        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
        {
            __m256i vseg_r = LOAD_SI256_CONST(r + i);
            __m256i vseg_g = LOAD_SI256_CONST(g + i);
            __m256i vseg_b = LOAD_SI256_CONST(b + i);

            vec4x8xf32 vfr = extract_4x8xf32_from_16xu8(vseg_r);
            vec4x8xf32 vfg = extract_4x8xf32_from_16xu8(vseg_g);
            vec4x8xf32 vfb = extract_4x8xf32_from_16xu8(vseg_b);

            vfr.data[0] = _mm256_mul_ps(vfr.data[0], vlum_mul_r);
            vfr.data[1] = _mm256_mul_ps(vfr.data[1], vlum_mul_r);
            vfr.data[2] = _mm256_mul_ps(vfr.data[2], vlum_mul_r);
            vfr.data[3] = _mm256_mul_ps(vfr.data[3], vlum_mul_r);

            vfg.data[0] = _mm256_mul_ps(vfg.data[0], vlum_mul_g);
            vfg.data[1] = _mm256_mul_ps(vfg.data[1], vlum_mul_g);
            vfg.data[2] = _mm256_mul_ps(vfg.data[2], vlum_mul_g);
            vfg.data[3] = _mm256_mul_ps(vfg.data[3], vlum_mul_g);

            vfb.data[0] = _mm256_mul_ps(vfb.data[0], vlum_mul_b);
            vfb.data[1] = _mm256_mul_ps(vfb.data[1], vlum_mul_b);
            vfb.data[2] = _mm256_mul_ps(vfb.data[2], vlum_mul_b);
            vfb.data[3] = _mm256_mul_ps(vfb.data[3], vlum_mul_b);

            vec4x8xf32 vlum;
            vlum.data[0] = _mm256_add_ps(_mm256_add_ps(vfr.data[0], vfg.data[0]), vfb.data[0]);
            vlum.data[1] = _mm256_add_ps(_mm256_add_ps(vfr.data[1], vfg.data[1]), vfb.data[1]);
            vlum.data[2] = _mm256_add_ps(_mm256_add_ps(vfr.data[2], vfg.data[2]), vfb.data[2]);
            vlum.data[3] = _mm256_add_ps(_mm256_add_ps(vfr.data[3], vfg.data[3]), vfb.data[3]);

            vlum.data[0] = _mm256_mul_ps(vlum.data[0], vlum_div_255);
            vlum.data[1] = _mm256_mul_ps(vlum.data[1], vlum_div_255);
            vlum.data[2] = _mm256_mul_ps(vlum.data[2], vlum_div_255);
            vlum.data[3] = _mm256_mul_ps(vlum.data[3], vlum_div_255);

            _mm256_store_ps(result.data() + i + 0, vlum.data[0]);
            _mm256_store_ps(result.data() + i + 8, vlum.data[1]);
            _mm256_store_ps(result.data() + i + 16, vlum.data[2]);
            _mm256_store_ps(result.data() + i + 24, vlum.data[3]);

            continue;
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {            
            result[i] = (r[i] * 0.2126F / 255) + (g[i] * 0.7152F / 255) + (b[i] * 0.0722F / 255);
        }

        return result;
    }

    image_planar_data unpack_image_data_avx2(const uint8_t* data, size_t len)
    {
        if (len < (AVX_ALIGNMENT * 4))
        {
            return unpack_image_data_ssse3(data, len);
        }

        image_planar_data result(len / 4);
        uint8_t* r = result.data_r();
        uint8_t* g = result.data_g();
        uint8_t* b = result.data_b();
        uint8_t* a = result.data_a();

        const __m256i vshufmask = _mm256_set_epi8(
            19, 15, 11, 7,
            18, 14, 10, 6,
            17, 13, 9, 5,
            16, 12, 8, 4,
            15, 11, 7, 3,
            14, 10, 6, 2, 
            13, 9, 5, 1,
            12, 8, 4, 0
        );

        size_t last_v_idx = len - (len % (AVX_ALIGNMENT * 4));
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT * 4)
        {
            __m256i vdata0 = LOAD_SI256_CONST(data + i + (AVX_ALIGNMENT * 0));
            __m256i vdata1 = LOAD_SI256_CONST(data + i + (AVX_ALIGNMENT * 1));
            __m256i vdata2 = LOAD_SI256_CONST(data + i + (AVX_ALIGNMENT * 2));
            __m256i vdata3 = LOAD_SI256_CONST(data + i + (AVX_ALIGNMENT * 3));

            __m256i v_di_data0 = _mm256_shuffle_epi8(vdata0, vshufmask);
            __m256i v_di_data1 = _mm256_shuffle_epi8(vdata1, vshufmask);
            __m256i v_di_data2 = _mm256_shuffle_epi8(vdata2, vshufmask);
            __m256i v_di_data3 = _mm256_shuffle_epi8(vdata3, vshufmask);
                                    
            __m256i v_r0r1g0g1 = _mm256_unpacklo_epi32(v_di_data0, v_di_data1);
            __m256i v_r2r3g2g3 = _mm256_unpacklo_epi32(v_di_data2, v_di_data3);
            __m256i v_r0r1r2r3 = _mm256_unpacklo_epi64(v_r0r1g0g1, v_r2r3g2g3);
            __m256i v_g0g1g2g3 = _mm256_unpackhi_epi64(v_r0r1g0g1, v_r2r3g2g3);
                                    
            __m256i v_b0b1a0a1 = _mm256_unpackhi_epi32(v_di_data0, v_di_data1);
            __m256i v_b2b3a2a3 = _mm256_unpackhi_epi32(v_di_data2, v_di_data3);
            __m256i v_b0b1b2b3 = _mm256_unpacklo_epi64(v_b0b1a0a1, v_b2b3a2a3);
            __m256i v_a0a1a2a3 = _mm256_unpackhi_epi64(v_b0b1a0a1, v_b2b3a2a3);

            STORE_SI256(r + (i / 4), v_r0r1r2r3);
            STORE_SI256(g + (i / 4), v_g0g1g2g3);
            STORE_SI256(b + (i / 4), v_b0b1b2b3);
            STORE_SI256(a + (i / 4), v_a0a1a2a3);
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

    fixed_vector<uint8_t> channel_compare_avx2(const channel_compare_args& args)
    {
        const size_t len = args.len;
        if (len < AVX_ALIGNMENT)
        {
            return channel_compare_std(args);
        }

        fixed_vector<uint8_t> result(len, AVX_ALIGNMENT);

        const __m256i vthreshold = _mm256_set1_epi8(args.threshold);

        size_t last_v_idx = len - (len % (AVX_ALIGNMENT));
        for (size_t i = 0; i < last_v_idx; i += AVX_ALIGNMENT)
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