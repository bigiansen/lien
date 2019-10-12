#include <ien/image.hpp>
#include <ien/platform.hpp>

#include <cinttypes>
#include <vector>

namespace ien::img
{
    void truncate_channel_data(image* img, int bits_r, int bits_g, int bits_b, int bits_a);
    std::vector<uint8_t> channel_average(const image* img);
    std::vector<uint8_t> channel_max(const image* img);
}