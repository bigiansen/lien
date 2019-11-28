#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

#include <zlib.h>
#include <cstdlib>

unsigned char* lien_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality)
{
    uLongf len;
    unsigned char* dest = reinterpret_cast<unsigned char*>(
        std::malloc(compressBound(data_len))
    );
    compress2(dest, &len, data, data_len, quality);
    *out_len = static_cast<int>(len);
    return dest;
}

#define STBIW_ZLIB_COMPRESS lien_zlib_compress
#include "stb_image_write.h"