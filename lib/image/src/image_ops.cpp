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
            static const int SSE2_STRIDE = 16;
            const size_t img_sz = args.len;
            if(img_sz < SSE2_STRIDE)
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

            for(size_t i = 0; i < img_sz; i += SSE2_STRIDE)
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

            size_t remainder_idx = img_sz - (img_sz % SSE2_STRIDE);
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

    std::vector<uint8_t> channel_average(const image* img)
    {
        typedef std::vector<uint8_t>(*func_ptr_t)(const _internal::channel_average_args&);

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        static func_ptr_t func = 
            platform::x86::get_feature(platform::x86::feature::AVX2)
            ? &_internal::channel_average_avx2
            : platform::x86::get_feature(platform::x86::feature::SSE2)
            ? &_internal::channel_average_sse2
            : &_internal::channel_average_std;

        #else
        static func_ptr_t func = &_internal::chanchannel_average_std;

        #endif

        _internal::channel_average_args args = {
            img->pixel_count(),
            img->cdata()->cdata_r(), 
            img->cdata()->cdata_g(), 
            img->cdata()->cdata_b(), 
            img->cdata()->cdata_a()
        };

        func(args);
    }

    namespace _internal
    {
        std::vector<uint8_t> channel_average_std(const channel_average_args& args)
        {
            const size_t img_sz = args.len;
            
            std::vector<uint8_t> result;
            result.resize(args.len);

            const uint8_t* r = args.ch_r;
            const uint8_t* g = args.ch_g;
            const uint8_t* b = args.ch_b;
            const uint8_t* a = args.ch_a;

            for(size_t i = 0; i < img_sz; ++i)
            {
                uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
                result[i] = static_cast<uint8_t>(sum / 4);
            }
            return result;
        }

        #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)

        std::vector<uint8_t> channel_average_sse2(const channel_average_args& args)
        {
            static const size_t SSE2_STRIDE = 16;
            const size_t img_sz = args.len;

            if(img_sz < SSE2_STRIDE)
            {
                channel_average_std(args);
                return;
            }
            
            std::vector<uint8_t> result;
            result.resize(args.len);
            
            const uint8_t* r = args.ch_r;
            const uint8_t* g = args.ch_g;
            const uint8_t* b = args.ch_b;
            const uint8_t* a = args.ch_a;

            for(size_t i = 0; i < img_sz; i += SSE2_STRIDE)
            {
                __m128i vseg_r = _mm_load_si128(reinterpret_cast<const __m128i*>(r + i));
                __m128i vseg_g = _mm_load_si128(reinterpret_cast<const __m128i*>(g + i));
                __m128i vseg_b = _mm_load_si128(reinterpret_cast<const __m128i*>(b + i));
                __m128i vseg_a = _mm_load_si128(reinterpret_cast<const __m128i*>(a + i));

                __m128i vavg_rg = _mm_avg_epu8(vseg_r, vseg_g);
                __m128i vavg_ba = _mm_avg_epu8(vseg_r, vseg_g);

                __m128i vagv_rgba = _mm_avg_epu8(vavg_rg, vavg_ba);

                _mm_store_si128(reinterpret_cast<__m128i*>(result.data() + i), vagv_rgba);
            }

            size_t remainder_idx = img_sz - (img_sz % SSE2_STRIDE);
            for(size_t i = remainder_idx; i < img_sz; ++i)
            {
                uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
                result[i] = static_cast<uint8_t>(sum / 4);
            }
            return result;
        }

        std::vector<uint8_t> channel_average_avx2(const channel_average_args& args)
        {
            static const size_t AVX2_STRIDE = 32;
            const size_t img_sz = args.len;

            if(img_sz < AVX2_STRIDE)
            {
                channel_average_std(args);
                return;
            }
            
            std::vector<uint8_t> result;
            result.resize(args.len);
            
            const uint8_t* r = args.ch_r;
            const uint8_t* g = args.ch_g;
            const uint8_t* b = args.ch_b;
            const uint8_t* a = args.ch_a;

            for(size_t i = 0; i < img_sz; i += AVX2_STRIDE)
            {
                __m256i vseg_r = _mm256_load_si256(reinterpret_cast<const __m256i*>(r + i));
                __m256i vseg_g = _mm256_load_si256(reinterpret_cast<const __m256i*>(g + i));
                __m256i vseg_b = _mm256_load_si256(reinterpret_cast<const __m256i*>(b + i));
                __m256i vseg_a = _mm256_load_si256(reinterpret_cast<const __m256i*>(a + i));

                __m256i vavg_rg = _mm256_avg_epu8(vseg_r, vseg_g);
                __m256i vavg_ba = _mm256_avg_epu8(vseg_r, vseg_g);

                __m256i vagv_rgba = _mm256_avg_epu8(vavg_rg, vavg_ba);

                _mm256_store_si256(reinterpret_cast<__m256i*>(result.data() + i), vagv_rgba);
            }

            size_t remainder_idx = img_sz - (img_sz % AVX2_STRIDE);
            for(size_t i = remainder_idx; i < img_sz; ++i)
            {
                uint16_t sum = static_cast<uint16_t>(r[i]) + g[i] + b[i] + a[i];
                result[i] = static_cast<uint8_t>(sum / 4);
            }
            return result;
        }

        #endif
    }
}