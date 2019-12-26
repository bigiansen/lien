#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/image.hpp>
#include <ien/rgba_channel.hpp>

#include <cinttypes>

namespace ien::image_ops
{
    void truncate_channel_data(image_unpacked_data* img, int bits_r, int bits_g, int bits_b, int bits_a);

    fixed_vector<uint8_t> rgba_average(const image& img);

    fixed_vector<uint8_t> rgba_max(const image& img);

    fixed_vector<uint8_t> rgba_min(const image& img);

    fixed_vector<uint8_t> rgb_average(const image& img);

    fixed_vector<uint8_t> rgb_max(const image& img);

    fixed_vector<uint8_t> rgb_min(const image& img);

    fixed_vector<uint8_t> rgba_sum_saturated(const image& img);

    fixed_vector<float> rgb_saturation(const image& img);

    fixed_vector<float> rgb_luminance(const image& img);

    image_unpacked_data unpack_image_data(const uint8_t* data, size_t len);

    fixed_vector<uint8_t> channel_compare(const image& img, rgba_channel channel, uint8_t threshold);
}