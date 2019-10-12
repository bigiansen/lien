#include <ien/image.hpp>
#include <ien/platform.hpp>

#include <cinttypes>
#include <vector>

namespace ien::img
{
    void truncate_channel_data(image* img, int bits_r, int bits_g, int bits_b, int bits_a);
    namespace _internal
    {
        struct truncate_channel_args
        {
            size_t len;
            uint8_t* ch_r;
            uint8_t* ch_g;
            uint8_t* ch_b;
            uint8_t* ch_a;
            int bits_r;
            int bits_g; 
            int bits_b;
            int bits_a;
        };

        void truncate_channel_data_std(const truncate_channel_args& args);
        
        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        void truncate_channel_data_sse2(const truncate_channel_args& args);
        void truncate_channel_data_avx2(const truncate_channel_args& args);

        #endif
    }

    std::vector<uint8_t> channel_average(const image* img);
    namespace _internal
    {
        struct channel_average_args
        {
            size_t len;
            const uint8_t* ch_r;
            const uint8_t* ch_g;
            const uint8_t* ch_b;
            const uint8_t* ch_a;
        };

        std::vector<uint8_t> channel_average_std(const channel_average_args& args);

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        std::vector<uint8_t> channel_average_sse2(const channel_average_args& args);
        std::vector<uint8_t> channel_average_avx2(const channel_average_args& args);

        #endif
    }
}