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


    static result_type create_unitcube(opengl::context& context)
    {
        static_assert(detail::has_member<Data>::position::value, "Vertex Type needs a position!");

        std::vector<Data> vertices(8);
        {
            vertices[0].position = {-0.5, -0.5,  0.5};
            vertices[1].position = { 0.5, -0.5,  0.5};
            vertices[2].position = { 0.5,  0.5,  0.5};
            vertices[3].position = {-0.5,  0.5,  0.5};
            vertices[4].position = {-0.5, -0.5, -0.5};
            vertices[5].position = { 0.5, -0.5, -0.5};
            vertices[6].position = { 0.5,  0.5, -0.5};
            vertices[7].position = {-0.5,  0.5, -0.5};
        }

        std::vector<unsigned int> indices =
        {
            0, 1, 2,
            0, 2, 3,

            1, 5, 6,
            1, 6, 2,

            5, 4, 7,
            5, 7, 6,

            4, 0, 3,
            4, 3, 7,

            2, 6, 7,
            2, 7, 3,

            4, 5, 1,
            4, 1, 0
        };

        auto vao = context.make_vertexarray();
        auto vbo = context.make_vertexbuffer<Data>(vertices);
        auto ibo = context.make_indexbuffer<unsigned int>(indices);
        return std::make_shared<mesh<Data>>("unit_cube", vao, vbo, ibo);
    }


    static result_type create_pyramid(opengl::context& context)
    {
        static_assert(detail::has_member<Data>::position::value, "Vertex Type needs a position!");

        std::vector<Data> vertices(18);
        {
            vertices[0].position = {0.0, 0.0, 0.0};
            vertices[1].position = {  std::sqrt(2.0f), -1.0, -std::sqrt(2.0f)};
            vertices[2].position = { -std::sqrt(2.0f), -1.0, -std::sqrt(2.0f)};

            vertices[3].position = {0.0, 0.0, 0.0};
            vertices[4].position = { std::sqrt(2.0f), -1.0,  std::sqrt(2.0f)};
            vertices[5].position = { std::sqrt(2.0f), -1.0, -std::sqrt(2.0f)};

            vertices[6].position = {0.0, 0.0, 0.0};
            vertices[7].position = { -std::sqrt(2.0f), -1.0,  std::sqrt(2.0f)};
            vertices[8].position = {  std::sqrt(2.0f), -1.0,  std::sqrt(2.0f)};

            vertices[9].position  = {0.0, 0.0, 0.0};
            vertices[10].position = { -std::sqrt(2.0f), -1.0,  -std::sqrt(2.0f)};
            vertices[11].position = { -std::sqrt(2.0f), -1.0,  std::sqrt(2.0f)};


            vertices[12].position = { -std::sqrt(2.0f), -1.0, -std::sqrt(2.0f)};
            vertices[13].position = {  std::sqrt(2.0f), -1.0, -std::sqrt(2.0f)};
            vertices[14].position = {  std::sqrt(2.0f), -1.0,  std::sqrt(2.0f)};

            vertices[15].position = { -std::sqrt(2.0f), -1.0, -std::sqrt(2.0f)};
            vertices[16].position = {  std::sqrt(2.0f), -1.0,  std::sqrt(2.0f)};
            vertices[17].position = { -std::sqrt(2.0f), -1.0,  std::sqrt(2.0f)};
        }

        auto vao = context.make_vertexarray();
        auto vbo = context.make_vertexbuffer<Data>(vertices);
        return std::make_shared<mesh<Data>>("unit_pyramid", vao, vbo);
    }

};

}
