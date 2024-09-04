#include "texturecube.h"

#include "viewer/core/assert.h"

#include <glm/gtc/type_ptr.hpp>

namespace opengl
{

namespace detail { int idx(cube_face face) { return static_cast<int>(face) - static_cast<int>(cube_face::positive_x); } }


texture_cube::~texture_cube()
{
    if (m_handle)
    {
        glDeleteTextures(1, &m_handle);
    }
}

void texture_cube::create(unsigned int width, unsigned int height, const glm::u8vec4& color)
{
    resize(width, height);
    generate_mip_maps();
}

void texture_cube::resize(unsigned int width, unsigned int height)
{
    resize(cube_face::positive_x, width, height);
    resize(cube_face::negative_x, width, height);
    resize(cube_face::positive_y, width, height);
    resize(cube_face::negative_y, width, height);
    resize(cube_face::positive_z, width, height);
    resize(cube_face::negative_z, width, height);
}

void texture_cube::resize(cube_face face, unsigned int width, unsigned int height)
{
    auto idx = detail::idx(face);
    m_sizes[idx] = {width, height};

    bind();
    glTexImage2D(static_cast<GLenum>(face), 0, static_cast<GLint>(m_internal_type), width, height, 0,
                 static_cast<GLenum>(m_format), static_cast<GLenum>(m_type), nullptr);
}

const glm::uvec2& texture_cube::size(cube_face face)
{
    return m_sizes[detail::idx(face)];
}

const std::array<glm::uvec2, 6>& texture_cube::sizes()
{
    return m_sizes;
}

void texture_cube::repeat(bool value)
{
    parameter(wrap_coord::wrap_r, value ? wrapping::repeat : wrapping::border);
    parameter(wrap_coord::wrap_s, value ? wrapping::repeat : wrapping::border);
    parameter(wrap_coord::wrap_t, value ? wrapping::repeat : wrapping::border);
}

void texture_cube::smooth(bool value)
{
    parameter(value ? min_filter::linear : min_filter::nearest);
    parameter(value ? mag_filter::linear : mag_filter::nearest);
}

void texture_cube::data(cube_face face, const unsigned char* pixels, unsigned int width, unsigned int height)
{
    auto idx = detail::idx(face);
    m_sizes[idx] = {width, height};

    bind();
    glTexImage2D(static_cast<GLenum>(face), 0, static_cast<GLint>(m_internal_type), width, height, 0,
                 static_cast<GLenum>(m_format), static_cast<GLenum>(m_type), pixels);
}

void texture_cube::data(cube_face face, const unsigned char* pixels)
{
    auto idx = detail::idx(face);
    data(face, pixels, m_sizes[idx].x, m_sizes[idx].y);
}

void texture_cube::parameter(min_filter filter)
{
    bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
}

void texture_cube::parameter(mag_filter filter)
{
    bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
}

void texture_cube::parameter(wrap_coord coord, wrapping wrap)
{
    bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, static_cast<GLenum>(coord), static_cast<GLint>(wrap));
}

void texture_cube::parameter(texture_color flag, const glm::vec4& color)
{
    bind();
    glTexParameterfv(GL_TEXTURE_CUBE_MAP, static_cast<GLenum>(flag), glm::value_ptr(color));
}

void texture_cube::bind(unsigned int unit) const
{
    m_context.bind_texture(GL_TEXTURE_CUBE_MAP, m_handle, unit);
}

void texture_cube::unbind(unsigned int unit) const
{
    m_context.bind_texture(GL_TEXTURE_CUBE_MAP, m_handle, 0);
}

GLuint texture_cube::gl_handle() const
{
    return m_handle;
}

texture_internal_type texture_cube::internal_format_type() const
{
    return m_internal_type;
}

texture_format texture_cube::format() const
{
    return m_format;
}

texture_type texture_cube::type() const
{
    return m_type;
}

void texture_cube::generate_mip_maps()
{
    if(m_format == texture_format::rgb_int || m_format == texture_format::rgba_int) return;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 1);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

texture_cube::texture_cube(context& gl_context, texture_internal_type internal, texture_format format, texture_type type, unsigned int width, unsigned int height, const glm::vec4& color)
    :  m_context(gl_context), m_internal_type(internal), m_format(format), m_type(type)
{
    glGenTextures(1, &m_handle);
    platform_assert(m_handle != 0, "Unable to allocate a new texture handle");

    resize(width, height);

    parameter(min_filter::linear);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 1);
}


texture_cube::texture_cube(context& gl_context, unsigned int width, unsigned int height, const glm::vec4& color)
    : m_context(gl_context), m_internal_type(texture_internal_type::rgba8), m_format(texture_format::rgba), m_type(texture_type::unsigned_byte_)
{
    glGenTextures(1, &m_handle);
    platform_assert(m_handle != 0, "Unable to allocate a new texture handle");

    std::fill(m_sizes.begin(), m_sizes.end(), glm::uvec2{width, height});

    parameter(min_filter::linear);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 1);
}



}
