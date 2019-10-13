#include <ien/internal/std/image_ops_std.hpp>

#include <ien/internal/image_ops_args.hpp>
#include <algorithm>
#include <cinttypes>

namespace ien::img::_internal
{
    const uint32_t trunc_and_table[8] = {
        0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
        0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
    };

    void truncate_channel_data_std(const truncate_channel_args& args)
    {
        size_t img_sz = args.len;
        uint8_t* r = args.ch_r;
        uint8_t* g = args.ch_g;
        uint8_t* b = args.ch_b;
        uint8_t* a = args.ch_a;

        uint32_t mask_r = trunc_and_table[args.bits_r];
        uint32_t mask_g = trunc_and_table[args.bits_g];
        uint32_t mask_b = trunc_and_table[args.bits_b];
        uint32_t mask_a = trunc_and_table[args.bits_a];

        for(size_t i = 0; i < img_sz; i += 4)
        {
            *(reinterpret_cast<uint32_t*>(r + i)) &= mask_r;
            *(reinterpret_cast<uint32_t*>(g + i)) &= mask_g;
            *(reinterpret_cast<uint32_t*>(b + i)) &= mask_b;
            *(reinterpret_cast<uint32_t*>(a + i)) &= mask_a;
        }

        size_t remainder_idx = img_sz - (img_sz % 16);
        for(size_t i = remainder_idx; i < img_sz; ++i)
        {
            r[i] &= trunc_and_table[args.bits_r];
            g[i] &= trunc_and_table[args.bits_g];
            b[i] &= trunc_and_table[args.bits_b];
            a[i] &= trunc_and_table[args.bits_a];
        }
    }

    std::vector<uint8_t> rgba_average_std(const channel_info_extract_args& args)
    {
        const size_t img_sz = args.len;
        
        std::vector<uint8_t> result;
        result.resize(args.len);

        const uint8_t* r = args.ch_r;
        const uint8_t* g = args.ch_g;
        const uint8_t* b = args.ch_b;
        const uint8_t* a = args.ch_a;

        for(size_t i = 0; i < img_sz; ++i)
        {
            uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
            result[i] = static_cast<uint8_t>(sum / 4);
        }
        return result;
    }

	std::vector<uint8_t> rgba_max_std(const channel_info_extract_args& args)
	{
		const size_t img_sz = args.len;

		std::vector<uint8_t> result;
		result.resize(args.len);

		const uint8_t* r = args.ch_r;
		const uint8_t* g = args.ch_g;
		const uint8_t* b = args.ch_b;
		const uint8_t* a = args.ch_a;

		for (size_t i = 0; i < img_sz; ++i)
		{
			result[i] = std::max({ r[i], g[i], b[i], a[i] });
		}
		return result;
	}

	std::vector<uint8_t> rgba_sum_saturated_std(const channel_info_extract_args& args)
	{
		const size_t img_sz = args.len;

		std::vector<uint8_t> result;
		result.resize(args.len);

		const uint8_t* r = args.ch_r;
		const uint8_t* g = args.ch_g;
		const uint8_t* b = args.ch_b;
		const uint8_t* a = args.ch_a;

		for (size_t i = 0; i < img_sz; ++i)
		{
			uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
			result[i] = static_cast<uint8_t>(std::min(0x00FFui16, sum));
		}
		return result;
	}
}