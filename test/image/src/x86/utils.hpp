#pragma once

#include <ien/platform.hpp>
#include <iostream>
#include <string>

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)
    #define LIEN_SIMD_TEMPLATE_ENABLED_X86(feat) ien::platform::x86::get_feature(ien::platform::x86::feature:: ##feat)
#else
    #define LIEN_SIMD_TEMPLATE_ENABLED_X86(feat) false
#endif

#define LIEN_SSE2_ENABLED() LIEN_SIMD_TEMPLATE_ENABLED_X86(SSE2)
#define LIEN_SSE3_ENABLED() LIEN_SIMD_TEMPLATE_ENABLED_X86(SSE3)
#define LIEN_SSE41_ENABLED() LIEN_SIMD_TEMPLATE_ENABLED_X86(SSE41)
#define LIEN_AVX_ENABLED() LIEN_SIMD_TEMPLATE_ENABLED_X86(AVX)
#define LIEN_AVX2_ENABLED() LIEN_SIMD_TEMPLATE_ENABLED_X86(AVX2)

#define LIEN_SKIP_SIMD_TEMPLATE(feat, method) \
    std::cout << "[WARNING]: feature \"" << #feat << "\" appears to be unavailable." \
        << "Skipping test: " << method << "\n";

#define LIEN_SKIP_SSE2_MSG(method) LIEN_SKIP_SIMD_TEMPLATE(SSE2, method)
#define LIEN_SKIP_SSE3_MSG(method) LIEN_SKIP_SIMD_TEMPLATE(SSE3, method)
#define LIEN_SKIP_SSE41_MSG(method) LIEN_SKIP_SIMD_TEMPLATE(SSE41, method)
#define LIEN_SKIP_AVX_MSG(method) LIEN_SKIP_SIMD_TEMPLATE(AVX, method)
#define LIEN_SKIP_AVX2_MSG(method) LIEN_SKIP_SIMD_TEMPLATE(AVX2, method)

#define LIEN_CHECK_SIMD_TEMPLATE(feat, method, fail) \
    if(!LIEN_ ##feat ##_ENABLED()) { LIEN_SKIP_ ##feat ##_MSG(method); fail;}

#define LIEN_CHECK_SSE2(method, fail) LIEN_CHECK_SIMD_TEMPLATE(SSE2, method, fail)
#define LIEN_CHECK_SSE3(method, fail) LIEN_CHECK_SIMD_TEMPLATE(SSE3, method, fail)
#define LIEN_CHECK_SSE41(method, fail) LIEN_CHECK_SIMD_TEMPLATE(SSE41, method, fail)
#define LIEN_CHECK_AVX(method, fail) LIEN_CHECK_SIMD_TEMPLATE(AVX, method, fail)
#define LIEN_CHECK_AVX2(method, fail) LIEN_CHECK_SIMD_TEMPLATE(AVX2, method, fail)