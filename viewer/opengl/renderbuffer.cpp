#include "renderbuffer.h"

#include "viewer/core/assert.h"

namespace opengl
{

renderbuffer::~renderbuffer()
{
    glDeleteRenderbuffers(1, &m_handle);
}

renderbuffer::renderbuffer(context &gl_context, renderbuffer_internal_type type, unsigned int width, unsigned int height)
    : m_context(gl_context), m_type(type), m_size(width, height)
{
    glGenRenderbuffers(1, &m_handle);
    platform_assert(m_handle != 0, "Unable to allocate a new renderbuffer handle");

    if(width == 0 || height == 0) return;
    storage(type, width, height);
}

GLuint renderbuffer::gl_handle() const
{
    return m_handle;
}

void renderbuffer::storage(renderbuffer_internal_type internal_format, unsigned int width, unsigned int height)
{
    m_type = internal_format;
    m_size = {width, height};

    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(m_type), width, height);
    unbind();
}

void renderbuffer::resize(unsigned int width, unsigned int height)
{
    storage(m_type, width, height);
}

renderbuffer_internal_type renderbuffer::format() const
{
    return m_type;
}

const glm::uvec2 &renderbuffer::size() const
{
    return m_size;
}

void renderbuffer::bind()
{
    m_context.bind_renderbuffer(GL_RENDERBUFFER, m_handle);
}

void renderbuffer::unbind()
{
    m_context.bind_renderbuffer(GL_RENDERBUFFER, 0);
}

}
