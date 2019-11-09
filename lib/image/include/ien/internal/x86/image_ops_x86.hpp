#pragma once

#include <ien/fixed_vector.hpp>
#include <ien/platform.hpp>
#include <ien/internal/image_ops_args.hpp>
#include <cinttypes>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)

namespace ien::img::_internal
{
    void truncate_channel_data_sse2(const truncate_channel_args& args);
    void truncate_channel_data_avx2(const truncate_channel_args& args);

    fixed_vector<uint8_t> rgba_average_sse2(const channel_info_extract_args_rgba& args);
    fixed_vector<uint8_t> rgba_average_avx2(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgba_max_sse2(const channel_info_extract_args_rgba& args);
    fixed_vector<uint8_t> rgba_max_avx2(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgba_min_sse2(const channel_info_extract_args_rgba& args);
    fixed_vector<uint8_t> rgba_min_avx2(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgba_sum_saturated_sse2(const channel_info_extract_args_rgba& args);
    fixed_vector<uint8_t> rgba_sum_saturated_avx2(const channel_info_extract_args_rgba& args);

    fixed_vector<float> rgb_saturation_sse2(const channel_info_extract_args_rgb& args);
    fixed_vector<float> rgb_saturation_avx2(const channel_info_extract_args_rgb& args);

    fixed_vector<float> rgb_luminance_sse2(const channel_info_extract_args_rgb& args);
    fixed_vector<float> rgb_luminance_avx2(const channel_info_extract_args_rgb& args);

	image_unpacked_data unpack_image_data_ssse3(const uint8_t* data, size_t len);
}

#endif