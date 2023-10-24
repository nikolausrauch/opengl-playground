#pragma once

#include "buffer.h"

#include <array>
#include <tuple>
#include <type_traits>

namespace opengl
{


/* TODO: over engineered */
/**
                    Attribute definition
    -------------------------------------------------
    (type, size, normalize, offset)

    type: render::eType::BYTE, render::eType::FLOAT, render::eType::UNSIGNED_INT, ...
    size: 1, 2, 3 (e.g. if float, vec2, vec3, in shader)
    mapping: opengl::buffer_mapping::cast (cast to float),
             opengl::buffer_mapping::normalized (cast to float and normalize),
             opengl::buffer_mapping::pure_data (use value directly)
    offset: offset to first vertex attribute in bytes
    -------------------------------------------------

    example:

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    !! TODO: seems over engineered. !!
    template<> struct gl::layout<Vertex>
                            {
                                static constexpr attr_info[] value =
                                {
                                    {gl::eType::FLOAT, 3, opengl::buffer_mapping::cast, offsetof(Vertex, pos)},
                                    {gl::eType::FLOAT, 2, opengl::buffer_mapping::cast, offsetof(Vertex, uv)};
                                }
                            };
*/

enum class buffer_mapping
{
    cast,
    normalized,
    pure_data
};

typedef const std::tuple<type, GLsizei, buffer_mapping, GLsizei> attr_info;

template<typename data>
struct layout { static const bool value = false; };

template <typename data>
class vertexbuffer : public buffer<data>
{
    typedef std::remove_reference_t<decltype(layout<data>::value)> info_type;
        static_assert (std::is_array_v<info_type>,
        "VertexBuffer doesn't have Attribute Definition (maybe you forgot to instantiate opengl::layout<data>{ static const attr_info value[] = {...} };)");

private:
    GLuint m_stride;

public:
    ~vertexbuffer();

    void bind();
    void unbind();

    unsigned int num_attrib() const;

private:
    vertexbuffer(context& context, buffer_usage usage = buffer_usage::static_draw);
    vertexbuffer(context& context, size_t numElements, buffer_usage usage = buffer_usage::static_draw);
    vertexbuffer(context& context, const data* items, size_t numElements, buffer_usage usage = buffer_usage::static_draw);
    vertexbuffer(context& context, std::initializer_list<data> items, buffer_usage usage = buffer_usage::static_draw);
    vertexbuffer(context& context, const std::vector<data>& items, buffer_usage usage = buffer_usage::static_draw);

    void computeStride();

    friend context;
};

template <typename data>
vertexbuffer<data>::vertexbuffer(context& context, buffer_usage usage)
    : buffer<data>(context, buffer_target::array, usage)
{
    computeStride();
}

template <typename data>
vertexbuffer<data>::vertexbuffer(context& context, size_t numElements, buffer_usage usage)
    : buffer<data>(context, buffer_target::array, numElements, usage)
{
    computeStride();
}

template <typename data>
vertexbuffer<data>::vertexbuffer(context& context, const data* items, size_t numElements, buffer_usage usage)
    : buffer<data>(context, buffer_target::array, items, numElements, usage)
{
    computeStride();
}

template <typename data>
vertexbuffer<data>::vertexbuffer(context& context, std::initializer_list<data> items, buffer_usage usage)
    : buffer<data>(context, buffer_target::array, items, usage)
{
    computeStride();
}

template <typename data>
vertexbuffer<data>::vertexbuffer(context& context, const std::vector<data>& items, buffer_usage usage)
    : buffer<data>(context, buffer_target::array, std::forward<const std::vector<data>>(items), usage)
{
    computeStride();
}

template<typename data>
void vertexbuffer<data>::bind()
{
    buffer<data>::bind();

    for(unsigned int i = 0; i < std::extent_v<info_type>; i++)
    {
        GLuint attribIdx = i;
        glEnableVertexAttribArray(attribIdx);

        auto mapping = std::get<2>(layout<data>::value[i]);

        if(mapping == buffer_mapping::cast)
        {
            glVertexAttribPointer(attribIdx,
                                  std::get<1>(layout<data>::value[i]),
                                  static_cast<GLenum>(std::get<0>(layout<data>::value[i])),
                                  GL_FALSE,
                                  m_stride, reinterpret_cast<GLvoid*>(std::get<3>(layout<data>::value[i])));
        }
        else if(mapping == buffer_mapping::normalized)
        {
            glVertexAttribPointer(attribIdx,
                                  std::get<1>(layout<data>::value[i]),
                                  static_cast<GLenum>(std::get<0>(layout<data>::value[i])),
                                  GL_TRUE,
                                  m_stride, reinterpret_cast<GLvoid*>(std::get<3>(layout<data>::value[i])));
        }
        else if(mapping == buffer_mapping::pure_data)
        {
            auto type = std::get<0>(layout<data>::value[i]);

            switch (type) {
            case type::byte_:
            case type::unsigned_byte_:
            case type::unsigned_short_:
            case type::unsigned_int_:
            case type::int_:
            {
                glVertexAttribIPointer(attribIdx,
                                      std::get<1>(layout<data>::value[i]),
                                      static_cast<GLenum>(type),
                                      m_stride, reinterpret_cast<GLvoid*>(std::get<3>(layout<data>::value[i])));
                break;
            }
            case type::fixed_:
            case type::half_float_:
            case type::float_:
            {
                glVertexAttribPointer(attribIdx,
                                      std::get<1>(layout<data>::value[i]),
                                      static_cast<GLenum>(type),
                                      GL_FALSE,
                                      m_stride, reinterpret_cast<GLvoid*>(std::get<3>(layout<data>::value[i])));
                break;
            }
            case type::double_:
            {
                glVertexAttribLPointer(attribIdx,
                                      std::get<1>(layout<data>::value[i]),
                                      static_cast<GLenum>(type),
                                      m_stride, reinterpret_cast<GLvoid*>(std::get<3>(layout<data>::value[i])));
                break;
            }
            default:
            {

            }
            }
        }
    }
}

template<typename data>
void vertexbuffer<data>::unbind()
{
    for(unsigned int i = 0; i < std::extent_v<info_type>; i++)
    {
        glDisableVertexAttribArray(i);
    }

    buffer<data>::unbind();
}

template<typename data>
unsigned int vertexbuffer<data>::num_attrib() const
{
    return std::extent_v<info_type>;
}

template<typename data>
vertexbuffer<data>::~vertexbuffer()
{

}

template<typename data>
void vertexbuffer<data>::computeStride()
{
    if(std::is_arithmetic_v<data>)
    {
        m_stride = 0;
        for(unsigned int i = 0; i < std::extent_v<info_type>; i++)
        {
            m_stride += std::get<1>(layout<data>::value[i]);
        }
    }
    else
    {
        m_stride = sizeof(data);
    }
}



}
