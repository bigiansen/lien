#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#include <ien/platform.hpp>
// Align to 16byte on SSE2 compatible platforms
#if defined(LIEN_ARCH_X86_64) || defined(LIEN_ARCH_X86)
    #if defined(LIEN_OS_WIN64) || defined(LIEN_OS_WIN32) // thanks microsoft
        #define STBI_MALLOC(sz) _aligned_malloc(sz, 16)
        #define STBI_FREE(h) _aligned_free(h)
        #define STBI_REALLOC(h, sz) _aligned_realloc(h, sz, 16)
    #else
        #define STBI_MALLOC(sz) std::aligned_alloc(16, sz)
        #define STBI_FREE(h) std::free(h)
        #define STBI_REALLOC(h, sz) std::realloc(h, sz)
    #endif
#endif

#include "stb_image.h"