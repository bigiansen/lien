#pragma once

#include <ien/internal/image_ops_args.hpp>

#include <cinttypes>
#include <vector>

namespace ien::img::_internal
{
    void truncate_channel_data_std(const truncate_channel_args& args);

    std::vector<uint8_t> rgba_average_std(const channel_info_extract_args_rgba& args);
    std::vector<uint8_t> rgba_max_std(const channel_info_extract_args_rgba& args);
    std::vector<uint8_t> rgba_min_std(const channel_info_extract_args_rgba& args);
    std::vector<uint8_t> rgba_sum_saturated_std(const channel_info_extract_args_rgba& args);
    
    std::vector<float> rgb_saturation_std(const channel_info_extract_args_rgb& args);
    std::vector<float> rgb_luminance_std(const channel_info_extract_args_rgb& args);
}