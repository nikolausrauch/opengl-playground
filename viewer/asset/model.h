#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <unordered_map>
#include <type_traits>
#include <filesystem>
#include <variant>

#include "viewer/opengl/vertexarray.h"
#include "viewer/opengl/vertexbuffer.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace asset
{

/*============= All possible Members for Vertex and Material =============*/
struct _all_material
{
    std::string name;

    /* base material properties */
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 transmittance;
    glm::vec3 emission;
    float shininess;

    float ior;
    float dissolve;

    opengl::handle<opengl::texture> map_ambient;
    opengl::handle<opengl::texture> map_diffuse;
    opengl::handle<opengl::texture> map_specular;
    opengl::handle<opengl::texture> map_specular_highlight;
    opengl::handle<opengl::texture> map_bump;
    opengl::handle<opengl::texture> map_displacement;

    /* pbr material properties */
    float roughness;
    float metallic;
    float sheen;
    float clearcoat_thickness;
    float clearcoat_roughness;
    float anisotropy;
    float anisotropy_rotation;

    opengl::handle<opengl::texture> map_albedo;
    opengl::handle<opengl::texture> map_metallic;
    opengl::handle<opengl::texture> map_roughness;
    opengl::handle<opengl::texture> map_normal;

    /* force combined metallic and roughness (map_Pm in blue and map_Pr in red) */
    opengl::handle<opengl::texture> map_metallic_roughness;
};

struct _all_vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec4 tangent;
    glm::vec3 color;
};


/*========================== Mesh ==========================*/
template<typename Data, typename Material> class model_loader;

template<typename Data>
class mesh
{
    using IndexType = unsigned int;
    using VertexType = Data;

private:
    const std::string m_name;
    opengl::handle<opengl::vertexarray> m_vao;
    opengl::handle<opengl::vertexbuffer<VertexType>> m_vertexbuffer;
    opengl::handle<opengl::indexbuffer<IndexType>> m_indexbuffer;

public:
    mesh(const std::string& name,
         const opengl::handle<opengl::vertexarray>& vao,
         const opengl::handle<opengl::vertexbuffer<VertexType>>& vb,
         const opengl::handle<opengl::indexbuffer<IndexType>>& ib)
        : m_name(name), m_vao(vao), m_vertexbuffer(vb), m_indexbuffer(ib)
    {
        m_vao->attach(m_vertexbuffer);
        m_vao->attach(m_indexbuffer);
        m_vao->unbind();
    }

    mesh(const std::string& name,
         const opengl::handle<opengl::vertexarray>& vao,
         const opengl::handle<opengl::vertexbuffer<VertexType>>& vb)
        : m_name(name), m_vao(vao), m_vertexbuffer(vb)
    {
        m_vao->attach(m_vertexbuffer);
        m_vao->unbind();
    }

    opengl::handle<opengl::vertexarray> vao() const { return m_vao; }
};

/*========================== Material ==========================*/
template<typename Data, typename Material>
class material_group
{
public:
    struct record
    {
        using IndexType = unsigned int;
        mesh<Data>& m_mesh;
        IndexType m_offset;
        IndexType m_count;
    };

private:
    const std::string m_name;
    Material m_material;
    std::list<record> m_records;

public:
    material_group(const std::string& name) : m_name(name) {}
    const std::string& name() const { return m_name; }
    const Material& material() const { return m_material; };
    const std::list<record>& records() const { return m_records; }

    friend class model_loader<Data, Material>;
};

/*========================== Model ==========================*/
template<typename Data, typename Material = std::monostate>
class model
{
    using VertexType = Data;
    using MaterialType = Material;
    template<typename T> using has_material_v = std::enable_if_t<!std::is_empty_v<T>>;

private:
    std::map<std::string, material_group<VertexType, MaterialType>> m_material_groups;
    std::map<std::string, mesh<VertexType>> m_meshes;

public:
    template<typename T = MaterialType, typename = has_material_v<T>> const auto& material_groups() const { return m_material_groups; }
    const auto& meshes() const { return m_meshes; }

    friend class model_loader<Data, Material>;
};

}
