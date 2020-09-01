#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#include <ien/platform.hpp>

#define STBI_MALLOC(sz) ien::platform::aligned_alloc(sz, LIEN_DEFAULT_ALIGNMENT)
#define STBI_FREE(ptr) ien::platform::aligned_free(ptr)
#define STBI_REALLOC(ptr, sz) ien::platform::aligned_realloc(ptr, sz, LIEN_DEFAULT_ALIGNMENT)

#include "stb_image.h"