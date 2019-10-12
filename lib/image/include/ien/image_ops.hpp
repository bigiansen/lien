#include <ien/image.hpp>
#include <ien/platform.hpp>

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
}