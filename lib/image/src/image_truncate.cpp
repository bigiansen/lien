#include <ien/image_truncate.hpp>

#include <ien/platform.hpp>

namespace ien::img
{
    void truncate_channel_bits(const truncate_args& args)
    {
    #if defined(LIEN_ARCH_X86_64)
        static void (*fptr)(uint8_t*, long, int, int, int, int) =
            &_internal::truncate_channel_bits_sse2;
    #elif defined(LIEN_ARCH_X86)
        static void (*fptr)(uint8_t*, long, int, int, int, int) =
            ien::platform::x86::get_feature(ien::platform::x86::feature::SSE2)
            ? &_internal::truncate_channel_bits_sse2
            : &_internal::truncate_channel_bits_std;
    #else
        static void (*fptr)(uint8_t*, long, int, int, int, int) =
            &_internal::truncate_channel_bits_std;
    #endif

        if (args.bits_r || args.bits_g || args.bits_b || args.bits_a)
        {
            auto img_datalen = (args.image_width * args.image_height * args.image_channels);
            fptr(args.image_data, img_datalen, args.bits_r, args.bits_g, args.bits_b, args.bits_a);
        }
    }

    namespace _internal
    {
        void truncate_channel_bits_std(uint8_t* data, long size, int r, int g, int b, int a)
        {
            if (size % 4 != 0)
            {
                throw std::invalid_argument("Specified size must be a multiple of 4");
            }

            uint32_t trunc_mask = 0;
            trunc_mask |= (1 << r) << 24;
            trunc_mask |= (1 << g) << 16;
            trunc_mask |= (1 << b) << 8;
            trunc_mask |= (1 << a);

            for (long i = 0; i < size; i += 4)
            {
                uint32_t* iptr = reinterpret_cast<uint32_t*>(data);
                *iptr &= trunc_mask;
            }
        }

        #if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
        #include <immintrin.h>
        void truncate_channel_bits_sse2(uint8_t* data, long size, int r, int g, int b, int a)
        {
            if (size % 4 != 0)
            {
                throw std::invalid_argument("Specified size must be a multiple of 4");
            }

            uint8_t trunc_mask[16] = {
                (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
                (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
                (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
                (uint8_t)(1 << r), (uint8_t)(1 << g), (uint8_t)(1 << b), (uint8_t)(1 << a),
            };

            __m128i vtrunc_mask = _mm_loadu_si32(trunc_mask);
            for (long i = 0; i < size; i += 16)
            {
                __m128i vdata = _mm_loadu_si32(data + i);
                vdata = _mm_and_si128(vdata, vtrunc_mask);
                _mm_store_si128(reinterpret_cast<__m128i*>(data + i), vdata);
            }
        }
        #endif
    }
}