#include <ien/image.hpp>

#include <ien/assert.hpp>
#include <ien/platform.hpp>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>
#include <stdexcept>

namespace ien::img
{

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// class image_unpacked_data
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

    image_unpacked_data::image_unpacked_data(size_t pixel_count)
        : _r(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _g(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _b(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _a(reinterpret_cast<uint8_t*>(LIEN_ALIGNED_ALLOC(pixel_count)))
        , _size(pixel_count)
    { }

    image_unpacked_data::~image_unpacked_data()
    {
        LIEN_ALIGNED_FREE(_r);
        LIEN_ALIGNED_FREE(_g);
        LIEN_ALIGNED_FREE(_b);
        LIEN_ALIGNED_FREE(_a);
    }

    uint8_t* image_unpacked_data::data_r() noexcept { return _r; }
    uint8_t* image_unpacked_data::data_g() noexcept { return _g; }
    uint8_t* image_unpacked_data::data_b() noexcept { return _b; }
    uint8_t* image_unpacked_data::data_a() noexcept { return _a; }

    const uint8_t* image_unpacked_data::cdata_r() const noexcept { return _r; }
    const uint8_t* image_unpacked_data::cdata_g() const noexcept { return _g; }
    const uint8_t* image_unpacked_data::cdata_b() const noexcept { return _b; }
    const uint8_t* image_unpacked_data::cdata_a() const noexcept { return _a; }

    size_t image_unpacked_data::size() const noexcept { return _size; }

    std::vector<uint8_t> image_unpacked_data::pack_data()
    {
        std::vector<uint8_t> result;
        result.resize(_size * 4);
        for(size_t i = 0; i < _size; ++i)
        {
            result[(i * 4) + 0] = _r[i];
            result[(i * 4) + 1] = _g[i];
            result[(i * 4) + 2] = _b[i];
            result[(i * 4) + 3] = _a[i];
        }
        return result;
    }

    image_unpacked_data unpack_image_data(uint8_t* data, size_t len)
    {
        runtime_assert(len % 4 == 0, "Packed image data len must be a multiple of 4!");
        image_unpacked_data result(len / 4);

        uint8_t* r = result.data_r();
        uint8_t* g = result.data_g();
        uint8_t* b = result.data_b();
        uint8_t* a = result.data_a();

        for(size_t i = 0; i < len; i += 4)
        {
            r[i / 4] = data[i + 0];
            g[i / 4] = data[i + 1];
            b[i / 4] = data[i + 2];
            a[i / 4] = data[i + 3];
        }
        return result;
    }

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// class image
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

    image::image(int width, int height)
        : _width(width)
        , _height(height)
        , _data(static_cast<size_t>(width) * height)
    { }

    image::image(const std::string& path)
		: _width(0), _height(0)
    {
        int channels_dummy = 0;
        uint8_t* packed_data = stbi_load(
            path.c_str(),
            &_width,
            &_height,
            &channels_dummy,
            4
        );

        if(packed_data == nullptr)
        {
            throw std::invalid_argument("Unable to load image with path: " + path);
        }
        _data = unpack_image_data(packed_data, (static_cast<size_t>(_width) * _height * 4));
        stbi_image_free(packed_data);
    }

    image_unpacked_data* image::data() noexcept { return &_data; }

    const image_unpacked_data* image::cdata() const noexcept { return &_data; }

    void image::save_to_file_png(const std::string& path, int compression_level)
    {
        std::vector<uint8_t> packed_data = _data.pack_data();
        stbi_write_png_compression_level = compression_level;
        stbi_write_png(path.c_str(), _width, _height, 4, packed_data.data(), _width * 4);
    }

    size_t image::pixel_count() const noexcept
    {
        return static_cast<size_t>(_width) * _height;
    }

    int image::width() const noexcept { return _width; }
    
    int image::height() const noexcept { return _height; }

    void image::save_to_file_jpeg(const std::string& path, int quality)
    {
        std::vector<uint8_t> packed_data = _data.pack_data();
        stbi_write_jpg(path.c_str(), _width, _height, 4, packed_data.data(), quality);
    }

    void image::save_to_file_tga(const std::string& path)
    {
        std::vector<uint8_t> packed_data = _data.pack_data();
        stbi_write_tga(path.c_str(), _width, _height, 4, packed_data.data());
    }
}