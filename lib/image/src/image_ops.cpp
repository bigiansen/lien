#include <ien/image_ops.hpp>

#include <ien/platform.hpp>

namespace ien::img
{
    void truncate_channel_data(image_unpacked_data* img, int bits_r, int bits_g, int bits_b, int bits_a)
    {
        typedef void(*func_ptr_t)(const _internal::truncate_channel_args& args);
        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        static func_ptr_t func = 
            platform::x86::get_feature(platform::x86::feature::AVX2)
            ? &_internal::truncate_channel_data_avx2
            : platform::x86::get_feature(platform::x86::feature::SSE2)
            ? &_internal::truncate_channel_data_sse2
            : &_internal::truncate_channel_data_std;

        #else
        static func_ptr_t func = &_internal::truncate_channel_data_std;

        #endif

        _internal::truncate_channel_args args = { 
            img->size(),
            img->data_r(), img->data_g(), img->data_b(), img->data_a(),
            bits_r, bits_g, bits_b, bits_a
        };

        func(args);
    }

    namespace _internal
    {
        const uint32_t trunc_and_table[8] = {
            0xFFFFFFFF, 0xFEFEFEFE, 0xFCFCFCFC, 0xF8F8F8F8,
            0xF0F0F0F0, 0xE0E0E0E0, 0xC0C0C0C0, 0x80808080
        };

        void truncate_channel_data_std(const truncate_channel_args& args)
        {
            size_t img_sz = args.len;
            uint8_t* r = args.ch_r;
            uint8_t* g = args.ch_g;
            uint8_t* b = args.ch_b;
            uint8_t* a = args.ch_a;

            uint32_t mask_r = trunc_and_table[args.bits_r];
            uint32_t mask_g = trunc_and_table[args.bits_g];
            uint32_t mask_b = trunc_and_table[args.bits_b];
            uint32_t mask_a = trunc_and_table[args.bits_a];

            for(size_t i = 0; i < img_sz; i += 4)
            {
                *(reinterpret_cast<uint32_t*>(r + i)) &= mask_r;
                *(reinterpret_cast<uint32_t*>(g + i)) &= mask_g;
                *(reinterpret_cast<uint32_t*>(b + i)) &= mask_b;
                *(reinterpret_cast<uint32_t*>(a + i)) &= mask_a;
            }

            size_t remainder_idx = img_sz - (img_sz % 16);
            for(size_t i = remainder_idx; i < img_sz; ++i)
            {
                r[i] &= trunc_and_table[args.bits_r];
                g[i] &= trunc_and_table[args.bits_g];
                b[i] &= trunc_and_table[args.bits_b];
                a[i] &= trunc_and_table[args.bits_a];
            }
        }

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        #include <immintrin.h>

        void truncate_channel_data_sse2(const truncate_channel_args& args)
        {
            static const int SSE2_VECSZ = 16;
            const size_t img_sz = args.len;
            if(img_sz < SSE2_VECSZ)
            {
                truncate_channel_data_std(args);
                return;
            }

            uint8_t* r = args.ch_r;
            uint8_t* g = args.ch_g;
            uint8_t* b = args.ch_b;
            uint8_t* a = args.ch_a;

            const __m128i vmask_r = _mm_set1_epi32(trunc_and_table[args.bits_r]);
            const __m128i vmask_g = _mm_set1_epi32(trunc_and_table[args.bits_g]);
            const __m128i vmask_b = _mm_set1_epi32(trunc_and_table[args.bits_b]);
            const __m128i vmask_a = _mm_set1_epi32(trunc_and_table[args.bits_a]);

            for(size_t i = 0; i < img_sz; i += 16)
            {
                __m128i seg_r = _mm_load_si128(reinterpret_cast<__m128i*>(r + i));
                __m128i seg_g = _mm_load_si128(reinterpret_cast<__m128i*>(g + i));
                __m128i seg_b = _mm_load_si128(reinterpret_cast<__m128i*>(b + i));
                __m128i seg_a = _mm_load_si128(reinterpret_cast<__m128i*>(a + i));

                seg_r = _mm_and_si128(seg_r, vmask_r);
                seg_g = _mm_and_si128(seg_g, vmask_g);
                seg_b = _mm_and_si128(seg_b, vmask_b);
                seg_a = _mm_and_si128(seg_a, vmask_a);

                _mm_store_si128(reinterpret_cast<__m128i*>(r + i), seg_r);
                _mm_store_si128(reinterpret_cast<__m128i*>(g + i), seg_g);
                _mm_store_si128(reinterpret_cast<__m128i*>(b + i), seg_b);
                _mm_store_si128(reinterpret_cast<__m128i*>(a + i), seg_a);
            }

            size_t remainder_idx = img_sz - (img_sz % 16);
            for(size_t i = remainder_idx; i < img_sz; ++i)
            {
                r[i] &= trunc_and_table[args.bits_r];
                g[i] &= trunc_and_table[args.bits_g];
                b[i] &= trunc_and_table[args.bits_b];
                a[i] &= trunc_and_table[args.bits_a];
            }
        }

        void truncate_channel_data_avx2(const truncate_channel_args& args)
        {
            static const int AVX2_STRIDE = 32;
            const size_t img_sz = args.len;
            if(img_sz < AVX2_STRIDE)
            {
                truncate_channel_data_std(args);
                return;
            }

            uint8_t* r = args.ch_r;
            uint8_t* g = args.ch_g;
            uint8_t* b = args.ch_b;
            uint8_t* a = args.ch_a;

            const __m256i vmask_r = _mm256_set1_epi8(trunc_and_table[args.bits_r]);
            const __m256i vmask_g = _mm256_set1_epi8(trunc_and_table[args.bits_g]);
            const __m256i vmask_b = _mm256_set1_epi8(trunc_and_table[args.bits_b]);
            const __m256i vmask_a = _mm256_set1_epi8(trunc_and_table[args.bits_a]);

            for(size_t i = 0; i < img_sz; i += AVX2_STRIDE)
            {
                __m256i seg_r = _mm256_load_si256(reinterpret_cast<__m256i*>(r + i));
                __m256i seg_g = _mm256_load_si256(reinterpret_cast<__m256i*>(g + i));
                __m256i seg_b = _mm256_load_si256(reinterpret_cast<__m256i*>(b + i));
                __m256i seg_a = _mm256_load_si256(reinterpret_cast<__m256i*>(a + i));

                seg_r = _mm256_and_si256(seg_r, vmask_r);
                seg_g = _mm256_and_si256(seg_g, vmask_g);
                seg_b = _mm256_and_si256(seg_b, vmask_b);
                seg_a = _mm256_and_si256(seg_a, vmask_a);

                _mm256_store_si256(reinterpret_cast<__m256i*>(r + i), seg_r);
                _mm256_store_si256(reinterpret_cast<__m256i*>(g + i), seg_g);
                _mm256_store_si256(reinterpret_cast<__m256i*>(b + i), seg_b);
                _mm256_store_si256(reinterpret_cast<__m256i*>(a + i), seg_a);
            }

            size_t remainder_idx = img_sz - (img_sz % AVX2_STRIDE);
            for(size_t i = remainder_idx; i < img_sz; ++i)
            {
                r[i] &= trunc_and_table[args.bits_r];
                g[i] &= trunc_and_table[args.bits_g];
                b[i] &= trunc_and_table[args.bits_b];
                a[i] &= trunc_and_table[args.bits_a];
            }
        }

        #endif
    }
}