#pragma once

#include <cinttypes>

namespace ien::img::_internal
{
    struct truncate_channel_args
    {
        size_t len;
        uint8_t* ch_r;
        uint8_t* ch_g;
        uint8_t* ch_b;
        uint8_t* ch_a;
        int bits_r;
        int bits_g; 
        int bits_b;
        int bits_a;
    };

	struct channel_info_extract_args
	{
		size_t len;
		const uint8_t* ch_r;
		const uint8_t* ch_g;
		const uint8_t* ch_b;
		const uint8_t* ch_a;
	};
}