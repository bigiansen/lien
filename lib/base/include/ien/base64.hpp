#include <cinttypes>
#include <cstddef>
#include <string>
#include <vector>

namespace ien::base64
{
    std::string encode(const uint8_t* src, size_t len);
    std::vector<uint8_t> decode(const uint8_t* data, size_t len);
}