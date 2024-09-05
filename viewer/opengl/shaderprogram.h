#pragma once

#include "context.h"

#include <array>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include <glm/glm.hpp>

namespace opengl
{

#define GLSL_CODE(version, shader)  "#version " #version "\n" #shader

enum class shader_type : GLenum
{
    vertex = GL_VERTEX_SHADER,
    fragment = GL_FRAGMENT_SHADER,
    geometry = GL_GEOMETRY_SHADER,
    tesselation_control = GL_TESS_CONTROL_SHADER,
    tesselation_eval = GL_TESS_EVALUATION_SHADER,
    compute = GL_COMPUTE_SHADER
};


class context;
class shader;

class shader_program
{    
private:
    context& m_context;
    GLuint m_handle;
    bool m_linked;

    std::unordered_map<std::string, unsigned int> m_uniform_location;
    std::array<std::unique_ptr<shader>, 6> m_shader;

public:
    ~shader_program();

    GLint uniform_location(const std::string& name) const;
    template <typename T> void uniform(const std::string& name, const T& value);
    template <typename T> void uniform(const std::string& name, const T& x, const T& y);
    template <typename T> void uniform(const std::string& name, const T& x, const T& y, const T& z);
    template <typename T> void uniform(const std::string& name, const T& x, const T& y, const T& z, const T& w);

    template <typename T, std::size_t S> void uniform(const std::string& name, const std::array<T, S>& value);

    void attach(const std::string& source, shader_type type);
    bool load(const std::string& path, shader_type type);

    void link();
    bool linked() const;

    void bind();
    void unbind();

    GLuint gl_handle() const;

private:
    shader_program(context& gl_context);

    shader_program(const shader_program&) = delete;
    shader_program& operator = (const shader_program&) = delete;

    void validate();
    void collect_uniforms();

    friend context;
};


namespace detail
{
    void uniform(GLint location, const glm::vec2& v);
    void uniform(GLint location, const glm::vec3& v);
    void uniform(GLint location, const glm::vec4& v);

    void uniform(GLint location, const glm::dvec2& v);
    void uniform(GLint location, const glm::dvec3& v);
    void uniform(GLint location, const glm::dvec4& v);

    void uniform(GLint location, const glm::ivec2& v);
    void uniform(GLint location, const glm::ivec3& v);
    void uniform(GLint location, const glm::ivec4& v);

    void uniform(GLint location, const glm::uvec2& v);
    void uniform(GLint location, const glm::uvec3& v);
    void uniform(GLint location, const glm::uvec4& v);

    void uniform(GLint location, const glm::bvec2& v);
    void uniform(GLint location, const glm::bvec3& v);
    void uniform(GLint location, const glm::bvec4& v);

    void uniform(GLint location, const glm::mat2& m);
    void uniform(GLint location, const glm::mat3& m);
    void uniform(GLint location, const glm::mat4& m);

    void uniform(GLint location, const glm::mat2x3& m);
    void uniform(GLint location, const glm::mat2x4& m);
    void uniform(GLint location, const glm::mat3x2& m);
    void uniform(GLint location, const glm::mat3x4& m);
    void uniform(GLint location, const glm::mat4x2& m);
    void uniform(GLint location, const glm::mat4x3& m);

    void uniform(GLint location, const glm::dmat2& m);
    void uniform(GLint location, const glm::dmat3& m);
    void uniform(GLint location, const glm::dmat4& m);

    void uniform(GLint location, const glm::dmat2x3& m);
    void uniform(GLint location, const glm::dmat2x4& m);
    void uniform(GLint location, const glm::dmat3x2& m);
    void uniform(GLint location, const glm::dmat3x4& m);
    void uniform(GLint location, const glm::dmat4x2& m);
    void uniform(GLint location, const glm::dmat4x3& m);

    void uniform(GLint location, GLfloat x);
    void uniform(GLint location, GLfloat x, GLfloat y);
    void uniform(GLint location, GLfloat x, GLfloat y, GLfloat z);
    void uniform(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

    void uniform(GLint location, GLdouble x);
    void uniform(GLint location, GLdouble x, GLdouble y);
    void uniform(GLint location, GLdouble x, GLdouble y, GLdouble z);
    void uniform(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);

    void uniform(GLint location, GLint x);
    void uniform(GLint location, GLint x, GLint y);
    void uniform(GLint location, GLint x, GLint y, GLint z);
    void uniform(GLint location, GLint x, GLint y, GLint z, GLint w);

    void uniform(GLint location, GLuint x);
    void uniform(GLint location, GLuint x, GLuint y);
    void uniform(GLint location, GLuint x, GLuint y, GLuint z);
    void uniform(GLint location, GLuint x, GLuint y, GLuint z, GLuint w);

    void uniform(GLint location, GLboolean x);
    void uniform(GLint location, GLboolean x, GLboolean y);
    void uniform(GLint location, GLboolean x, GLboolean y, GLboolean z);
    void uniform(GLint location, GLboolean x, GLboolean y, GLboolean z, GLboolean w);

    void uniform(GLint location, const glm::mat4& m, const std::size_t size);
}

template <typename T>
void shader_program::uniform(const std::string& name, const T& value)
{
    GLint location = uniform_location(name);
    detail::uniform(location, value);
}

template <typename T>
void shader_program::uniform(const std::string& name, const T& x, const T& y)
{
    GLint location = uniform_location(name);
    detail::uniform(location, x, y);
}

template <typename T>
void shader_program::uniform(const std::string& name, const T& x, const T& y, const T& z)
{
    GLint location = uniform_location(name);
    detail::uniform(location, x, y, z);
}

template <typename T>
void shader_program::uniform(const std::string& name, const T& x, const T& y, const T& z, const T& w)
{
    GLint location = uniform_location(name);
    detail::uniform(location, x, y, z, w);
}

template <typename T, std::size_t S>
void shader_program::uniform(const std::string& name, const std::array<T, S>& value)
{
    GLint location = uniform_location(name);
    detail::uniform(location, value[0], S);
}

}
