#pragma once

#include "context.h"

#include <initializer_list>
#include <stdexcept>
#include <vector>


namespace opengl
{

enum class buffer_access : GLenum
{
    read                = GL_MAP_READ_BIT,
    write               = GL_MAP_WRITE_BIT,
    persistent          = GL_MAP_PERSISTENT_BIT,
    coherent            = GL_MAP_COHERENT_BIT,
    invalidate_buffer   = GL_MAP_INVALIDATE_BUFFER_BIT,
    invalidate_range    = GL_MAP_INVALIDATE_RANGE_BIT,
    flush_explicit      = GL_MAP_FLUSH_EXPLICIT_BIT,
    unsynchronized      = GL_MAP_UNSYNCHRONIZED_BIT
};

enum class buffer_usage : GLenum
{
    stream_draw = GL_STREAM_DRAW,
    stream_read = GL_STREAM_READ,
    stream_copy = GL_STREAM_COPY,

    static_draw = GL_STATIC_DRAW,
    static_read = GL_STATIC_READ,
    static_copy = GL_STATIC_COPY,

    dynamic_draw = GL_DYNAMIC_DRAW,
    dynamic_read = GL_DYNAMIC_READ,
    dynamic_copy = GL_DYNAMIC_COPY
};

enum class buffer_target : GLenum
{
    array               = GL_ARRAY_BUFFER,
    copy_read			= GL_COPY_READ_BUFFER,
    copy_write			= GL_COPY_WRITE_BUFFER,
    element_array		= GL_ELEMENT_ARRAY_BUFFER
};


template <typename T>
class buffer
{
protected:
    context& m_context;
    buffer_target m_target;
    buffer_usage m_usage;

    GLuint m_handle;
    size_t m_num_items;

    buffer(context& context, buffer_target target, buffer_usage usage);
    buffer(context &context, buffer_target target, size_t number, buffer_usage usage);
    buffer(context &context, buffer_target target_, std::initializer_list<T> items, buffer_usage usage);
    buffer(context &context, buffer_target target_, const std::vector<T>& items, buffer_usage usage);
    buffer(context &context, buffer_target target_, const T* items, unsigned int numItems, buffer_usage usage);

public:
    virtual ~buffer();

    buffer(const buffer&) = delete;
    buffer& operator = (const buffer&) = delete;

    GLuint gl_handle() const;
    buffer_target target() const;
    buffer_usage usage() const;
    size_t size() const;
    bool mapped() const;

    void data(const T* items, unsigned int number);
    void data(std::initializer_list<T> items);
    void data(const std::vector<T>& items);
    void data(unsigned int offsetItems, const T* items, unsigned int numItems);
    void data(unsigned int offsetItems, std::initializer_list<T> items);
    void data(unsigned int offsetItems, const std::vector<T>& items);

    void bind();
    void unbind();
};


template <typename T>
buffer<T>::buffer(context &context, buffer_target target_, buffer_usage usage)
    : m_context(context), m_target(target_), m_usage(usage), m_handle(0), m_num_items(0)
{
    glGenBuffers(1, &m_handle);
    bind();
}

template<typename T>
buffer<T>::buffer(context &context, buffer_target target_, size_t number, buffer_usage usage)
    : m_context(context), m_target(target_), m_usage(usage), m_handle(0), m_num_items(number)
{
    GLenum target = static_cast<GLenum>(target_);
    size_t numBytes = number * sizeof(T);

    glGenBuffers(1, &m_handle);
    bind();
    glBufferData(target, static_cast<GLsizeiptr>(numBytes), nullptr, static_cast<GLenum>(usage));
}

template <typename T>
buffer<T>::buffer(context &context, buffer_target target_, std::initializer_list<T> items, buffer_usage usage)
    : m_context(context), m_target(target_), m_usage(usage), m_num_items(items.size())
{
    GLenum target = static_cast<GLenum>(target_);
    size_t numBytes = items.size() * sizeof(T);

    glGenBuffers(1, &m_handle);
    bind();
    glBufferData(target, numBytes, items.begin(), static_cast<GLenum>(usage));
}

template <typename T>
buffer<T>::buffer(context &context, buffer_target target_, const std::vector<T> &items, buffer_usage usage)
    : m_context(context), m_target(target_), m_usage(usage), m_num_items(items.size())
{
    GLenum target = static_cast<GLenum>(target_);
    size_t numBytes = items.size() * sizeof(T);

    glGenBuffers(1, &m_handle);
    bind();
    glBufferData(target, numBytes, &(items[0]), static_cast<GLenum>(usage));
}

template<typename T>
buffer<T>::buffer(context &context, buffer_target target_, const T *items, unsigned int numItems, buffer_usage usage)
    : m_context(context), m_target(target_), m_usage(usage), m_num_items(numItems)
{
    GLenum target = static_cast<GLenum>(target_);
    size_t numBytes = numItems * sizeof(T);

    glGenBuffers(1, &m_handle);
    bind();
    glBufferData(target, numBytes, items, static_cast<GLenum>(usage));
}

template <typename T>
buffer<T>::~buffer()
{
    if(m_handle)
    {
        glDeleteBuffers(1, &m_handle);
    }
}

template <typename T>
GLuint buffer<T>::gl_handle() const
{
    return m_handle;
}

template <typename T>
buffer_target buffer<T>::target() const
{
    return m_target;
}

template <typename T>
buffer_usage buffer<T>::usage() const
{
    return m_usage;
}

template <typename T>
size_t buffer<T>::size() const
{
    return m_num_items;
}

template <typename T>
void buffer<T>::data(const T* items, unsigned int number)
{
    size_t numBytes = number * sizeof(T);
    m_num_items = number;

    bind();
    glBufferData(static_cast<GLenum>(m_target), numBytes, items, static_cast<GLenum>(m_usage));
}

template <typename T>
void buffer<T>::data(std::initializer_list<T> items)
{
    size_t numBytes = items.size() * sizeof(T);
    m_num_items = items.size();

    bind();
    glBufferData(static_cast<GLenum>(m_target), numBytes, items.begin(), static_cast<GLenum>(m_usage));
}

template <typename T>
void buffer<T>::data(const std::vector<T>& items)
{
    size_t numBytes = items.size() * sizeof(T);
    m_num_items = items.size();

    bind();
    glBufferData(static_cast<GLenum>(m_target), numBytes, items.data(), static_cast<GLenum>(m_usage));
}

template <typename T>
void buffer<T>::data(unsigned int offsetItems, const T* items, unsigned int numItems)
{
    size_t offsetBytes = offsetItems * sizeof(T);
    size_t numBytes = numItems * sizeof(T);

    bind();
    glBufferSubData(static_cast<GLenum>(m_target), offsetBytes, numBytes, items);
}

template <typename T>
void buffer<T>::data(unsigned int offsetItems, std::initializer_list<T> items)
{
    size_t offsetBytes = offsetItems * sizeof(T);
    size_t numBytes = items.size() * sizeof(T);

    bind();
    glBufferSubData(static_cast<GLenum>(m_target), offsetBytes, numBytes, items.begin());
}

template <typename T>
void buffer<T>::data(unsigned int offsetItems, const std::vector<T>& items)
{
    size_t offsetBytes = offsetItems * sizeof(T);
    size_t numBytes = items.size() * sizeof(T);

    bind();
    glBufferSubData(static_cast<GLenum>(m_target), offsetBytes, numBytes, items.data());
}

template <typename T>
void buffer<T>::bind()
{
    m_context.bind_buffer(static_cast<GLenum>(m_target), m_handle);
}

template<typename T>
void buffer<T>::unbind()
{
    m_context.bind_buffer(static_cast<GLenum>(m_target), 0);
}

}
