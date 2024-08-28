#pragma once

#include "model.h"
#include "viewer/asset/detail/obj_model.h"
#include "viewer/core/log.h"

#include <tiny_obj_loader/tiny_obj_loader.h>
#include <tiny_ply/source/tinyply.h>

namespace asset
{

/*========================== pointcloud ==========================*/
template<typename Data>
class pointcloud
{
    using VertexType = Data;

private:
    opengl::handle<opengl::vertexarray> m_vao;
    opengl::handle<opengl::vertexbuffer<VertexType>> m_vertexbuffer;

public:
    pointcloud(
         const opengl::handle<opengl::vertexarray>& vao,
         const opengl::handle<opengl::vertexbuffer<VertexType>>& vb)
        : m_vao(vao), m_vertexbuffer(vb)
    {
        m_vao->attach(m_vertexbuffer);
        m_vao->unbind();
    }

    opengl::handle<opengl::vertexarray> vao() const { return m_vao; }
};

template<typename Data>
class pointcloud_loader
{
public:
    using result_type = std::shared_ptr<pointcloud<Data>>;
    static result_type load_obj(opengl::context& context, const std::filesystem::path& path)
    {
        if(!std::filesystem::exists(path))
        {
            platform_log(core::log::level::error, "Cannot find obj file {}", path.string());
            return nullptr;
        }

        tinyobj::ObjReaderConfig config;
        config.triangulate = true;

        tinyobj::ObjReader reader;

        if(!reader.ParseFromFile(path.string(), config))
        {
            if(!reader.Error().empty())
            {
                platform_log(core::log::level::error, "{}", reader.Error());
            }

            return nullptr;
        }

        if(!reader.Warning().empty())
        {
            platform_log(core::log::level::warning, "{}", reader.Warning());
        }

        auto& attrib = reader.GetAttrib();

        static_assert(detail::has_member<Data>::position::value, "Vertex Type needs a position!");
        if constexpr (detail::has_member<Data>::color::value)
        {
            if(attrib.colors.empty())
            {
                platform_log(core::log::level::warning, "Loaded model does include vertex color values, but was requested ( {} )", path.string());
            }
        }

        if(attrib.vertices.size() > 0)
        {
            std::vector<Data> vertices;
            vertices.resize(attrib.vertices.size() / 3);

            for(unsigned int p = 0; p < vertices.size(); p++)
            {
                static_assert(detail::has_member<Data>::position::value, "Vertex Type needs a position!");

                auto& vertex = vertices[p];
                vertex.position =
                    {
                        attrib.vertices[3*p + 0],
                        attrib.vertices[3*p + 1],
                        attrib.vertices[3*p + 2]
                    };

                if constexpr (detail::has_member<Data>::color::value)
                {
                    if(attrib.colors.size() < attrib.vertices.size())
                    {
                        continue;
                    }

                    vertex.color =
                        {
                            attrib.colors[3*p + 0],
                            attrib.colors[3*p + 1],
                            attrib.colors[3*p + 2]
                        };
                }
            }

            auto vao = context.make_vertexarray();
            auto vertexbuffer = context.make_vertexbuffer<Data>(vertices);

            auto asset_cloud = std::make_shared<pointcloud<Data>>(vao, vertexbuffer);
            return asset_cloud;
        }

        platform_log(core::log::level::error, "{} does not contain vertices!", path.string());
        return nullptr;

        /*** unsupported by tinyobj? ***/
        /*
        for(auto& shape : shapes)
        {
            if(shape.points.indices.size() > 0)
            {
                size_t num_points = shape.points.indices.size();

                std::vector<Data> vertices;
                vertices.resize(num_points);

                for(unsigned int p = 0; p < num_points; p++)
                {
                    tinyobj::index_t idx = shape.points.indices[p];
                    detail::load_vertex(vertices[p], attrib, idx, path);
                }

                auto vao = context.make_vertexarray();
                auto vertexbuffer = context.make_vertexbuffer<Data>(vertices);
            }
        }
        */
    }

    static result_type load_ply(opengl::context& context, const std::filesystem::path& path)
    {
        if(!std::filesystem::exists(path))
        {
            platform_log(core::log::level::error, "Cannot find ply file {}", path.string());
            return nullptr;
        }

        std::ifstream file(path.string(), std::ios::binary);
        if (file.fail())
        {
            platform_log(core::log::level::error, "Couldn't open file {}", path.string());
            return nullptr;
        }


        tinyply::PlyFile ply_file;
        ply_file.parse_header(file);

        std::shared_ptr<tinyply::PlyData> vertices, colors;
        try { vertices = ply_file.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { platform_log(core::log::level::error, "{}", e.what()); return nullptr; }

        if constexpr (detail::has_member<Data>::color::value)
        {
            try { colors = ply_file.request_properties_from_element("vertex", { "red", "green", "blue" }); }
            catch (const std::exception & e) { platform_log(core::log::level::error, "{}", e.what()); return nullptr; }
        }

        ply_file.read(file);


        if(vertices->count > 0)
        {
            /*** load to custom structure (TODO: probably a better approach to this) ***/
            std::vector<Data> verts_data(vertices->count);
            for(unsigned int p = 0; p < verts_data.size(); p++)
            {
                auto& vertex = verts_data[p];

                if (vertices->t == tinyply::Type::FLOAT32)
                {
                    vertex.position =
                    {
                        ((const float*)vertices->buffer.get())[p*3 + 0],
                        ((const float*)vertices->buffer.get())[p*3 + 1],
                        ((const float*)vertices->buffer.get())[p*3 + 2]
                    };
                }
                else if(vertices->t == tinyply::Type::FLOAT64)
                {
                    vertex.position =
                        {
                            ((const double*)vertices->buffer.get())[p*3 + 0],
                            ((const double*)vertices->buffer.get())[p*3 + 1],
                            ((const double*)vertices->buffer.get())[p*3 + 2]
                        };
                }

                if constexpr (detail::has_member<Data>::color::value)
                {
                    if(colors->t == tinyply::Type::UINT8)
                    {
                        vertex.color =
                        {
                            ((const uint8_t*)colors->buffer.get())[p*3 + 0],
                            ((const uint8_t*)colors->buffer.get())[p*3 + 1],
                            ((const uint8_t*)colors->buffer.get())[p*3 + 2]
                        };
                        vertex.color /= 255.0f;
                    }

                    if(colors->t == tinyply::Type::FLOAT32)
                    {
                        vertex.color =
                            {
                                ((const float*)colors->buffer.get())[p*3 + 0],
                                ((const float*)colors->buffer.get())[p*3 + 1],
                                ((const float*)colors->buffer.get())[p*3 + 2]
                            };
                    }
                }
            }

            auto vao = context.make_vertexarray();
            auto vertexbuffer = context.make_vertexbuffer<Data>(verts_data);

            auto asset_cloud = std::make_shared<pointcloud<Data>>(vao, vertexbuffer);
            return asset_cloud;
        }

        return nullptr;
    }
};

}
