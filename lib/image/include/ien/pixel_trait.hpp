#pragma once

#include <cinttypes>
#include <vector>

namespace ien::img
{
    enum class pixel_trait
    {
        GS_GA_GVALUE = 0,
        GA_ALPHA = 1,
        GA_AVG_VALUE = 2,
        RGB_RGBA_COLOR_RED = 3,
        RGB_RGBA_COLOR_GREEN = 4,
        RGB_RGBA_COLOR_BLUE = 5,
        RGB_RGBA_LUMINANCE = 6,
        RGB_RGBA_SATURATION = 7,
        RGBA_ALPHA = 8,
        RGB_AVG_VALUE = 9,
        RGBA_AVG_VALUE = 10,
    };

    extern float get_pixel_trait(
        pixel_trait,
        const uint8_t* px_data);

    extern std::vector<float> get_pixel_trait_map(
        pixel_trait, 
        const uint8_t* px_data,
        size_t count);
}