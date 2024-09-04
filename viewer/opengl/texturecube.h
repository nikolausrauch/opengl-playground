#pragma once

#include "texture.h"

#include <array>


namespace asset { class texture_loader; }

namespace opengl
{

enum class cube_face : GLenum
{
    positive_x  = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    negative_x  = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    positive_y  = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    negative_y  = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    positive_z  = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    negative_z  = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

class texture_cube final
{
private:
    context& m_context;
    GLuint m_handle;
    texture_internal_type m_internal_type;
    texture_format m_format;
    texture_type m_type;
    std::array<glm::uvec2, 6> m_sizes;

public:
    ~texture_cube();

    texture_cube(const texture &) = delete;
    texture_cube &operator=(const texture &) = delete;

    void create(unsigned int width = 1, unsigned int height = 1, const glm::u8vec4& color = {0, 0, 0, 255});
    void resize(unsigned int width, unsigned int height);
    void resize(cube_face face, unsigned int width, unsigned int height);
    const glm::uvec2& size(cube_face face);
    const std::array<glm::uvec2, 6>& sizes();

    void repeat(bool value = true);
    void smooth(bool value = true);

    void data(cube_face face, const unsigned char* pixels, unsigned int width, unsigned int height);
    void data(cube_face face, const unsigned char* pixels);

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

    void generate_mip_maps();

private:
    texture_cube(context& gl_context, texture_internal_type internal, texture_format format, texture_type type,
            unsigned int width = 1, unsigned int height = 1, const glm::vec4& color = {0, 0, 0, 255});
    texture_cube(context& gl_context, unsigned int width = 1, unsigned int height = 1, const glm::vec4& color = {0, 0, 0, 255});

    friend context;
    friend asset::texture_loader;
};

}
