#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/internal/image_ops_args.hpp>

#include <cinttypes>

namespace ien::image_ops::_internal
{
    void truncate_channel_data_std(const truncate_channel_args& args);

    fixed_vector<float> rgba_average_std(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgba_max_std(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgba_min_std(const channel_info_extract_args_rgba& args);

    fixed_vector<float> rgb_average_std(const channel_info_extract_args_rgb& args);

    fixed_vector<uint8_t> rgb_max_std(const channel_info_extract_args_rgb& args);

    fixed_vector<uint8_t> rgb_min_std(const channel_info_extract_args_rgb& args);

    fixed_vector<uint8_t> rgba_sum_saturated_std(const channel_info_extract_args_rgba& args);
    
    fixed_vector<float> rgb_saturation_std(const channel_info_extract_args_rgb& args);
    
    fixed_vector<float> rgb_luminance_std(const channel_info_extract_args_rgb& args);

    image_planar_data unpack_image_data_std(const uint8_t* data, size_t len);

    fixed_vector<uint8_t> channel_compare_std(const channel_compare_args& args);
}