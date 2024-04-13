#pragma once

#include "model.h"
#include "detail/obj_model.h"
#include "viewer/core/log.h"

#include <tiny_obj_loader/tiny_obj_loader.h>
#include <glm/geometric.hpp>

namespace asset
{

template<typename Data, typename Material = std::monostate>
class model_loader
{
public:
    using result_type = std::shared_ptr<model<Data, Material>>;
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

        auto asset_model = std::make_shared<model<Data, Material>>();
        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        /*============= loading of materials =============*/
        if constexpr (!std::is_empty_v<Material>)
        {
            if(materials.empty())
            {
                platform_log(core::log::level::error, "Loaded Model not include material properties ({})", path.string());
            }
            else
            {
                /* create material groups */
                for(unsigned int i = 0; i < materials.size(); i++)
                {
                    auto& mat = asset_model->m_material_groups.emplace(std::make_pair(materials[i].name, material_group<Data, Material>(materials[i].name))).first->second;
                    detail::load_material(context, mat.m_material, materials[i], path);
                }
            }
        }


        /*============= load shapes and create buffers, and material groups =============*/
        for(auto& shape : shapes)
        {
            if(shape.mesh.num_face_vertices.size() > 0)
            {
                size_t num_faces = shape.mesh.num_face_vertices.size();
                std::unordered_map<unsigned int, unsigned int> materialGroups;
                std::vector<int> origIdx(num_faces * 3, 0);
                std::vector<glm::vec3> tangents(num_faces * 3, {0, 0, 0});
                std::vector<glm::vec3> bitangents(num_faces * 3, {0, 0, 0});

                std::vector<Data> vertices;
                vertices.resize(num_faces * 3);

                std::map<std::string, std::vector<unsigned int>> indices;
                std::map<std::string, glm::uvec2> indexoffset;

                size_t index_offset = 0;
                for(size_t f = 0; f < num_faces; f++)
                {
                    /* load vertices */
                    for(size_t v = 0; v < 3; v++)
                    {
                        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                        origIdx[index_offset + v] = idx.vertex_index;

                        auto& vertex = vertices[index_offset + v];

                        detail::load_vertex(vertex, attrib, idx, path);
                    }

                    /* assign material indices */
                    if constexpr (!std::is_empty_v<Material>)
                    {
                        unsigned int matIdx = shape.mesh.material_ids[f];
                        const auto& mat_name = materials[matIdx].name;

                        indices[mat_name].insert(indices[mat_name].end(),
                                                 {
                                                     static_cast<unsigned int>(index_offset + 0),
                                                     static_cast<unsigned int>(index_offset + 1),
                                                     static_cast<unsigned int>(index_offset + 2)
                                                 });
                    }

                    /* compute tangent space vectors */
                    if constexpr (detail::has_member<Data>::tangent::value)
                    {
                        if constexpr (! (detail::has_member<Data>::texcoord::value && detail::has_member<Data>::normal::value))
                        {
                            platform_log(core::log::level::error, "Tangent space calculation requires texture coordinates and normals ({})", path);
                            continue;
                        }

                        auto& v1 = vertices[index_offset + 0];
                        auto& v2 = vertices[index_offset + 1];
                        auto& v3 = vertices[index_offset + 2];

                        auto e1 = v2.position - v1.position;
                        auto e2 = v3.position - v1.position;
                        auto uv1 = v2.texcoord - v1.texcoord;
                        auto uv2 = v3.texcoord - v1.texcoord;

                        /* TODO: this needs a cleaner solution (what if uv coordinates are the same) */
                        if(length(uv1) < 1e-8f) { uv1.x = 1e-6f; }
                        if(length(uv2) < 1e-8f) { uv2.y = 1e-6f; }

                        float invDet = 1.0f / std::max(uv1.x * uv2.y - uv2.x * uv1.y, 1e-6f);

                        int idx_1 = shape.mesh.indices[index_offset + 0].vertex_index;
                        int idx_2 = shape.mesh.indices[index_offset + 1].vertex_index;
                        int idx_3 = shape.mesh.indices[index_offset + 2].vertex_index;
                        if constexpr (detail::has_member<Data>::tangent::value)
                        {
                            auto tangent = invDet * (uv2.y * e1 - uv1.y * e2);
                            tangents[idx_1] += tangent;
                            tangents[idx_2] += tangent;
                            tangents[idx_3] += tangent;

                            auto bitangent = invDet * (uv1.x * e2 - uv2.x * e1);
                            bitangents[idx_1] += bitangent;
                            bitangents[idx_2] += bitangent;
                            bitangents[idx_3] += bitangent;
                        }
                    }

                    index_offset += 3;
                }

                if constexpr (detail::has_member<Data>::tangent::value)
                {
                    for(unsigned int i = 0; i < vertices.size(); i++)
                    {
                        auto& vertex = vertices[i];
                        int idx = origIdx[i];
                        const auto& tangent = tangents[idx];
                        const auto& bitangent = tangents[idx];
                        const auto& normal = vertex.normal;
                        vertex.tangent = glm::vec4(glm::normalize(tangent - glm::dot(tangent, normal) * normal), 0.0f);
                        vertex.tangent.w = glm::dot(glm::cross(glm::vec3(tangent), bitangent), normal) > 0.0f ? 1.0f : -1.0f;
                    }
                }

                /* create opengl resources (TODO: cleanup; unecessary complicated handling material indices)*/
                std::vector<unsigned int> all_indices;
                for(const auto& [name, index_list] : indices)
                {
                    if(index_list.empty())
                    {
                        platform_log(core::log::level::warning, "Dropped unused material {} in file {}", shape.name, path.string());
                        continue;
                    }

                    indexoffset[name] =  glm::uvec2{all_indices.size(), index_list.size()};
                    all_indices.insert(all_indices.end(), index_list.begin(), index_list.end());
                }

                auto vao = context.make_vertexarray();
                auto vertexbuffer = context.make_vertexbuffer<Data>(vertices);
                auto indexbuffer = context.make_indexbuffer<unsigned int>(all_indices);
                auto& asset_mesh = asset_model->m_meshes.emplace(std::make_pair(shape.name, mesh<Data>(shape.name, vao, vertexbuffer, indexbuffer))).first->second;

                for(const auto& [name, offset] : indexoffset)
                {
                    auto it = asset_model->m_material_groups.find(name);
                    if(it == asset_model->m_material_groups.end())
                    {
                        platform_log(core::log::level::warning, "Unknown Material defined for object {} in file {}", shape.name, path.string());
                        continue;
                    }

                    auto& asset_material = (*it).second;
                    asset_material.m_records.emplace_back(asset_mesh, offset.x, offset.y);
                }
            }
            else if(shape.points.indices.size() > 0)
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
                asset_model->m_meshes.emplace(std::make_pair(shape.name, mesh<Data>(shape.name, vao, vertexbuffer)));
            }
        }

        return asset_model;
    }
};


}
