#pragma once

#include <cinttypes>
#include <vector>

namespace ien::img
{
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // TRUNCATE
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
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
        void truncate_channel_bits_std(uint8_t* data, size_t size, int r, int g, int b, int a);
        void truncate_channel_bits_sse2(uint8_t* data, size_t size, int r, int g, int b, int a);
    }

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // MAX CHANNEL RGBA
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    std::vector<uint8_t> max_channel_rgba(const uint8_t* data, size_t len);
    namespace _internal
    {
        std::vector<uint8_t> max_channel_rgba_std(const uint8_t* data, size_t len);
        std::vector<uint8_t> max_channel_rgba_sse2(const uint8_t* data, size_t len);
        std::vector<uint8_t> max_channel_rgba_ssse3(const uint8_t* data, size_t len);
    }

    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // VISUAL DATA
    //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    std::vector<uint8_t> channel_red(const uint8_t* data, size_t len);
    std::vector<uint8_t> channel_green(const uint8_t* data, size_t len);
    std::vector<uint8_t> channel_blue(const uint8_t* data, size_t len);
    std::vector<uint8_t> channel_alpha(const uint8_t* data, size_t len);
    namespace _internal
    {
        std::vector<uint8_t> get_channel_std(const uint8_t* data, size_t len, size_t ch_idx);
    }
}