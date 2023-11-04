#pragma once

#include "viewer/asset/model.h"
#include "viewer/asset/texture.h"
#include "viewer/asset/image.h"
#include "viewer/core/log.h"


#include <filesystem>

#include <tiny_obj_loader/tiny_obj_loader.h>

namespace asset::detail
{

template<typename T>
struct has_member
{
    typedef char one;
    struct two { char _x[2]; };

#define test_one(name) template<typename C> static one test_##name( decltype (&C::name) );
#define test_two(name) template<typename C> static two test_##name(...);
#define test_value(name) struct name{ enum { value = sizeof( test_##name<T>(0) ) == sizeof(char) }; };
#define property(name) test_one(name) test_two(name) test_value(name)

    /* vertex members */
    property(position)
    property(normal)
    property(texcoord)
    property(color)
    property(tangent)

    /* material members */
    property(name);
    property(ambient);
    property(diffuse);
    property(specular);
    property(transmittance);
    property(emission);
    property(shininess);
    property(ior);
    property(dissolve);

    property(map_ambient);
    property(map_diffuse);
    property(map_specular);
    property(map_specular_highlight);
    property(map_bump);
    property(map_displacement);

    /* pbr materials */
    property(roughness);
    property(metallic);
    property(sheen);
    property(clearcoat_thickness);
    property(clearcoat_roughness);
    property(anisotropy);
    property(anisotropy_rotation);

    property(map_albedo);
    property(map_roughness);
    property(map_metallic);
    property(map_normal);

    property(map_metallic_roughness);

#undef test_one
#undef test_two
#undef test_value
#undef property
};


template<typename VertexType>
void load_vertex(VertexType& vertex, const tinyobj::attrib_t& attrib, const tinyobj::index_t& idx, const std::filesystem::path& filepath)
{
    static_assert(detail::has_member<VertexType>::position::value, "Vertex Type needs a position!");
    {
        if(attrib.vertices.empty())
        {
            platform_log(core::log::level::error, "Loaded model does not include vertex coordinates? ( {} )", filepath.string());
            return;
        }

        vertex.position =
        {
            attrib.vertices[3*size_t(idx.vertex_index) + 0],
            attrib.vertices[3*size_t(idx.vertex_index) + 1],
            attrib.vertices[3*size_t(idx.vertex_index) + 2]
        };
    }

    if constexpr (detail::has_member<VertexType>::normal::value)
    {
        if(idx.normal_index < 0 || attrib.normals.empty())
        {
            platform_log(core::log::level::warning, "Loaded model does not include normals, but was requested ( {} )", filepath.string());
            return;
        }

        vertex.normal =
        {
            attrib.normals[3*size_t(idx.normal_index) + 0],
            attrib.normals[3*size_t(idx.normal_index) + 1],
            attrib.normals[3*size_t(idx.normal_index) + 2]
        };
    }

    if constexpr (detail::has_member<VertexType>::texcoord::value)
    {
        if(idx.texcoord_index < 0 || attrib.texcoords.empty())
        {
            platform_log(core::log::level::warning, "Loaded model does not texture coordinates, but was requested ( {} )", filepath.string());
            return;
        }

        vertex.texcoord =
        {
            attrib.texcoords[2*size_t(idx.texcoord_index) + 0],
            attrib.texcoords[2*size_t(idx.texcoord_index) + 1]
        };
    }

    if constexpr (detail::has_member<VertexType>::color::value)
    {
        if(attrib.colors.empty())
        {
            platform_log(core::log::level::warning, "Loaded model does include vertex color values, but was requested ( {} )", filepath.string());
            return;
        }

        vertex.color =
        {
            attrib.colors[3*size_t(idx.vertex_index) + 0],
            attrib.colors[3*size_t(idx.vertex_index) + 1],
            attrib.colors[3*size_t(idx.vertex_index) + 2]
        };
    }
}

void load_material_texture(auto& context, auto& member, const std::string& tex_name, const auto& default_value, const std::string& map_name, const std::string& material_name, const std::filesystem::path& filepath)
{
    if(tex_name.empty())
    {
        platform_log(core::log::level::error, "{} missing for material {} in {}", map_name, material_name, filepath.string());
        member = std::dynamic_pointer_cast<opengl::texture>(texture_loader::load(context, 1, 1, default_value));
        platform_log(core::log::level::warning, "default {} loaded for material {}", map_name, material_name);
    }
    else
    {
        auto path = filepath.parent_path();
        path /= tex_name;
        member = std::dynamic_pointer_cast<opengl::texture>(texture_loader::load(context, path));
    }
}

template<typename MaterialType>
void load_material(auto& context, MaterialType& material, const tinyobj::material_t& loaded, const std::filesystem::path& filepath)
{
    if constexpr(detail::has_member<MaterialType>::name::value)
    {
        material.name = loaded.name;
    }


    /*----------------- obj standard -------------------*/
    if constexpr(detail::has_member<MaterialType>::ambient::value)
    {
        material.ambient = { loaded.ambient[0], loaded.ambient[1], loaded.ambient[2] };
    }

    if constexpr(detail::has_member<MaterialType>::diffuse::value)
    {
        material.diffuse = { loaded.diffuse[0], loaded.diffuse[1], loaded.diffuse[2] };
    }

    if constexpr(detail::has_member<MaterialType>::specular::value)
    {
        material.specular = { loaded.specular[0], loaded.specular[1], loaded.specular[2] };
    }

    if constexpr(detail::has_member<MaterialType>::transmittance::value)
    {
        material.transmittance = { loaded.transmittance[0], loaded.transmittance[1], loaded.transmittance[2] };
    }

    if constexpr(detail::has_member<MaterialType>::emission::value)
    {
        material.emission = { loaded.emission[0], loaded.emission[1], loaded.emission[2] };
    }

    if constexpr(detail::has_member<MaterialType>::shininess::value)
    {
        material.shininess = loaded.shininess;
    }

    if constexpr(detail::has_member<MaterialType>::ior::value)
    {
        material.ior = loaded.ior;
    }

    if constexpr(detail::has_member<MaterialType>::dissolve::value)
    {
        material.dissolve = loaded.dissolve;
    }

    if constexpr(detail::has_member<MaterialType>::map_ambient::value)
    {
        load_material_texture(context, material.map_ambient, loaded.ambient_texname, color{0, 0, 0, 255}, "ambient map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_diffuse::value)
    {
        load_material_texture(context, material.map_diffuse, loaded.diffuse_texname, color{0, 0, 0, 255}, "diffuse map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_specular::value)
    {
        load_material_texture(context, material.map_specular, loaded.specular_texname, color{0, 0, 0, 255}, "specular map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_specular_highlight::value)
    {
        load_material_texture(context, material.map_specular_highlight, loaded.specular_highlight_texname, color{0, 0, 0, 255}, "specular highlight map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_bump::value)
    {
        load_material_texture(context, material.map_bump, loaded.bump_texname, color{0, 0, 0, 255}, "bump map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_displacement::value)
    {
        load_material_texture(context, material.map_displacement, loaded.displacement_texname, color{0, 0, 0, 255}, "displacement map", loaded.name, filepath);
    }


    /*----------------- pbr extension -------------------*/
    if constexpr(detail::has_member<MaterialType>::roughness::value)
    {
        material.roughness = loaded.roughness;
    }

    if constexpr(detail::has_member<MaterialType>::metallic::value)
    {
        material.metallic = loaded.metallic;
    }

    if constexpr(detail::has_member<MaterialType>::sheen::value)
    {
        material.sheen = loaded.sheen;
    }

    if constexpr(detail::has_member<MaterialType>::clearcoat_thickness::value)
    {
        material.clearcoat_thickness = loaded.clearcoat_thickness;
    }

    if constexpr(detail::has_member<MaterialType>::clearcoat_roughness::value)
    {
        material.clearcoat_roughness = loaded.clearcoat_roughness;
    }

    if constexpr(detail::has_member<MaterialType>::anisotropy::value)
    {
        material.anisotropy = loaded.anisotropy;
    }

    if constexpr(detail::has_member<MaterialType>::anisotropy_rotation::value)
    {
        material.anisotropy_rotation = loaded.anisotropy_rotation;
    }

    if constexpr(detail::has_member<MaterialType>::map_albedo::value)
    {
        load_material_texture(context, material.map_albedo, loaded.diffuse_texname, color{0, 0, 0, 255}, "albedo/diffuse map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_metallic::value)
    {
        load_material_texture(context, material.map_metallic, loaded.metallic_texname, color{0, 0, 0, 255}, "metallic map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_roughness::value)
    {
        load_material_texture(context, material.map_roughness, loaded.roughness_texname, color{0, 0, 0, 255}, "roughness map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_normal::value)
    {
        load_material_texture(context, material.map_normal, loaded.normal_texname, color{0, 0, 0, 255}, "normal map", loaded.name, filepath);
    }

    if constexpr(detail::has_member<MaterialType>::map_metallic_roughness::value)
    {
        if(loaded.metallic_texname.empty())
        {
            platform_log(core::log::level::error, "metallic map missing for material {} in {}", loaded.name, filepath.string());

            if(loaded.roughness_texname.empty())
            {
                platform_log(core::log::level::error, "roughness map missing for material {} in {}", loaded.name, filepath.string());
                material.map_metallic_roughness = texture_loader::load(context, 1, 1, color{0, 0, 0, 255});
            }
            else
            {
                platform_log(core::log::level::info, "assume roughness metallic map is stored in roughness map {} in {}", loaded.name, filepath.string());
                load_material_texture(context, material.map_metallic_roughness, loaded.roughness_texname, color{0, 0, 0, 255}, "roughness map", loaded.name, filepath);
            }
        }
        else if(!loaded.metallic_texname.empty() && loaded.roughness_texname.empty())
        {
            platform_log(core::log::level::error, "metallic map missing for material {} in {}", loaded.name, filepath.string());
            load_material_texture(context, material.map_metallic_roughness, loaded.metallic_texname, color{0, 0, 0, 255}, "metallic map", loaded.name, filepath);
        }
        else if(!loaded.metallic_texname.empty() && !loaded.roughness_texname.empty())
        {
            auto map_roughness = image_loader::load(loaded.roughness_texname);
            auto map_metallic = image_loader::load(loaded.metallic_texname);

            if(map_metallic->size() != map_roughness->size())
            {
                platform_log(core::log::level::error, "metallic and roughness maps are not of the same dimensions (required if merging is forced)");
                material.map_metallic_roughness = texture_loader::load(context, 1, 1, color{0, 0, 0, 255});
            }
            else
            {
                /* merge to single texture */
                image map_metallic_roughness(map_metallic->size().x, map_metallic->size().y, color{0, 0, 0, 255});
                for(int j = 0; j < map_metallic_roughness.size().y; j++)
                {
                    for(int i = 0; i < map_metallic_roughness.size().x; i++)
                    {
                        map_metallic_roughness(i, j) = { 0, (*map_roughness)(i,j).x, (*map_metallic)(i,j).x, 255 };
                    }
                }

                material.map_metallic_roughness = texture_loader::load(context, map_metallic_roughness);
            }
        }
    }
}

}
