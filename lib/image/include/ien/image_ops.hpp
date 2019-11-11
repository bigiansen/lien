#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/image.hpp>

#include <cinttypes>

namespace ien::img
{
    void truncate_channel_data(image* img, int bits_r, int bits_g, int bits_b, int bits_a);
    fixed_vector<uint8_t> rgba_average(const image* img);
    fixed_vector<uint8_t> rgba_max(const image* img);
    fixed_vector<uint8_t> rgba_min(const image* img);
    fixed_vector<uint8_t> rgba_sum_saturated(const image* img);
    fixed_vector<float> rgb_saturation(const image* img);
    fixed_vector<float> rgb_luminance(const image* img);

	image_unpacked_data unpack_image_data(const uint8_t* data, size_t len);
}