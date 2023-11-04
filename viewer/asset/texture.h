#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "viewer/opengl/texture.h"

#include <memory>
#include <filesystem>

namespace opengl { class context; }

namespace asset
{

class image;

class texture_loader
{
public:
    using result_type = std::shared_ptr<opengl::texture>;
    static result_type load(opengl::context& context, const std::filesystem::path& path);
    static result_type load(opengl::context& context, const image& img);
    static result_type load(opengl::context& context, unsigned int width, unsigned int height, const glm::u8vec4& color = {0, 0, 0, 255});
};

}


