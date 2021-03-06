#include <ien/internal/std/image_ops_std.hpp>

#include <ien/internal/image_ops_args.hpp>
#include <ien/arithmetic.hpp>

#include <algorithm>
#include <cinttypes>

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
    const uint32_t trunc_and_table[8] = {
        0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
        0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
    };

    const int STD_STRIDE = 4;

    void truncate_channel_data_std(const truncate_channel_args& args)
    {
        const size_t img_sz = args.len;
        BIND_CHANNELS(args, r, g, b, a);

        uint32_t mask_r = trunc_and_table[args.bits_r];
        uint32_t mask_g = trunc_and_table[args.bits_g];
        uint32_t mask_b = trunc_and_table[args.bits_b];
        uint32_t mask_a = trunc_and_table[args.bits_a];

        const size_t last_v_idx = img_sz - (img_sz % STD_STRIDE);
        for(size_t i = 0; i < img_sz; i += STD_STRIDE)
        {
            *(reinterpret_cast<uint32_t*>(r + i)) &= mask_r;
            *(reinterpret_cast<uint32_t*>(g + i)) &= mask_g;
            *(reinterpret_cast<uint32_t*>(b + i)) &= mask_b;
            *(reinterpret_cast<uint32_t*>(a + i)) &= mask_a;
        }

        for(size_t i = last_v_idx; i < img_sz; ++i)
        {
            r[i] &= trunc_and_table[args.bits_r];
            g[i] &= trunc_and_table[args.bits_g];
            b[i] &= trunc_and_table[args.bits_b];
            a[i] &= trunc_and_table[args.bits_a];
        }
    }

    fixed_vector<uint8_t> rgba_average_std(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;
        
        fixed_vector<uint8_t> result(args.len);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        for(size_t i = 0; i < img_sz; ++i)
        {
            result[i] = average<uint8_t>(r[i], g[i], b[i], a[i]);
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_max_std(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        fixed_vector<uint8_t> result(args.len);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        for (size_t i = 0; i < img_sz; ++i)
        {
            // std::max({a,b,c,d}) results in worse assembly with clang and msvc (GCC generates identical code)
            uint8_t max = r[i];            
            if (max < g[i]) { max = g[i]; }
            if (max < b[i]) { max = b[i]; }
            if (max < a[i]) { max = a[i]; }

            result[i] = max;
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_min_std(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        fixed_vector<uint8_t> result(args.len);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        for (size_t i = 0; i < img_sz; ++i)
        {
            result[i] = std::min({ r[i], g[i], b[i], a[i] });
        }
        return result;
    }

    fixed_vector<float> rgb_average_std(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;
        
        fixed_vector<float> result(args.len);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        for(size_t i = 0; i < img_sz; ++i)
        {
            float sum = safe_add<float>(r[i], g[i], b[i]);
            result[i] = sum / 3;
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_max_std(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;
        fixed_vector<uint8_t> result(args.len);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        for (size_t i = 0; i < img_sz; ++i)
        {
            result[i] = std::max({ r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgb_min_std(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;
        fixed_vector<uint8_t> result(args.len);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        for (size_t i = 0; i < img_sz; ++i)
        {
            result[i] = std::min({ r[i], g[i], b[i] });
        }
        return result;
    }

    fixed_vector<uint8_t> rgba_sum_saturated_std(const channel_info_extract_args_rgba& args)
    {
        const size_t img_sz = args.len;

        fixed_vector<uint8_t> result(args.len);

        BIND_CHANNELS_RGBA_CONST(args, r, g, b, a);

        for (size_t i = 0; i < img_sz; ++i)
        {
            uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
            result[i] = static_cast<uint8_t>(std::min(static_cast<uint16_t>(0x00FFu), sum));
        }
        return result;
    }

    fixed_vector<float> rgb_saturation_std(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        fixed_vector<float> result(args.len);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        // SATURATION(r, g, b) = (MAX(r, g, b) - MIN(r, g, b)) / MAX(r, g, b)
        for (size_t i = 0; i < img_sz; ++i)
        {
            float vmax = static_cast<float>(std::max({ r[i], g[i], b[i] })) / 255.0F;
            float vmin = static_cast<float>(std::min({ r[i], g[i], b[i] })) / 255.0F;
            result[i] = (vmax - vmin) / vmax;
        }

        return result;
    }

    fixed_vector<float> rgb_luminance_std(const channel_info_extract_args_rgb& args)
    {
        const size_t img_sz = args.len;

        fixed_vector<float> result(args.len);

        BIND_CHANNELS_RGB_CONST(args, r, g, b);

        for (size_t i = 0; i < img_sz; ++i)
        {
            result[i] = (r[i] * 0.2126F / 255) + (g[i] * 0.7152F / 255) + (b[i] * 0.0722F / 255);
        }

        return result;
    }

	image_planar_data unpack_image_data_std(const uint8_t* data, size_t len)
	{
		image_planar_data result(len / 4);

		uint8_t* r = result.data_r();
		uint8_t* g = result.data_g();
		uint8_t* b = result.data_b();
		uint8_t* a = result.data_a();

		for (size_t i = 0; i < len; i += 4)
		{
			r[i / 4] = data[i + 0];
			g[i / 4] = data[i + 1];
			b[i / 4] = data[i + 2];
			a[i / 4] = data[i + 3];
		}
		return result;
	}

    fixed_vector<uint8_t> channel_compare_std(const channel_compare_args& args)
    {
        fixed_vector<uint8_t> result(args.len, LIEN_DEFAULT_ALIGNMENT);
        for(size_t i = 0; i < args.len; ++i)
        {
            result[i] = args.ch[i] >= args.threshold;
        }
        return result;
    }
}