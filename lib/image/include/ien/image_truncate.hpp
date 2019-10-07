#pragma once

#include <cinttypes>

namespace ien::img
{
    struct truncate_args
    {
        uint8_t* image_data;
        int image_width, 
            image_height,
            image_channels,
            bits_r, 
            bits_g,
            bits_b,
            bits_a;
    };

    void truncate_channel_bits(const truncate_args& args);

    namespace _internal
    {
        void truncate_channel_bits_std(uint8_t* data, long size, int r, int g, int b, int a);
        void truncate_channel_bits_sse2(uint8_t* data, long size, int r, int g, int b, int a);
    }
}