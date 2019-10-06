#include <ien/pixel_trait.hpp>

#include <ien/assert.hpp>
#include <algorithm>
#include <array>

namespace ien::img
{
    float saturation(const uint8_t* pxdata)
    {
        const std::initializer_list<uint8_t> il_rgb = { 
            pxdata[0], pxdata[1], pxdata[2] 
        };

        uint8_t max_rgb = std::max(il_rgb);
        uint8_t min_rgb = std::min(il_rgb);

        return static_cast<float>(max_rgb - min_rgb) / max_rgb;
    }

    float luminance(const uint8_t* pxdata)
    {
        const std::initializer_list<uint8_t> il_rgb = { 
            pxdata[0], pxdata[1], pxdata[2] 
        };

        uint8_t max_rgb = std::max(il_rgb);
        uint8_t min_rgb = std::min(il_rgb);

        return std::abs((0.5F * (max_rgb + min_rgb)) / 255.0F);
    }

    float average_ga(const uint8_t* pxdata)
    {
        uint8_t g = pxdata[0];
        uint8_t a = pxdata[1];
        return static_cast<float>(g + a) / 2.0F;
    }

    float average_rgb(const uint8_t* pxdata)
    {
        uint8_t r = pxdata[0];
        uint8_t g = pxdata[1];
        uint8_t b = pxdata[2];
        return static_cast<float>(r + g + b) / 3.0F;
    }

    float average_rgba(const uint8_t* pxdata)
    {
        uint8_t r = pxdata[0];
        uint8_t g = pxdata[1];
        uint8_t b = pxdata[2];
        uint8_t a = pxdata[3];
        return static_cast<float>(r + g + b + a) / 4.0F;
    }

    float get_pixel_trait(pixel_trait pxt, const uint8_t* pxdata)
    {
        switch(pxt)
        {
            case pixel_trait::GS_GA_GVALUE:
            case pixel_trait::RGB_RGBA_COLOR_RED:
                return static_cast<float>(pxdata[0]) / 255.0F;
            case pixel_trait::GA_ALPHA:
            case pixel_trait::RGB_RGBA_COLOR_GREEN:
                return static_cast<float>(pxdata[1]) / 255.0F;
            case pixel_trait::RGB_RGBA_COLOR_BLUE:
                return static_cast<float>(pxdata[2]) / 255.0F;
            case pixel_trait::RGBA_ALPHA:
                return static_cast<float>(pxdata[3]) / 255.0F;
            case pixel_trait::GA_AVG_VALUE:
                return average_ga(pxdata);
            case pixel_trait::RGB_AVG_VALUE:
                return average_rgb(pxdata);
            case pixel_trait::RGBA_AVG_VALUE:
                return average_rgba(pxdata);
            case pixel_trait::RGB_RGBA_LUMINANCE:
                return luminance(pxdata);
            case pixel_trait::RGB_RGBA_SATURATION:
                return saturation(pxdata);
            default:
                throw std::invalid_argument("Invalid pixel_trait!");
        }
    }

    std::vector<float> get_pixel_trait_map(
        pixel_trait pxt,
        const uint8_t* px_data,
        size_t count)
    {
        std::vector<float> result;
        result.resize(count);

        int stride = 4;
        for(size_t i = 0; i < count; ++i)
        {
            const uint8_t* ptr = px_data + (i * stride);
            result[i] = get_pixel_trait(pxt, ptr);
        }
        return result;
    }
}