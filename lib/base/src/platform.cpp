#include <ien/platform.hpp>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)

#if defined(LIEN_COMPILER_MSVC)
    #include <intrin.h>
#endif

#include <array>
#include <iostream>
#include <memory>
#include <unordered_map>

#ifndef LIEN_COMPILER_MSVC
    // https://github.com/01org/linux-sgx/blob/master/common/inc/internal/linux/cpuid_gnu.h
    void __cpuidex(int cpuid[4], int func_id, int subfunc_id)
    {
    #if defined(LIEN_ARCH_X86_64)
        asm volatile ("cpuid"
                : "=a" (cpuid[0]), "=b" (cpuid[1]), "=c" (cpuid[2]), "=d" (cpuid[3])
                : "0" (func_id), "2" (subfunc_id));
    #elif defined(LIEN_ARCH_X86)
        asm volatile ("xchgl %%ebx, %1; cpuid; xchgl %%ebx, %1"
                : "=a" (cpuid[0]), "=r" (cpuid[1]), "=c" (cpuid[2]), "=d" (cpuid[3])
                : "0" (func_id), "2" (subfunc_id));
    #else
        #error "Unsupported MSVC platform"
    #endif
    }
#endif

namespace ien
{
    template<typename TEnumType, size_t MAX_VALUE> 
    class feature_map
    {
    private:
        std::array<bool, MAX_VALUE + 1> _map;

    public:
        bool& operator[](TEnumType idx)
        { return _map[static_cast<size_t>(idx)]; }

        bool& operator[](size_t idx) 
        { return _map[idx]; }
    };
}

namespace ien::platform::x86
{
    void init();
    void print_enabled_features(std::ostream& ostr);
    bool get_feature(feature f);

    typedef std::unordered_map<int, std::string> feature_names_map_t;

    feature_names_map_t gen_names_map()
    {
        feature_names_map_t result;
        result.emplace((int)feature::ABM,         "ABM        " );
        result.emplace((int)feature::ADX,         "ADX        " );
        result.emplace((int)feature::AES,         "AES        " );
        result.emplace((int)feature::AVX2,        "AVX2       " );
        result.emplace((int)feature::AVX512BW,    "AVX512BW   " );
        result.emplace((int)feature::AVX512CD,    "AVX512CD   " );
        result.emplace((int)feature::AVX512DQ,    "AVX512DQ   " );
        result.emplace((int)feature::AVX512ER,    "AVX512ER   " );
        result.emplace((int)feature::AVX512F,     "AVX512F    " );
        result.emplace((int)feature::AVX512IFMA,  "AVX512IFMA " );
        result.emplace((int)feature::AVX512PF,    "AVX512PF   " );
        result.emplace((int)feature::AVX512VBMI,  "AVX512VBMI " );
        result.emplace((int)feature::AVX512VL,    "AVX512VL   " );
        result.emplace((int)feature::AVX,         "AVX        " );
        result.emplace((int)feature::BMI1,        "BMI1       " );
        result.emplace((int)feature::BMI2,        "BMI2       " );
        result.emplace((int)feature::FMA3,        "FMA3       " );
        result.emplace((int)feature::FMA4,        "FMA4       " );
        result.emplace((int)feature::MMX,         "MMX        " );
        result.emplace((int)feature::PREFETCHWT1, "PREFETCHWT1" );
        result.emplace((int)feature::RDRAND,      "RDRAND     " );
        result.emplace((int)feature::SHA,         "SHA        " );
        result.emplace((int)feature::SSE2,        "SSE2       " );
        result.emplace((int)feature::SSE3,        "SSE3       " );
        result.emplace((int)feature::SSE41,       "SSE41      " );
        result.emplace((int)feature::SSE42,       "SSE42      " );
        result.emplace((int)feature::SSE4a,       "SSE4a      " );
        result.emplace((int)feature::SSE,         "SSE        " );
        result.emplace((int)feature::SSSE3,       "SSSE3      " );
        result.emplace((int)feature::x64,         "x64        " );
        result.emplace((int)feature::XOP,         "XOP        " );
        return result; 
    }

    feature_map<feature, 30> map;
    std::unique_ptr<feature_names_map_t> names;

    void W32_CPUID(int* info, int x)
    { 
        __cpuidex(info, x, 0);
    }

    bool initialized = false;
    void init()
    {
        names = std::make_unique<feature_names_map_t>(gen_names_map());

        int info[4];
        W32_CPUID(info, 0);
        int nIds = info[0];

        W32_CPUID(info, 0x80000000);
        unsigned nExIds = info[0];

        //  Detect Features
        if (nIds >= 0x00000001)
        {
            W32_CPUID(info, 0x00000001);
            map[feature::MMX]    = (info[3] & (1 << 23)) != 0;
            map[feature::SSE]    = (info[3] & (1 << 25)) != 0;
            map[feature::SSE2]   = (info[3] & (1 << 26)) != 0;
            map[feature::SSE3]   = (info[2] & (1 <<  0)) != 0;
            map[feature::SSSE3]  = (info[2] & (1 <<  9)) != 0;
            map[feature::SSE41]  = (info[2] & (1 << 19)) != 0;
            map[feature::SSE42]  = (info[2] & (1 << 20)) != 0;
            map[feature::AES]    = (info[2] & (1 << 25)) != 0;
            map[feature::AVX]    = (info[2] & (1 << 28)) != 0;
            map[feature::FMA3]   = (info[2] & (1 << 12)) != 0;
            map[feature::RDRAND] = (info[2] & (1 << 30)) != 0;
        }
        if (nIds >= 0x00000007)
        {
            W32_CPUID(info,0x00000007);
            map[feature::AVX2]        = (info[1] & (1 <<  5)) != 0;
            map[feature::BMI1]        = (info[1] & (1 <<  3)) != 0;
            map[feature::BMI2]        = (info[1] & (1 <<  8)) != 0;
            map[feature::ADX]         = (info[1] & (1 << 19)) != 0;
            map[feature::SHA]         = (info[1] & (1 << 29)) != 0;
            map[feature::PREFETCHWT1] = (info[2] & (1 <<  0)) != 0;
            map[feature::AVX512F]     = (info[1] & (1 << 16)) != 0;
            map[feature::AVX512CD]    = (info[1] & (1 << 28)) != 0;
            map[feature::AVX512PF]    = (info[1] & (1 << 26)) != 0;
            map[feature::AVX512ER]    = (info[1] & (1 << 27)) != 0;
            map[feature::AVX512VL]    = (info[1] & (1 << 31)) != 0;
            map[feature::AVX512BW]    = (info[1] & (1 << 30)) != 0;
            map[feature::AVX512DQ]    = (info[1] & (1 << 17)) != 0;
            map[feature::AVX512IFMA]  = (info[1] & (1 << 21)) != 0;
            map[feature::AVX512VBMI]  = (info[2] & (1 <<  1)) != 0;
        }
        if (nExIds >= 0x80000001)
        {
            W32_CPUID(info,0x80000001);
            map[feature::x64]   = (info[3] & (1 << 29)) != 0;
            map[feature::ABM]   = (info[2] & (1 <<  5)) != 0;
            map[feature::SSE4a] = (info[2] & (1 <<  6)) != 0;
            map[feature::FMA4]  = (info[2] & (1 << 16)) != 0;
            map[feature::XOP]   = (info[2] & (1 << 11)) != 0;
        }
    }

    bool get_feature(feature f)
    {
        if(!initialized)
        {
            init();
            initialized = true;
        }
        return map[f];
    }

    void force_feature(feature f, bool enabled)
    {
        if(!initialized)
        {
            init();
            initialized = true;
        }
        map[f] = enabled;
    }
}

#endif