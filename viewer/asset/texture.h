#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <filesystem>

namespace opengl { class context; }

namespace asset
{

using color = glm::u8vec4;

class texture
{
private:

public:
    virtual void create(unsigned int width, unsigned int height, const color& color = {0, 0, 0, 255}) = 0;
    virtual void resize(unsigned int width, unsigned int height) = 0;
    virtual const glm::uvec2& size() = 0;

    virtual void repeat(bool value = true) = 0;
    virtual void smooth(bool value = true) = 0;

    virtual void data(const unsigned char* pixels, unsigned int width, unsigned int height) = 0;
    virtual void data(const unsigned char* pixels) = 0;
};


class texture_loader
{
public:
    using result_type = std::shared_ptr<texture>;
    result_type operator()(opengl::context& context, const std::filesystem::path& path);
};

}


