#include <ien/internal/arm/neon/image_ops_neon.hpp>
#include <ien/platform.hpp>

#if defined(LIEN_ARM_NEON)

#include <ien/assert.hpp>
#include <ien/internal/image_ops_args.hpp>
#include <ien/internal/std/image_ops_std.hpp>
#include <algorithm>
#include <arm_neon.h>

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

namespace ien::image_ops::_internal
{
    constexpr size_t NEON_STRIDE = 16;

    float32x4x4_t extract_4x4f32_from_8x16u8(uint8x16_t v)
    {                                                                           // v = v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15
        uint8x8_t hv_lo = vget_low_u8(v);                                       // v0, v1, v2, v3, v4, v5, v6, v7
        uint8x8_t hv_hi = vget_high_u8(v);                                      // v8, v9, v10, v11, v12, v13, v14, v15

        uint8x16_t vtemp_lo = vreinterpretq_u8_u16(vmovl_u8(hv_lo));            // 0, v0, 0, v1, 0, v2, 0, v3, 0, v4, 0, v5, 0, v6, 0, v7
        uint8x8_t hvtemp_lo0 = vget_low_u8(vtemp_lo);                           // 0, v0, 0, v1, 0, v2, 0, v3
        uint8x8_t hvtemp_lo1 = vget_high_u8(vtemp_lo);                          // 0, v4, 0, v5, 0, v6, 0, v7

        uint32x4_t vtemp32_lo0 = vmovl_u16(vreinterpret_u16_u8(hvtemp_lo0));    // 0, 0, 0, v0, 0, 0, 0, v1, 0, 0, 0, v2, 0, 0, 0, v3
        uint32x4_t vtemp32_lo1 = vmovl_u16(vreinterpret_u16_u8(hvtemp_lo1));    // 0, 0, 0, v4, 0, 0, 0, v5, 0, 0, 0, v6, 0, 0, 0, v7

        float32x4_t vfv0 = vcvtq_f32_u32(vtemp32_lo0);                          // (float) vf0, vf1, vf2, vf3
        float32x4_t vfv1 = vcvtq_f32_u32(vtemp32_lo1);                          // (float) vf4, vf5, vf6, vf7

        uint8x16_t vtemp_hi = vreinterpretq_u8_u16(vmovl_u8(hv_hi));            // same for high uint8x8
        uint8x8_t hvtemp_hi0 = vget_low_u8(vtemp_hi);
        uint8x8_t hvtemp_hi1 = vget_high_u8(vtemp_hi);

        uint32x4_t vtemp32_hi0 = vmovl_u16(vreinterpret_u16_u8(hvtemp_hi0)); 
        uint32x4_t vtemp32_hi1 = vmovl_u16(vreinterpret_u16_u8(hvtemp_hi1));

        float32x4_t vfv2 = vcvtq_f32_u32(vtemp32_hi0);
        float32x4_t vfv3 = vcvtq_f32_u32(vtemp32_hi1);

        return {{ vfv0, vfv1, vfv2, vfv3 }};
    }

    float32x4_t neon_divide_f32_fast(float32x4_t dividend, float32x4_t divisor)
    {
        float32x4_t reciprocal = vrecpeq_f32(divisor);
        return vmulq_f32(dividend, reciprocal);
    }

    float32x4_t neon_divide_f32(float32x4_t dividend, float32x4_t divisor, uint8_t newton_rhaphson_steps = 1)
    {
        float32x4_t reciprocal = vrecpeq_f32(divisor);
        for(uint8_t i = 0; i < newton_rhaphson_steps; ++i)
        {
            reciprocal = vmulq_f32(vrecpsq_f32(divisor, reciprocal), reciprocal);
        }
        return vmulq_f32(dividend, reciprocal);
    }

    void truncate_channel_data_neon(const truncate_channel_args& args)
    {
        const uint8_t trunc_and_table[8] = {
            0xFF, 0xFE, 0xFC, 0xF8,
            0xF0, 0xE0, 0xC0, 0x80
        };

        const size_t img_sz = args.len;

        if(img_sz < NEON_STRIDE)
        {
            truncate_channel_data_std(args);
        }

        BIND_CHANNELS(args, r, g, b, a);

        const uint8x16_t vmask_r = vld1q_dup_u8(&trunc_and_table[args.bits_r]);
        const uint8x16_t vmask_g = vld1q_dup_u8(&trunc_and_table[args.bits_g]);
        const uint8x16_t vmask_b = vld1q_dup_u8(&trunc_and_table[args.bits_b]);
        const uint8x16_t vmask_a = vld1q_dup_u8(&trunc_and_table[args.bits_a]);

        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);
            uint8x16_t vseg_a = vld1q_u8(a + i);

            vseg_r = vandq_u8(vseg_r, vmask_r);
            vseg_g = vandq_u8(vseg_g, vmask_g);
            vseg_b = vandq_u8(vseg_b, vmask_b);
            vseg_a = vandq_u8(vseg_a, vmask_a);

            vst1q_u8(r + i, vseg_r);
            vst1q_u8(g + i, vseg_g);
            vst1q_u8(b + i, vseg_b);
            vst1q_u8(a + i, vseg_a);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            r[i] &= trunc_and_table[args.bits_r];
            g[i] &= trunc_and_table[args.bits_g];
            b[i] &= trunc_and_table[args.bits_b];
            a[i] &= trunc_and_table[args.bits_a];
        }
    }

    fixed_vector<uint8_t> rgba_average_neon(const channel_info_extract_args_rgba& args)
    {
        return rgba_average_std(args);
        // Not implemented...
    }

    fixed_vector<uint8_t> rgba_max_neon(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            return rgba_max_std(args);
        }
        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        fixed_vector<uint8_t> result(args.len, NEON_STRIDE);
        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);
            uint8x16_t vseg_a = vld1q_u8(a + i);

            uint8x16_t vmax_rg = vmaxq_u8(vseg_r, vseg_g);
            uint8x16_t vmax_ba = vmaxq_u8(vseg_b, vseg_a);
            uint8x16_t vmax_rgba = vmaxq_u8(vmax_rg, vmax_ba);

            vst1q_u8(result.data() + i, vmax_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::max({r[i], g[i], b[i], a[i]});
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_min_neon(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            return rgba_min_std(args);
        }
        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        fixed_vector<uint8_t> result(args.len, NEON_STRIDE);
        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);
            uint8x16_t vseg_a = vld1q_u8(a + i);

            uint8x16_t vmin_rg = vminq_u8(vseg_r, vseg_g);
            uint8x16_t vmin_ba = vminq_u8(vseg_b, vseg_a);
            uint8x16_t vmin_rgba = vminq_u8(vmin_rg, vmin_ba);

            vst1q_u8(result.data() + i, vmin_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::min({r[i], g[i], b[i], a[i]});
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_average_neon(const channel_info_extract_args_rgb& args)
    {
        return rgb_average_std(args);
        // Not implemented...
    }

    fixed_vector<uint8_t> rgb_max_neon(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            return rgb_max_std(args);
        }
        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        fixed_vector<uint8_t> result(args.len, NEON_STRIDE);
        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);

            uint8x16_t vmax_rg = vmaxq_u8(vseg_r, vseg_g);
            uint8x16_t vmax_rgb = vmaxq_u8(vseg_b, vseg_b);

            vst1q_u8(result.data() + i, vmax_rgb);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::max({ r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_min_neon(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            return rgb_min_std(args);
        }
        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        fixed_vector<uint8_t> result(args.len, NEON_STRIDE);
        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);

            uint8x16_t vmin_rg = vminq_u8(vseg_r, vseg_g);
            uint8x16_t vmin_rgb = vminq_u8(vmin_rg, vseg_b);

            vst1q_u8(result.data() + i, vmin_rgb);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            result[i] = std::min({r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_sum_saturated_neon(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            return rgba_sum_saturated_std(args);
        }
        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        fixed_vector<uint8_t> result(args.len, NEON_STRIDE);
        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);
            uint8x16_t vseg_a = vld1q_u8(a + i);

            uint8x16_t vsum_rg = vqaddq_u8(vseg_r, vseg_g);
            uint8x16_t vsum_ba = vqaddq_u8(vseg_b, vseg_a);
            uint8x16_t vsum_rgba = vqaddq_u8(vsum_rg, vsum_ba);

            vst1q_u8(result.data() + i, vsum_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
            result[i] = static_cast<uint8_t>(std::min(static_cast<uint16_t>(255), sum));
        }
        return result;
    }    

    fixed_vector<float> rgb_saturation_neon(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            return rgb_saturation_std(args);
        }
        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        fixed_vector<float> result(args.len, NEON_STRIDE);

        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);

            uint8x16_t vmax_rg = vmaxq_u8(vseg_r, vseg_g);
            uint8x16_t vmax_rgb = vmaxq_u8(vmax_rg, vseg_b);

            uint8x16_t vmin_rg = vminq_u8(vseg_r, vseg_g);
            uint8x16_t vmin_rgb = vminq_u8(vmin_rg, vseg_b);

            uint8x16_t vdiff_rgb = vsubq_u8(vmax_rgb, vmin_rgb);

            float32x4x4_t vfdiffq = extract_4x4f32_from_8x16u8(vdiff_rgb);
            float32x4x4_t vfmaxq = extract_4x4f32_from_8x16u8(vmax_rgb);

            for(int vidx = 0; vidx < 4; ++vidx)
            {
                float32x4_t vfsat = neon_divide_f32(vfdiffq.val[vidx], vfmaxq.val[vidx], 2);
                float* dest_ptr = result.data() + i + (vidx * 4);
                vst1q_f32(dest_ptr, vfsat);
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

    fixed_vector<float> rgb_luminance_neon(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            return rgb_luminance_std(args);
        }
        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        fixed_vector<float> result(args.len, NEON_STRIDE);

        const float lum_mul = 0.0019607843137254F;
        float32x4_t vlum_mul = vld1q_dup_f32(&lum_mul);

        size_t last_v_idx = img_sz - (img_sz % NEON_STRIDE);
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg_r = vld1q_u8(r + i);
            uint8x16_t vseg_g = vld1q_u8(g + i);
            uint8x16_t vseg_b = vld1q_u8(b + i);

            uint8x16_t vmax_rg = vmaxq_u8(vseg_r, vseg_g);
            uint8x16_t vmax_rgb = vmaxq_u8(vmax_rg, vseg_b);

            uint8x16_t vmin_rg = vminq_u8(vseg_r, vseg_g);
            uint8x16_t vmin_rgb = vminq_u8(vmin_rg, vseg_b);

            float32x4x4_t vmaxq = extract_4x4f32_from_8x16u8(vmax_rgb);
            float32x4x4_t vminq = extract_4x4f32_from_8x16u8(vmin_rgb);

            float32x4x4_t vsumq = {
                vaddq_f32(vmaxq.val[0], vminq.val[0]),
                vaddq_f32(vmaxq.val[1], vminq.val[1]),
                vaddq_f32(vmaxq.val[2], vminq.val[2]),
                vaddq_f32(vmaxq.val[3], vminq.val[3])
            };

            for(int vidx = 0; vidx < 4; ++vidx)
            {
                float32x4_t vflum = vmulq_f32(vsumq.val[vidx], vlum_mul);
                float* dest_ptr = result.data() + i + (vidx * 4);
                vst1q_f32(dest_ptr, vflum);
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

    image_unpacked_data unpack_image_data_neon(const uint8_t* data, size_t len)
    {
        if(len < NEON_STRIDE)
        {
            return unpack_image_data_std(data, len);
        }

        image_unpacked_data result(len / 4);

        uint8_t* r = result.data_r();
        uint8_t* g = result.data_g();
        uint8_t* b = result.data_b();
        uint8_t* a = result.data_a();

        size_t last_v_idx = len - (len % NEON_STRIDE);
        for(size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16x4_t vrgba = vld4q_u8(data + i);

            size_t vidx = i / 4;
            vst1q_u8(r + (vidx), vrgba.val[0]);
            vst1q_u8(g + (vidx), vrgba.val[1]);
            vst1q_u8(b + (vidx), vrgba.val[2]);
            vst1q_u8(a + (vidx), vrgba.val[3]);
        }

        for (size_t i = last_v_idx; i < len; ++i)
        {
            size_t vidx = i / 4;
            r[vidx] = data[i + 0];
            g[vidx] = data[i + 1];
            b[vidx] = data[i + 2];
            a[vidx] = data[i + 3];
        }

        return result;
    }

    fixed_vector<uint8_t> channel_compare_neon(const channel_compare_args& args)
    {
        const size_t len = args.len;
        if (len < NEON_STRIDE)
        {
            return channel_compare_std(args);
        }

        fixed_vector<uint8_t> result(len, NEON_STRIDE);

        const uint8x16_t vthreshold = vld1q_dup_u8(&args.threshold);

        size_t last_v_idx = len - (len % (NEON_STRIDE));
        for (size_t i = 0; i < last_v_idx; i += NEON_STRIDE)
        {
            uint8x16_t vseg = vld1q_u8(args.ch + i);
            uint8x16_t vcmp = vcgeq_u8(vseg, vthreshold);
            vst1q_u8(result.data() + i, vcmp);
        }

        for (size_t i = last_v_idx; i < len; ++i)
        {
            result[i] = args.ch[i] >= args.threshold;
        }

        return result;
    }
}

#endif
