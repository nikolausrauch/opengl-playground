#pragma once

#include "context.h"

#include <glm/vec2.hpp>

namespace opengl
{

enum class renderbuffer_internal_type : GLenum
{
    rgb                 = GL_RGB8,
    rgba                = GL_RGBA8,
    depth_component     = GL_DEPTH_COMPONENT,
    depth_componentf    = GL_DEPTH_COMPONENT32F,
    depth_component16   = GL_DEPTH_COMPONENT16,
    depth_component24   = GL_DEPTH_COMPONENT24,
    depth24_stencil8    = GL_DEPTH24_STENCIL8,
    depth32f_stencil8   = GL_DEPTH32F_STENCIL8,
    stencil_index8      = GL_STENCIL_INDEX8
};

class renderbuffer
{
private:
    context& m_context;
    GLuint m_handle;

    renderbuffer_internal_type m_type;
    glm::uvec2 m_size;

public:
    ~renderbuffer();

    renderbuffer(const renderbuffer &) = delete;
    renderbuffer &operator=(const renderbuffer &) = delete;

    GLuint gl_handle() const;

    void storage(renderbuffer_internal_type internal_format, unsigned int width, unsigned int height);
    void resize(unsigned int width, unsigned int height);

    renderbuffer_internal_type format() const;
    const glm::uvec2& size() const;

    void bind();
    void unbind();

private:
    renderbuffer(context& gl_context, renderbuffer_internal_type type, unsigned int width, unsigned height);

    friend context;
};

}
