#pragma once

#include "model.h"
#include "detail/obj_model.h"

namespace asset
{

template<typename Data>
class shape
{
public:
    using result_type = std::shared_ptr<mesh<Data>>;
    static result_type create_screenquad(opengl::context& context)
    {
        static_assert(detail::has_member<Data>::position::value, "Vertex Type needs a position!");

        std::vector<Data> vertices(4);
        std::vector<unsigned int> indices(6);
        {
            vertices[0].position = {-1.0, -1.0, 0.0};
            vertices[1].position = { 1.0,  1.0, 0.0};
            vertices[2].position = { 1.0, -1.0, 0.0};
            vertices[3].position = {-1.0,  1.0, 0.0};

            indices[0] = 0; indices[1] = 1; indices[2] = 2;
            indices[3] = 0; indices[4] = 3; indices[5] = 1;
        }

        if constexpr (detail::has_member<Data>::texcoord::value)
        {
            vertices[0].texcoord = { 0.0,  0.0};
            vertices[1].texcoord = { 1.0,  1.0};
            vertices[2].texcoord = { 1.0,  0.0};
            vertices[3].texcoord = { 0.0,  1.0};
        }

        auto vao = context.make_vertexarray();
        auto vbo = context.make_vertexbuffer<Data>(vertices);
        auto ibo = context.make_indexbuffer<unsigned int>(indices);
        return std::make_shared<mesh<Data>>("full_screen_quad", vao, vbo, ibo);
    }
};

}
