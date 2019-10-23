#pragma once

//--------------------------------------------
// ARCH
//--------------------------------------------
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64)
    #define LIEN_ARCH_X86_64
#endif

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #define LIEN_ARCH_X86
#endif

#if defined(__arm__) || defined(_M_ARM)
    #define LIEN_ARCH_ARM
#endif

#if defined(__aarch64__)
    #define LIEN_ARCH_ARM64
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// OS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if defined(_WIN64)
    #define LIEN_OS_WIN64
#elif defined(_WIN32)
    #define LIEN_OS_WIN32
#endif

#if defined(__unix__)
    #define LIEN_OS_UNIX
#endif

#if defined(__APPLE__)
    #define LIEN_OS_MAC
#endif

#if defined(__linux__)
    #define LIEN_OS_LINUX
#endif

#if defined(__FreeBSD__)
    #define LIEN_OS_FREEBSD
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// COMPILER
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#if defined(_MSC_VER)
    #define LIEN_COMPILER_MSVC
    
#elif defined(__GNUC__) && !defined(__clang__)
    #define LIEN_COMPILER_GNU

#elif defined(__clang__)
    #define LIEN_COMPILER_CLANG   

#elif defined(__INTEL_COMPILER)
    #define LIEN_COMPILER_INTEL

#elif defined(__EMSCRIPTEN__)
    #define LIEN_COMPILER_EMSCRIPTEN

#elif defined(__wasm__)
    #define LIEN_COMPILER_WASM

#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ALIGNED ALLOCATION
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// Set default alignment of aligned (m)alloc to 32 bits (AVX)
#ifndef LIEN_ALIGNED_ALLOC_ALIGNMENT
    #define LIEN_ALIGNED_ALLOC_ALIGNMENT 32
#endif

#if defined(LIEN_COMPILER_MSVC)
    #define LIEN_ALIGNED_ALLOC(sz) _aligned_malloc(sz, LIEN_ALIGNED_ALLOC_ALIGNMENT)
    #define LIEN_ALIGNED_FREE(ptr) _aligned_free(ptr)
    #define LIEN_ALIGNED_REALLOC(ptr, sz) _aligned_realloc(ptr, sz, LIEN_ALIGNED_ALLOC_ALIGNMENT)
#else
    #define LIEN_ALIGNED_ALLOC(sz) std::aligned_alloc(LIEN_ALIGNED_ALLOC_ALIGNMENT, sz)
    #define LIEN_ALIGNED_FREE(ptr) std::free(ptr)
    #define LIEN_ALIGNED_REALLOC(ptr, sz) std::realloc(ptr, sz)
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PLATFORM FEATURES (x86)
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#if defined(LIEN_ARCH_X86) || defined(LIEN_ARCH_X86_64)

#include <iostream>
namespace ien::platform::x86
{
    enum class feature : int
    {
        ABM, ADX,
        AES, AVX2,
        AVX512BW, AVX512CD,
        AVX512DQ, AVX512ER,
        AVX512F, AVX512IFMA,
        AVX512PF, AVX512VBMI,
        AVX512VL, AVX,
        BMI1, BMI2,
        FMA3, FMA4,
        MMX, PREFETCHWT1,
        RDRAND, SHA,
        SSE2, SSE3,
        SSE41, SSE42,
        SSE4a, SSE,
        SSSE3, x64,
        XOP
    };

    extern void force_feature(feature, bool);
    extern bool get_feature(feature);
    extern void print_enabled_features(std::ostream& = std::cout);
}

#endif