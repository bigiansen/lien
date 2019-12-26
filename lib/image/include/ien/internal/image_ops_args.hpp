#pragma once

#include <ien/image.hpp>
#include <ien/rgba_channel.hpp>
#include <cinttypes>

namespace ien::image_ops::_internal
{
    struct truncate_channel_args
    {
        size_t len = 0;
        uint8_t* ch_r = nullptr;
        uint8_t* ch_g = nullptr;
        uint8_t* ch_b = nullptr;
        uint8_t* ch_a = nullptr;
        int bits_r = 0;
        int bits_g = 0;
        int bits_b = 0;
        int bits_a = 0;

        constexpr truncate_channel_args() { }

        truncate_channel_args(image& img, int r, int g, int b, int a)
            : len(img.pixel_count())
            , ch_r(img.data()->data_r())
            , ch_g(img.data()->data_g())
            , ch_b(img.data()->data_b())
            , ch_a(img.data()->data_a())
            , bits_r(r)
            , bits_g(g)
            , bits_b(b)
            , bits_a(a)
        { }

        truncate_channel_args(image_unpacked_data& img, int r, int g, int b, int a)
            : len(img.size())
            , ch_r(img.data_r())
            , ch_g(img.data_g())
            , ch_b(img.data_b())
            , ch_a(img.data_a())
            , bits_r(r)
            , bits_g(g)
            , bits_b(b)
            , bits_a(a)
        { }
    };

    struct channel_info_extract_args_rgba
    {
        size_t len = 0;
        const uint8_t* ch_r = nullptr;
        const uint8_t* ch_g = nullptr;
        const uint8_t* ch_b = nullptr;
        const uint8_t* ch_a = nullptr;

        constexpr channel_info_extract_args_rgba() { }

        channel_info_extract_args_rgba(const image& img)
            : len(img.pixel_count())
            , ch_r(img.cdata()->cdata_r())
            , ch_g(img.cdata()->cdata_g())
            , ch_b(img.cdata()->cdata_b())
            , ch_a(img.cdata()->cdata_a())
        { }
    };

    struct channel_info_extract_args_rgb
    {
        size_t len = 0;
        const uint8_t* ch_r = nullptr;
        const uint8_t* ch_g = nullptr;
        const uint8_t* ch_b = nullptr;

        constexpr channel_info_extract_args_rgb() { }

        channel_info_extract_args_rgb(const image& img)
            : len(img.pixel_count())
            , ch_r(img.cdata()->cdata_r())
            , ch_g(img.cdata()->cdata_g())
            , ch_b(img.cdata()->cdata_b())
        { }
    };

    struct channel_compare_args
    {
        size_t len = 0;
        const uint8_t* ch = nullptr;
        const uint8_t threshold = 0;

        channel_compare_args(const image& img, rgba_channel channel, uint8_t thres)
            : len(img.pixel_count())
            , threshold(thres)
        {
            switch(channel)
            {
                case rgba_channel::R:
                    ch = img.cdata()->cdata_r();
                    break;
                case rgba_channel::G:
                    ch = img.cdata()->cdata_g();
                    break;
                case rgba_channel::B:
                    ch = img.cdata()->cdata_b();
                    break;
                case rgba_channel::A:
                    ch = img.cdata()->cdata_a();
                    break;
            }
        }
    };
}