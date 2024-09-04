#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "viewer/opengl/texture.h"
#include "viewer/opengl/texturecube.h"

#include <memory>
#include <filesystem>

namespace opengl { class context; }

namespace asset
{

class image;

class texture_loader
{
public:
    using result_type_tex = std::shared_ptr<opengl::texture>;
    static result_type_tex load(opengl::context& context, const std::filesystem::path& path, bool flip = true);
    static result_type_tex load(opengl::context& context, const image& img);
    static result_type_tex load(opengl::context& context, unsigned int width, unsigned int height, const glm::u8vec4& color = {0, 0, 0, 255});

    using result_type_cube = std::shared_ptr<opengl::texture_cube>;
    static result_type_cube load_cube(opengl::context& context, const std::array<std::filesystem::path, 6>& paths, bool flip = true);
};

}


