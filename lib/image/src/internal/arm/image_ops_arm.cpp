#include <ien/internal/arm/image_ops_arm.hpp>
#include <ien/platform.hpp>

#if defined(LIEN_ARM_NEON)

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

namespace ien::img::_internal
{
    constexpr size_t NEON_STRIDE = 16;

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
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            rgba_average_std(args);
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

            uint8x16_t vavg_rg = vrhaddq_u8(vseg_r, vseg_g);
            uint8x16_t vavg_ba = vrhaddq_u8(vseg_b, vseg_a);
            uint8x16_t vavg_rgba = vrhaddq_u8(vavg_rg, vavg_ba);

            vst1q_u8(result.data() + i, vavg_rgba);
        }

        for (size_t i = last_v_idx; i < img_sz; ++i)
        {
            uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
            result[i] = static_cast<uint8_t>(sum / 4);
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_max_neon(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            rgba_max_std(args);
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
            rgba_average_std(args);
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

    fixed_vector<uint8_t> rgba_sum_saturated_neon(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;
        if(img_sz < NEON_STRIDE)
        {
            rgba_average_std(args);
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

            uint8x16_t vsum_rg = vqadd_u8(vseg_r, vseg_g);
            uint8x16_t vsum_ba = vqadd_u8(vseg_b, vseg_a);
            uint8x16_t vsum_rgba = vqadd_u8(vsum_rg, vsum_ba);

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
        return fixed_vector<float>(0);
    }

    fixed_vector<float> rgb_luminance_neon(const channel_info_extract_args_rgb& args)
    {
        return fixed_vector<float>(0);
    }

    image_unpacked_data unpack_image_data_neon(const uint8_t* data, size_t len)
    {
        image_unpacked_data result(len / 4);

        uint8_t* r = result.data_r();
        uint8_t* g = result.data_g();
        uint8_t* b = result.data_b();
        uint8_t* a = result.data_a();

        size_t last_v_idx = len - (len % NEON_STRIDE);
        for(size_t i = 0; i < len; i += NEON_STRIDE)
        {
            uint8x16_t vr = vld4q_u8(data + i + 0);
            uint8x16_t vg = vld4q_u8(data + i + 1);
            uint8x16_t vb = vld4q_u8(data + i + 2);
            uint8x16_t va = vld4q_u8(data + i + 3);

            vst1q_u8(r + (i / 4), vr);
            vst1q_u8(g + (i / 4), vg);
            vst1q_u8(b + (i / 4), vb);
            vst1q_u8(a + (i / 4), va);
        }
    }
}

#endif
