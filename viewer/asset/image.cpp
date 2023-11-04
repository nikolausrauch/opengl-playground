#include "image.h"

#include "viewer/core/assert.h"
#include "viewer/core/log.h"

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

namespace asset
{

image::image(unsigned int width, unsigned int height, const color &color)
    : m_pixels(width*height, color), m_size(width, height)
{

}

image::image(unsigned int width, unsigned int height, unsigned char *data)
    : m_pixels(reinterpret_cast<color*>(data), reinterpret_cast<color*>(data) + width * height),
      m_size(width, height)
{

}

void image::create(unsigned int width, unsigned int height, const color &color)
{
    m_pixels.resize(width*height, color);
}

const glm::uvec2 &image::size() const
{
    return m_size;
}

const unsigned char *image::ptr() const
{
    return reinterpret_cast<const unsigned char*>(m_pixels.data());
}

unsigned char *image::ptr()
{
    return reinterpret_cast<unsigned char*>(m_pixels.data());
}

const std::vector<color> &image::pixels() const
{
    return m_pixels;
}

const color &image::operator()(int x, int y) const
{
    platform_assert(x*y < static_cast<int>(m_size.x * m_size.y), "Indices out of bounds");
    return m_pixels[y*m_size.x + x];
}

color& image::operator()(int x, int y)
{
    platform_assert(x*y < static_cast<int>(m_size.x * m_size.y), "Indices out of bounds");
    return m_pixels[y*m_size.x + x];
}

image_loader::result_type image_loader::load(const std::filesystem::path &path)
{
    int width = 1;
    int height = 1;
    int components = 4;

    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &components, 4);

    if(data == nullptr)
    {
        platform_log(core::log::level::error, "[asset::image] Coudn't load image {0}", path.string());
        return result_type(new image(1, 1, color{255, 0, 0, 255}));
    }

    auto* img = new image(width, height, data);
    stbi_image_free(data);

    return result_type(img);
}

}
