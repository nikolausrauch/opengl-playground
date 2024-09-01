#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "viewer/opengl/texture3D.h"

#include <memory>
#include <filesystem>

namespace opengl { class context; }


namespace asset
{

class volume_loader
{
public:
    using result_type = std::shared_ptr<opengl::texture_3D>;

    static result_type load_raw(opengl::context& context, const std::filesystem::path& path, const glm::uvec3& size);
    static result_type load_dat(opengl::context& context, const std::filesystem::path& path);
};

}


