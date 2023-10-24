#pragma once

#include "context.h"

#include "vertexbuffer.h"
#include "indexbuffer.h"

namespace opengl
{

namespace detail
{

template <GLuint constant>
struct ToGLType {};

template <> struct ToGLType<GL_BYTE>			{ GLbyte type; };
template <> struct ToGLType<GL_UNSIGNED_BYTE>	{ GLubyte type; };
template <> struct ToGLType<GL_SHORT>			{ GLshort type; };
template <> struct ToGLType<GL_UNSIGNED_SHORT>	{ GLushort type; };
template <> struct ToGLType<GL_INT>				{ GLint type; };
template <> struct ToGLType<GL_UNSIGNED_INT>	{ GLuint type; };
template <> struct ToGLType<GL_FLOAT>			{ GLfloat type; };
template <> struct ToGLType<GL_DOUBLE>			{ GLdouble type; };
template <> struct ToGLType<GL_HALF_FLOAT>		{ GLhalf type; };
template <> struct ToGLType<GL_FIXED>			{ GLfixed type; };


template <typename T>
struct ToGLEnum {};

template <> struct ToGLEnum<GLbyte>		{ static const GLenum value = GL_BYTE; };
template <> struct ToGLEnum<GLubyte>	{ static const GLenum value = GL_UNSIGNED_BYTE; };
template <> struct ToGLEnum<GLshort>	{ static const GLenum value = GL_SHORT; };
template <> struct ToGLEnum<GLushort>	{ static const GLenum value = GL_UNSIGNED_SHORT; };
template <> struct ToGLEnum<GLint>		{ static const GLenum value = GL_INT; };
template <> struct ToGLEnum<GLuint>		{ static const GLenum value = GL_UNSIGNED_INT; };
template <> struct ToGLEnum<GLfloat>	{ static const GLenum value = GL_FLOAT; };
template <> struct ToGLEnum<GLdouble>   { static const GLenum value = GL_DOUBLE; };

}


class vertexarray
{
private:
    context& m_context;
    GLuint m_handle;

    GLsizei m_vertexbuffer_size = 0;
    GLsizei m_vertexbuffer_attrib = 0;

    GLsizei m_indexbuffer_size = 0;
    GLenum m_indexbuffer_type = 0;

public:
    ~vertexarray();

    GLuint gl_handle();

    void bind() const;
    void unbind() const;

    template <typename T>
    void attach(const handle<vertexbuffer<T>>& vertexBuffer);

    template <typename T>
    void update(const handle<vertexbuffer<T>>& vertexBuffer);

    template <typename T>
    void attach(const handle<indexbuffer<T>>& indexBuffer);

    template <typename T>
    void update(const handle<indexbuffer<T>>& indexBuffer);


    void draw(primitives mode) const;
    void draw(size_t count, primitives mode) const;

private:
    vertexarray(context& context);

    friend context;
};

template<typename T>
void vertexarray::update(const handle<indexbuffer<T>>& indexBuffer)
{
    m_indexbuffer_size = static_cast<GLsizei>(indexBuffer->size());
    m_indexbuffer_type = detail::ToGLEnum<T>::value;
}

template<typename T>
void vertexarray::attach(const handle<indexbuffer<T>>& indexBuffer)
{
    bind();
    indexBuffer->bind();

    m_indexbuffer_size = static_cast<GLsizei>(indexBuffer->size());
    m_indexbuffer_type = detail::ToGLEnum<T>::value;
}

template<typename T>
void vertexarray::update(const handle<vertexbuffer<T>>& vertexBuffer)
{
    m_vertexbuffer_size = static_cast<GLsizei>(vertexBuffer->size());
    m_vertexbuffer_attrib = vertexBuffer->num_attrib();
}

template<typename T>
void vertexarray::attach(const handle<vertexbuffer<T>>& vertexBuffer)
{
    bind();
    vertexBuffer->bind();

    m_vertexbuffer_size = static_cast<GLsizei>(vertexBuffer->size());
    m_vertexbuffer_attrib = vertexBuffer->num_attrib();
}


}
