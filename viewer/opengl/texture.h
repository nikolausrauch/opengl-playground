#pragma once

#include "context.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace asset { class texture_loader; }

namespace opengl
{

enum class texture_internal_type : GLenum
{
    r8 = GL_R8,
    r16 = GL_R16,
    r16F = GL_R16F,
    rgb8 = GL_RGB8,
    rgb8U = GL_RGB8UI,
    rgb16 = GL_RGB16,
    rgb16U = GL_RGB16UI,
    rgb16F = GL_RGB16F,
    rgb32F = GL_RGB32F,
    rgba8 = GL_RGBA8,
    rgba8u = GL_RGB8UI,
    rgba16 = GL_RGBA16,
    rgba16f = GL_RGBA16F,
    rgba32ui = GL_RGBA32UI,
    depth = GL_DEPTH_COMPONENT,
    depth32 = GL_DEPTH_COMPONENT32,
    depth_stencil = GL_DEPTH_STENCIL,
    depth24_stencil8 = GL_DEPTH24_STENCIL8
};

enum class texture_format : GLenum
{
    red = GL_RED,
    rgb = GL_RGB,
    rgb_int = GL_RGB_INTEGER,
    rgba = GL_RGBA,
    rgba_int = GL_RGBA_INTEGER,
    depth = GL_DEPTH_COMPONENT,
    depth_stencil = GL_DEPTH_STENCIL
};

enum class texture_type : GLenum
{
    unsigned_byte_ = GL_UNSIGNED_BYTE,
    unsigned_int_24_8 = GL_UNSIGNED_INT_24_8,
    byte_ = GL_BYTE,
    unsigned_short_ = GL_UNSIGNED_SHORT,
    short_ = GL_SHORT,
    unsigned_int_ = GL_UNSIGNED_INT,
    int_ = GL_INT,
    float_ = GL_FLOAT
};

enum class texture_color : GLenum
{
    border_color = GL_TEXTURE_BORDER_COLOR
};

enum class wrapping : GLenum
{
    edge = GL_CLAMP_TO_EDGE,
    border = GL_CLAMP_TO_BORDER,
    repeat = GL_REPEAT,
    mirrored_repeat = GL_MIRRORED_REPEAT,
    mirrored_edge = GL_MIRROR_CLAMP_TO_EDGE
};

enum class wrap_coord : GLenum
{
    wrap_s = GL_TEXTURE_WRAP_S,
    wrap_t = GL_TEXTURE_WRAP_T,
    wrap_r = GL_TEXTURE_WRAP_R
};

enum class min_filter : GLenum
{
    nearest = GL_NEAREST,
    linear = GL_LINEAR,
    nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
    linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
    nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
    linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR
};

enum class mag_filter : GLenum
{
    nearest = GL_NEAREST,
    linear = GL_LINEAR
};

enum class pixel_format : GLenum
{
    stencil = GL_STENCIL_INDEX,
    depth_component = GL_DEPTH_COMPONENT,
    red = GL_RED,
    green = GL_GREEN,
    blue = GL_BLUE,
    alpha = GL_ALPHA,
    rgb = GL_RGB,
    bgr = GL_BGR,
    rgba = GL_RGBA,
    rgba_integer = GL_RGBA_INTEGER,
    bgra = GL_BGRA
};


class texture final
{
private:
    context& m_context;
    GLuint m_handle;
    texture_internal_type m_internal_type;
    texture_format m_format;
    texture_type m_type;
    glm::uvec2 m_size = {1, 1};

public:
    ~texture();

    texture(const texture &) = delete;
    texture &operator=(const texture &) = delete;

    void create(unsigned int width = 1, unsigned int height = 1, const glm::u8vec4& color = {0, 0, 0, 255});
    void resize(unsigned int width, unsigned int height);
    const glm::uvec2& size();

    void repeat(bool value = true);
    void smooth(bool value = true);

    void data(const unsigned char* pixels, unsigned int width, unsigned int height);
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
    texture(context& gl_context, texture_internal_type internal, texture_format format, texture_type type,
            unsigned int width = 1, unsigned int height = 1, const glm::vec4& color = {0, 0, 0, 255});
    texture(context& gl_context, unsigned int width = 1, unsigned int height = 1, const glm::vec4& color = {0, 0, 0, 255});

    void generate_mip_maps();

    friend context;
    friend asset::texture_loader;
};

}
