#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <filesystem>
#include <vector>
#include <memory>

namespace asset
{

using color = glm::u8vec4;

class image
{
private:
    std::vector<color> m_pixels;
    glm::uvec2 m_size;

public:
    image(unsigned int width = 1, unsigned int height = 1, const color& color = {0, 0, 0, 255});
    image(unsigned int width, unsigned int height, unsigned char* data);

    void create(unsigned int width, unsigned int height, const color& color);

    const glm::uvec2& size() const;
    const unsigned char* ptr() const;
    unsigned char* ptr();
    const std::vector<color>& pixels() const;

    const color& operator()(int x, int y) const;
    color& operator()(int x, int y);
};

class image_loader
{
public:
    using result_type = std::shared_ptr<image>;
    static result_type load(const std::filesystem::path& path);
};

}
