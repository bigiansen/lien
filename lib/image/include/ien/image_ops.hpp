#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/planar_image.hpp>
#include <ien/rgba_channel.hpp>

#include <cinttypes>

namespace ien::image_ops
{
    void truncate_channel_data(image_planar_data* img, int bits_r, int bits_g, int bits_b, int bits_a);

    fixed_vector<uint8_t> rgba_average(const planar_image& img);

    fixed_vector<uint8_t> rgba_max(const planar_image& img);

    fixed_vector<uint8_t> rgba_min(const planar_image& img);

    fixed_vector<float> rgb_average(const planar_image& img);

    fixed_vector<uint8_t> rgb_max(const planar_image& img);

    fixed_vector<uint8_t> rgb_min(const planar_image& img);

    fixed_vector<uint8_t> rgba_sum_saturated(const planar_image& img);

    fixed_vector<float> rgb_saturation(const planar_image& img);

    fixed_vector<float> rgb_luminance(const planar_image& img);

    image_planar_data unpack_image_data(const uint8_t* data, size_t len);

    fixed_vector<uint8_t> channel_compare(const planar_image& img, rgba_channel channel, uint8_t threshold);
}