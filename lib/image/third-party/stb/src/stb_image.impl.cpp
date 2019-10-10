#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#include <ien/platform.hpp>

#define STBI_MALLOC(sz) LIEN_ALIGNED_ALLOC(sz)
#define STBI_FREE(ptr) LIEN_ALIGNED_FREE(ptr)
#define STBI_REALLOC(ptr, sz) LIEN_ALIGNED_REALLOC(ptr, sz)

#include "stb_image.h"