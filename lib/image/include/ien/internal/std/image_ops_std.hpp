#pragma once

#include <ien/internal/image_ops_args.hpp>

#include <cinttypes>
#include <vector>

namespace ien::img::_internal
{
    void truncate_channel_data_std(const truncate_channel_args& args);
    std::vector<uint8_t> channel_average_std(const channel_average_args& args);
    std::vector<uint8_t> channel_max_std(const channel_max_args& args);
}