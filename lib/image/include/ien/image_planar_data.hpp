#pragma once

#include <ien/fixed_vector.hpp>

#include <array>
#include <cinttypes>
#include <vector>

namespace ien
{
    class planar_image;
    class image_planar_data
    {
        friend class planar_image;

    private:
        uint8_t* _r;
        uint8_t* _g;
        uint8_t* _b;
        uint8_t* _a;
        size_t _alignment;
        size_t _size;
        bool _moved = false;

        constexpr image_planar_data() noexcept 
            : _r(nullptr)
            , _g(nullptr)
            , _b(nullptr)
            , _a(nullptr)
            , _alignment(0)
            , _size(0)
        { }
        
    public:
        image_planar_data(size_t pixel_count);
        ~image_planar_data();

        image_planar_data(const image_planar_data& cp_src);
        image_planar_data(image_planar_data&& mv_src) noexcept;

        void operator=(image_planar_data&& mv_src) noexcept;

        uint8_t* data_r() noexcept;
        uint8_t* data_g() noexcept;
        uint8_t* data_b() noexcept;
        uint8_t* data_a() noexcept;

        const uint8_t* cdata_r() const noexcept;
        const uint8_t* cdata_g() const noexcept;
        const uint8_t* cdata_b() const noexcept;
        const uint8_t* cdata_a() const noexcept;

        size_t size() const noexcept;

        void resize(size_t len);

        uint32_t get_pixel(size_t index) const;
        void set_pixel(size_t index, uint32_t rgba);

        [[nodiscard]] ien::fixed_vector<uint8_t> pack_data() const;
    };
}