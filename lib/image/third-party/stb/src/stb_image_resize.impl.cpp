#define STB_IMAGE_RESIZE_IMPLEMENTATION

// Set default alignment of stbi malloc to 32 bits (AVX)
#ifndef LIEN_STB_ALLIGN_MALLOC
    #define LIEN_STB_ALLIGN_MALLOC 32
#endif

#include <ien/platform.hpp>

// Align to 16byte on SSE2 compatible platforms
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    #if defined(LIEN_OS_WIN64) || defined(LIEN_OS_WIN32) // thanks microsoft
        #define STBIR_MALLOC(sz) _aligned_malloc(sz, LIEN_STBI_ALLIGN_MALLOC)
        #define STBIR_FREE(h) _aligned_free(h)
        #define STBIR_REALLOC(h, sz) _aligned_realloc(h, sz, LIEN_STBI_ALLIGN_MALLOC)
    #else
        #define STBIR_MALLOC(sz) std::aligned_alloc(LIEN_STBI_ALLIGN_MALLOC, sz)
        #define STBIR_FREE(h) std::free(h)
        #define STBIR_REALLOC(h, sz) std::realloc(h, sz)
    #endif
#endif

#include "stb_image_resize.h"