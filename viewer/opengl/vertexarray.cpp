#include "vertexarray.h"


namespace opengl
{

GLuint vertexarray::gl_handle()
{
    return m_handle;
}

void vertexarray::bind() const
{
    m_context.bind_vertexarray(m_handle);
}

void vertexarray::unbind() const
{
    m_context.bind_vertexarray(0);
}

void vertexarray::draw(primitives mode_) const
{
    bind();

    GLenum mode = static_cast<GLenum>(mode_);

    if(m_indexbuffer_size == 0)
    {
        glDrawArrays(mode, 0, m_vertexbuffer_size);
    }
    else
    {
        glDrawElements(mode, m_indexbuffer_size, m_indexbuffer_type, nullptr);
    }
}

void vertexarray::draw(size_t count, primitives mode_) const
{
    bind();

    GLenum mode = static_cast<GLenum>(mode_);

    if(m_indexbuffer_size == 0)
    {
        glDrawArrays(mode, 0, count);
    }
    else
    {
        glDrawElements(mode, count, m_indexbuffer_type, nullptr);
    }
}

void vertexarray::draw(size_t offset, size_t count, primitives mode_) const
{
    bind();

    GLenum mode = static_cast<GLenum>(mode_);

    if(m_indexbuffer_size == 0)
    {
        glDrawArrays(mode, offset, count);
    }
    else
    {
        glDrawElements(mode, count, m_indexbuffer_type, (void*) offset);
    }
}

vertexarray::vertexarray(context &context)
    : m_context(context)
{
    glGenVertexArrays(1, &m_handle);
}

vertexarray::~vertexarray()
{
    if(m_handle)
    {
        glDeleteVertexArrays(1, &m_handle);
    }
}

}
