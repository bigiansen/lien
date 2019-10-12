#include <ien/platform.hpp>

#include <ien/internal/image_ops_args.hpp>
#include <cinttypes>
#include <vector>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)

namespace ien::img::_internal
{
    void truncate_channel_data_sse2(const truncate_channel_args& args);
    void truncate_channel_data_avx2(const truncate_channel_args& args);

    std::vector<uint8_t> channel_average_sse2(const channel_average_args& args);
    std::vector<uint8_t> channel_average_avx2(const channel_average_args& args);

    std::vector<uint8_t> channel_max_sse2(const channel_max_args& args);
    std::vector<uint8_t> channel_max_avx2(const channel_max_args& args);
}

#endif