#pragma once

#include <ien/platform.hpp>
#include <iostream>
#include <string>

inline bool sse2_enabled()
{
    #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        return ien::platform::x86::get_feature(ien::platform::x86::feature::SSE2);
    #else
        return false;
    #endif
}

inline bool avx2_enabled()
{
    #if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
        return ien::platform::x86::get_feature(ien::platform::x86::feature::AVX2);
    #else
        return false;
    #endif
}

inline void skip_sse2_msg(const std::string& method_name)
{
    std::cout 
            << "[WARNING]: SSE2 appears to be unavailable." 
            << "Skipping test: "
            << method_name
            << std::endl;
}

inline void skip_avx2_msg(const std::string& method_name)
{
    std::cout 
            << "[WARNING]: AVX2 appears to be unavailable." 
            << "Skipping test: "
            << method_name
            << std::endl;
}

#define CHECK_SSE2(method_name) \
    if(!sse2_enabled()){ \
        skip_sse2_msg(method_name); \
        return; \
    }

#define CHECK_AVX2(method_name) \
    if(!avx2_enabled()){ \
        skip_avx2_msg(method_name); \
        return; \
    }
