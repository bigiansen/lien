#pragma once

#include <ien/internal/image_ops_args.hpp>

#include <cinttypes>
#include <vector>

namespace ien::img::_internal
{
    void truncate_channel_data_std(const truncate_channel_args& args);

    std::vector<uint8_t> rgba_average_std(const channel_info_extract_args& args);
    std::vector<uint8_t> rgba_max_std(const channel_info_extract_args& args);
	std::vector<uint8_t> rgba_sum_saturated_std(const channel_info_extract_args& args);
}