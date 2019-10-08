#include <ien/platform.hpp>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)

#if __has_include(<intrin.h>)
    #include <intrin.h>
#elif __has_include(<x86intrin.h>)
    #include <x86intrin.h>
#else
    #error "Could not find intrinsics header for CPUID!"
#endif

#include <iostream>
#include <unordered_map>

namespace ien::platform::x86
{
    void init();
    void print_enabled_features(std::ostream& ostr);
    bool get_feature(feature f);

    typedef std::unordered_map<int, bool> feature_map_t;
    typedef std::unordered_map<int, std::string> feature_names_map_t;

    feature_map_t gen_feature_map()
    {
        feature_map_t result;
        result.emplace((int)feature::ABM,         false);
        result.emplace((int)feature::ADX,         false);
        result.emplace((int)feature::AES,         false);
        result.emplace((int)feature::AVX2,        false);
        result.emplace((int)feature::AVX512BW,    false);
        result.emplace((int)feature::AVX512CD,    false);
        result.emplace((int)feature::AVX512DQ,    false);
        result.emplace((int)feature::AVX512ER,    false);
        result.emplace((int)feature::AVX512F,     false);
        result.emplace((int)feature::AVX512IFMA,  false);
        result.emplace((int)feature::AVX512PF,    false);
        result.emplace((int)feature::AVX512VBMI,  false);
        result.emplace((int)feature::AVX512VL,    false);
        result.emplace((int)feature::AVX,         false);
        result.emplace((int)feature::BMI1,        false);
        result.emplace((int)feature::BMI2,        false);
        result.emplace((int)feature::FMA3,        false);
        result.emplace((int)feature::FMA4,        false);
        result.emplace((int)feature::MMX,         false);
        result.emplace((int)feature::PREFETCHWT1, false);
        result.emplace((int)feature::RDRAND,      false);
        result.emplace((int)feature::SHA,         false);
        result.emplace((int)feature::SSE2,        false);
        result.emplace((int)feature::SSE3,        false);
        result.emplace((int)feature::SSE41,       false);
        result.emplace((int)feature::SSE42,       false);
        result.emplace((int)feature::SSE4a,       false);
        result.emplace((int)feature::SSE,         false);
        result.emplace((int)feature::SSSE3,       false);
        result.emplace((int)feature::x64,         false);
        result.emplace((int)feature::XOP,         false);
        return result;
    }

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

    std::unique_ptr<feature_map_t> feature_map;
    std::unique_ptr<feature_names_map_t> feature_names;

    void W32_CPUID(int* info, int x)
    { 
        __cpuidex(info, x, 0);
    }

    bool initialized = false;
    void init()
    {
        feature_map = std::make_unique<feature_map_t>(gen_feature_map());
        feature_names = std::make_unique<feature_names_map_t>(gen_names_map());

        int info[4];
        W32_CPUID(info, 0);
        int nIds = info[0];

        W32_CPUID(info, 0x80000000);
        unsigned nExIds = info[0];

        //  Detect Features
        if (nIds >= 0x00000001)
        {
            W32_CPUID(info, 0x00000001);
            (*feature_map)[(int)feature::MMX]    = (info[3] & (1 << 23)) != 0;
            (*feature_map)[(int)feature::SSE]    = (info[3] & (1 << 25)) != 0;
            (*feature_map)[(int)feature::SSE2]   = (info[3] & (1 << 26)) != 0;
            (*feature_map)[(int)feature::SSE3]   = (info[2] & (1 <<  0)) != 0;
            (*feature_map)[(int)feature::SSSE3]  = (info[2] & (1 <<  9)) != 0;
            (*feature_map)[(int)feature::SSE41]  = (info[2] & (1 << 19)) != 0;
            (*feature_map)[(int)feature::SSE42]  = (info[2] & (1 << 20)) != 0;
            (*feature_map)[(int)feature::AES]    = (info[2] & (1 << 25)) != 0;
            (*feature_map)[(int)feature::AVX]    = (info[2] & (1 << 28)) != 0;
            (*feature_map)[(int)feature::FMA3]   = (info[2] & (1 << 12)) != 0;
            (*feature_map)[(int)feature::RDRAND] = (info[2] & (1 << 30)) != 0;
        }
        if (nIds >= 0x00000007)
        {
            W32_CPUID(info,0x00000007);
            (*feature_map)[(int)feature::AVX2]        = (info[1] & (1 <<  5)) != 0;
            (*feature_map)[(int)feature::BMI1]        = (info[1] & (1 <<  3)) != 0;
            (*feature_map)[(int)feature::BMI2]        = (info[1] & (1 <<  8)) != 0;
            (*feature_map)[(int)feature::ADX]         = (info[1] & (1 << 19)) != 0;
            (*feature_map)[(int)feature::SHA]         = (info[1] & (1 << 29)) != 0;
            (*feature_map)[(int)feature::PREFETCHWT1] = (info[2] & (1 <<  0)) != 0;
            (*feature_map)[(int)feature::AVX512F]     = (info[1] & (1 << 16)) != 0;
            (*feature_map)[(int)feature::AVX512CD]    = (info[1] & (1 << 28)) != 0;
            (*feature_map)[(int)feature::AVX512PF]    = (info[1] & (1 << 26)) != 0;
            (*feature_map)[(int)feature::AVX512ER]    = (info[1] & (1 << 27)) != 0;
            (*feature_map)[(int)feature::AVX512VL]    = (info[1] & (1 << 31)) != 0;
            (*feature_map)[(int)feature::AVX512BW]    = (info[1] & (1 << 30)) != 0;
            (*feature_map)[(int)feature::AVX512DQ]    = (info[1] & (1 << 17)) != 0;
            (*feature_map)[(int)feature::AVX512IFMA]  = (info[1] & (1 << 21)) != 0;
            (*feature_map)[(int)feature::AVX512VBMI]  = (info[2] & (1 <<  1)) != 0;
        }
        if (nExIds >= 0x80000001)
        {
            W32_CPUID(info,0x80000001);
            (*feature_map)[(int)feature::x64]   = (info[3] & (1 << 29)) != 0;
            (*feature_map)[(int)feature::ABM]   = (info[2] & (1 <<  5)) != 0;
            (*feature_map)[(int)feature::SSE4a] = (info[2] & (1 <<  6)) != 0;
            (*feature_map)[(int)feature::FMA4]  = (info[2] & (1 << 16)) != 0;
            (*feature_map)[(int)feature::XOP]   = (info[2] & (1 << 11)) != 0;
        }
    }
    
    void print_enabled_features(std::ostream& ostr)
    {
        if(!initialized)
        {
            init();
            initialized = true;
        }

        ostr << "-- Enabled CPU features (x86) --" << std::endl;
        for(auto& [ftype, enabled] : *feature_map)
        {            
            ostr << " >> [" << (*feature_names).at(ftype).c_str() << "] "
                 << (enabled ? "YES" : "NO") << std::endl;
        }
        ostr << std::endl;
    }

    bool get_feature(feature f)
    {
        if(!initialized)
        {
            init();
            initialized = true;
        }
        return (*feature_map)[(int)f];
    }

    void force_feature(feature f, bool enabled)
    {
        if(!initialized)
        {
            init();
            initialized = true;
        }
        (*feature_map)[(int)f] = enabled;
    }
}

#endif