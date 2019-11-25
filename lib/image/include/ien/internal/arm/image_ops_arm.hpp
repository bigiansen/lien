#include <ien/platform.hpp>

#if defined(LIEN_ARM_NEON)

#include <ien/fixed_vector.hpp>
#include <ien/internal/image_ops_args.hpp>

namespace ien::img::_internal
{
    void truncate_channel_data_neon(const truncate_channel_args& args);

    fixed_vector<uint8_t> rgba_average_neon(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgba_max_neon(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgba_min_neon(const channel_info_extract_args_rgba& args);

    fixed_vector<uint8_t> rgb_max_neon(const channel_info_extract_args_rgb& args);

    fixed_vector<uint8_t> rgb_min_neon(const channel_info_extract_args_rgb& args);

    fixed_vector<uint8_t> rgba_sum_saturated_neon(const channel_info_extract_args_rgba& args);

    fixed_vector<float> rgb_saturation_neon(const channel_info_extract_args_rgb& args);

    fixed_vector<float> rgb_luminance_neon(const channel_info_extract_args_rgb& args);

	image_unpacked_data unpack_image_data_neon(const uint8_t* data, size_t len);
}

#endif
