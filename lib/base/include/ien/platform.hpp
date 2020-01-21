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
    #define LIEN_COMPILER_MSVC_VER _MSC_VER
    
#elif defined(__GNUC__) && !defined(__clang__)
    #define LIEN_COMPILER_GNU
    #define LIEN_COMPILER_GNU_VER __GNUC__

#elif defined(__clang__)
    #define LIEN_COMPILER_CLANG
    #define LIEN_COMPILER_CLANG_VER __clang_major__

#elif defined(__INTEL_COMPILER)
    #define LIEN_COMPILER_INTEL
    #define LIEN_COMPILER_INTEL_VER __INTEL_COMPILER

#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ARM SPECIFIC
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if defined(LIEN_ARCH_ARM) || defined(LIEN_ARCH_ARM64)
    #if defined(__ARM_NEON) || defined(__ARM_NEON__)
        #ifndef LIEN_ARM_NEON
            #define LIEN_ARM_NEON
        #endif
    #endif
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ALIGNED ALLOCATION
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define LIEN_ALIGNED_SZ(sz, alig) (sz - (sz % alig) + alig)

#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    #define LIEN_DEFAULT_ALIGNMENT 32
#elif defined(LIEN_ARCH_ARM) || defined(LIEN_ARCH_ARM64)
    #define LIEN_DEFAULT_ALIGNMENT 16
#else
    #define LIEN_DEFAULT_ALIGNMENT 32
#endif

#if defined(LIEN_COMPILER_MSVC)
    #define LIEN_ALIGNED_ALLOC(sz, alig) _aligned_malloc(LIEN_ALIGNED_SZ(sz, alig), alig)
    #define LIEN_ALIGNED_FREE(ptr) _aligned_free(ptr)
    #define LIEN_ALIGNED_REALLOC(ptr, sz, alig) _aligned_realloc(ptr, LIEN_ALIGNED_SZ(sz, alig), alig)
#else
    #include <stdlib.h> // Seems that std::aligned_alloc is not well supported yet
    #define LIEN_ALIGNED_ALLOC(sz, alig) aligned_alloc(alig, LIEN_ALIGNED_SZ(sz, alig))
    #define LIEN_ALIGNED_FREE(ptr) free(ptr)
    #define LIEN_ALIGNED_REALLOC(ptr, sz, alig) realloc(ptr, LIEN_ALIGNED_SZ(sz, alig))
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ALIGNMENT HINTS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#if defined(LIEN_COMPILER_MSVC)
    #define LIEN_HINT_ALIGNED(arg, alig) arg; __assume((((char*) arg) - ((char*) 0)) % (alig) == 0)
#elif defined(LIEN_COMPILER_GNU) || defined(LIEN_COMPILER_CLANG)
    #define LIEN_HINT_ALIGNED(arg, alig) __builtin_assume_aligned(arg, alig)
#elif defined(LIEN_COMPILER_INTEL)
    #define LIEN_HINT_ALIGNED(arg, alig) arg; __assume_aligned(arg, alig)
#else
    #define LIEN_HINT_ALIGNED(arg, alig) arg
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// UNSUPPORTED COMPILER GUARDS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifndef LIEN_COMPILER_IGNORE_VER
    #if defined(LIEN_COMPILER_MSVC)
        #if LIEN_COMPILER_MSVC_VER < 1900
            #error "Unsupported Microsoft Visual C++ Compiler Version!"
            #error "A compiler with at least C++17 support is required"
            #error "If you believe this error to be a false positive, define LIEN_COMPILER_IGNORE_VER in your code"
        #endif
    #elif defined(LIEN_COMPILER_GNU)
        #if LIEN_COMPILER_GNU_VER < 7
            #error "Unsupported GNU C++ Compiler Version!"
            #error "A compiler with at least C++17 support is required"
            #error "If you believe this error to be a false positive, define LIEN_COMPILER_IGNORE_VER in your code"
        #endif
    #elif defined(LIEN_COMPILER_CLANG)
        #if LIEN_COMPILER_CLANG_VER < 5
            #error "Unsupported Clang C++ Compiler Version!"
            #error "A compiler with at least C++17 support is required"
            #error "If you believe this error to be a false positive, define LIEN_COMPILER_IGNORE_VER in your code"
        #endif
    #elif defined(LIEN_COMPILER_INTEL)
        #if LIEN_COMPILER_INTEL_VER < 1900
            #error "Unsupported Intel C++ Compiler Version!"
            #error "A compiler with at least C++17 support is required"
            #error "If you believe this error to be a false positive, define LIEN_COMPILER_IGNORE_VER in your code"
        #endif
    #endif
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