#include "texture.h"

#include "viewer/core/assert.h"

#include <glm/gtc/type_ptr.hpp>

namespace opengl
{

texture::~texture()
{
    if (m_handle)
    {
        glDeleteTextures(1, &m_handle);
    }
}

texture::texture(context &gl_context, texture_internal_type internal, texture_format format, texture_type type,
                 unsigned int width, unsigned int height, const glm::vec4 &color)
    :  m_context(gl_context), m_internal_type(internal), m_format(format), m_type(type), m_size(width, height)
{
    glGenTextures(1, &m_handle);
    platform_assert(m_handle != 0, "Unable to allocate a new texture handle");

    parameter(min_filter::linear);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
}

void texture::create(unsigned int width, unsigned int height, const asset::color &color)
{
    resize(width, height);
    generate_mip_maps();
}

void texture::resize(unsigned int width, unsigned int height)
{
    m_size = {width, height};

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(m_internal_type), m_size.x, m_size.y, 0,
                 static_cast<GLenum>(m_format), static_cast<GLenum>(m_type), nullptr);
}

const glm::uvec2 &texture::size()
{
    return m_size;
}

void texture::repeat(bool value)
{
    parameter(wrap_coord::wrap_r, value ? wrapping::repeat : wrapping::border);
    parameter(wrap_coord::wrap_s, value ? wrapping::repeat : wrapping::border);
    parameter(wrap_coord::wrap_t, value ? wrapping::repeat : wrapping::border);
}

void texture::smooth(bool value)
{
    parameter(value ? min_filter::linear : min_filter::nearest);
    parameter(value ? mag_filter::linear : mag_filter::nearest);
}

void texture::data(const unsigned char *pixels, unsigned int width, unsigned int height)
{
    bind();
    m_size = {width, height};
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(m_internal_type), m_size.x, m_size.y, 0, static_cast<GLenum>(m_format), static_cast<GLenum>(m_type), pixels);
    generate_mip_maps();
}

void texture::data(const unsigned char *pixels)
{
    data(pixels, m_size.x, m_size.y);
}

void texture::parameter(min_filter filter)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
}

void texture::parameter(mag_filter filter)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
}

void texture::parameter(wrap_coord coord, wrapping wrap)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, static_cast<GLenum>(coord), static_cast<GLint>(wrap));
}

void texture::parameter(texture_color flag, const glm::vec4 &color)
{
    bind();
    glTexParameterfv(GL_TEXTURE_2D, static_cast<GLenum>(flag), glm::value_ptr(color));
}

void texture::bind(unsigned int unit) const
{
    m_context.bind_texture(GL_TEXTURE_2D, m_handle, unit);
}

void texture::unbind(unsigned int unit) const
{
    m_context.bind_texture(GL_TEXTURE_2D, m_handle, 0);
}

GLuint texture::gl_handle() const
{
    return m_handle;
}

texture_internal_type texture::internal_format_type() const
{
    return m_internal_type;
}

texture_format texture::format() const
{
    return m_format;
}

texture_type texture::type() const
{
    return m_type;
}

texture::texture(context &gl_context, unsigned int width, unsigned int height, const glm::vec4 &color)
    : m_context(gl_context), m_internal_type(texture_internal_type::rgba8), m_format(texture_format::rgba), m_type(texture_type::unsigned_byte_), m_size(width, height)
{
    glGenTextures(1, &m_handle);
    platform_assert(m_handle != 0, "Unable to allocate a new texture handle");

    parameter(min_filter::linear);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
}

void texture::generate_mip_maps()
{
    if(m_format == texture_format::rgb_int || m_format == texture_format::rgba_int) return;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
    glGenerateMipmap(GL_TEXTURE_2D);
}

}
