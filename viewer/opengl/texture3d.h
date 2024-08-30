#pragma once

#include "texture.h"

#include <glm/vec3.hpp>

namespace asset { class volume_loader; }

namespace opengl
{

class texture_3D final
{
private:
    context& m_context;
    GLuint m_handle;
    texture_internal_type m_internal_type;
    texture_format m_format;
    texture_type m_type;
    glm::uvec3 m_size = {1, 1, 1};

public:
    ~texture_3D();

    texture_3D(const texture_3D &) = delete;
    texture_3D &operator=(const texture_3D &) = delete;

    void create(unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const glm::u8vec4& color = {0, 0, 0, 255});
    void resize(unsigned int width, unsigned int height, unsigned int depth);
    const glm::uvec3& size();

    void repeat(bool value = true);
    void smooth(bool value = true);

    void data(const unsigned char* pixels, unsigned int width, unsigned int height, unsigned int depth);
    void data(const unsigned char* pixels);

    void parameter(min_filter filter);
    void parameter(mag_filter filter);
    void parameter(wrap_coord coord, wrapping wrap);
    void parameter(texture_color flag, const glm::vec4 &color);

    void bind(unsigned int unit = 0) const;
    void unbind(unsigned int unit = 0) const;

    GLuint gl_handle() const;
    texture_internal_type internal_format_type() const;
    texture_format format() const;
    texture_type type() const;

private:
    texture_3D(context& gl_context, texture_internal_type internal, texture_format format, texture_type type,
            unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const glm::vec4& color = {0, 0, 0, 255});
    texture_3D(context& gl_context, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const glm::vec4& color = {0, 0, 0, 255});

    void generate_mip_maps();

    friend context;
    friend asset::volume_loader;
};


}
