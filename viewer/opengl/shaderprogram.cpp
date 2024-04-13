#include "shaderprogram.h"

#include "viewer/core/assert.h"
#include "context.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace opengl
{

namespace detail
{

const int shader_index(shader_type type)
{
    switch (type)
    {
    case shader_type::vertex:   return 0;
    case shader_type::fragment: return 1;
    case shader_type::compute:  return 2;
    case shader_type::geometry: return 3;
    case shader_type::tesselation_control: return 4;
    case shader_type::tesselation_eval: return 5;
    }

    platform_assert(false, "unknown shader type");
    return 0;
}

}

class shader
{
private:
    shader_type m_type;
    GLuint m_handle;

public:
    shader(shader_type type)
        : m_type(type), m_handle(0)
    {
        m_handle = glCreateShader(static_cast<GLenum>(type));

        platform_assert(m_handle, "Invalid OpenGL handle");
        if(!m_handle) { platform_log(core::log::level::error, "Failed to create shader"); }
    }

    ~shader()
    {
        if(m_handle)
        {
            glDeleteShader(m_handle);
        }
    }

    shader(const shader&) = delete;
    shader& operator = (const shader&) = delete;

    GLuint handle() const { return m_handle; }

    void compile(const std::string& source)
    {
        GLint compile_result = 0;

        const char* shader_code = source.c_str();
        const int shader_code_size = static_cast<int>(source.size());

        glShaderSource(m_handle, 1, &shader_code, &shader_code_size);
        glCompileShader(m_handle);
        glGetShaderiv(m_handle, GL_COMPILE_STATUS, &compile_result);

        if(compile_result == GL_FALSE)
        {
            GLint info_length = 0;
            std::string shader_log;

            glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &info_length);
            shader_log.resize(static_cast<std::size_t>(info_length));
            glGetShaderInfoLog(m_handle, info_length, nullptr, &shader_log[0]);

            platform_log(core::log::level::error, "ERROR compiling shader: {0}", shader_log);
        }
    }

    std::string source() const
    {
        GLsizei num_bytes = 0;
        glGetShaderiv(m_handle, GL_SHADER_SOURCE_LENGTH, &num_bytes);

        std::string shader_source;
        shader_source.resize(static_cast<std::size_t>(num_bytes));
        GLsizei bytesWritten = 0;
        glGetShaderSource(m_handle, num_bytes, &bytesWritten, &shader_source[0]);

        return shader_source;
    }
};


GLint shader_program::uniform_location(const std::string &name) const
{
    auto it = m_uniform_location.find(name);

    if (it == m_uniform_location.end())
    {
        platform_log(core::log::level::warning, "Cannot locate uniform {0}", name);
        return -1;
    }

    return it->second;
}

shader_program::shader_program(context& gl_context)
    : m_context(gl_context), m_handle(glCreateProgram()), m_linked(false)
{
    platform_assert(m_handle, "Invalid shaderprogram handle");
}

shader_program::~shader_program()
{
    if(m_handle)
    {
        for(const auto& shader : m_shader)
        {
            if(shader)
            {
                glDetachShader(m_handle, shader->handle());
            }
        }

        glDeleteProgram(m_handle);
    }
}

void shader_program::attach(const std::string &source, shader_type type)
{
    int idx = detail::shader_index(type);

    m_shader[idx] = std::make_unique<shader>(type);
    m_shader[idx]->compile(source);
    glAttachShader(m_handle, m_shader[idx]->handle());
}

bool shader_program::load(const std::string& path, shader_type type)
{
    std::ifstream fileStream(path.c_str());

    if(!fileStream.good())
    {
        platform_log(core::log::level::error, "Couldn't open shader file : {0}", path);
        return false;
    }

    std::string source( (std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>() );
    attach(source, type);

    return true;
}

void shader_program::link()
{
    platform_assert(!m_linked, "Shaderprogram already linked");
    platform_assert(m_handle, "Invalid Shaderprogram handle");

    glLinkProgram(m_handle);

    GLint result;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &result);

    if(result == GL_FALSE)
    {
        GLint msg_length = 0;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &msg_length);

        if(msg_length > 0)
        {
            std::string programLog;
            programLog.resize(static_cast<std::size_t>(msg_length));
            glGetProgramInfoLog(m_handle, msg_length, nullptr, &programLog[0]);

            platform_log(core::log::level::error, "ERROR link shader: {0}", programLog);
        }

        return;
    }

    m_linked = true;

    validate();
    collect_uniforms();
}

bool shader_program::linked() const
{
    return m_linked;
}

void shader_program::bind()
{
    m_context.bind_shader(m_handle);
}

void shader_program::unbind()
{
    m_context.bind_shader(0);
}

void shader_program::validate()
{
    platform_assert(m_linked, "Shaderprogram is not linked");

    glValidateProgram(m_handle);

    GLint result;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &result);

    if(result == GL_FALSE)
    {
        GLint msg_length = 0;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &msg_length);

        if(msg_length > 0)
        {
            std::string programLog;
            programLog.resize(static_cast<std::size_t>(msg_length));
            glGetProgramInfoLog(m_handle, msg_length, nullptr, &programLog[0]);

            platform_log(core::log::level::error, "ERROR validating shader: {0}", programLog);
        }
    }
}

void shader_program::collect_uniforms()
{
    auto query = std::array<GLenum, 2>{ GL_NAME_LENGTH, GL_LOCATION };
    auto result = std::array<GLint, query.size()>();
    std::string name;

    GLint num_uniforms = 0;
    glGetProgramInterfaceiv(m_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_uniforms);

    for(int i = 0; i < num_uniforms; i++)
    {
        glGetProgramResourceiv(m_handle, GL_UNIFORM, static_cast<GLuint>(i),
                               query.size(), query.data(), query.size(),
                               nullptr, result.data());

        name.resize(static_cast<std::size_t>(result[0]));
        glGetProgramResourceName(m_handle, GL_UNIFORM, static_cast<GLuint>(i), result[0], nullptr, name.data());

        name.pop_back();
        m_uniform_location.insert({name, result[1]});
    }
}

namespace detail
{

    void uniform(GLint location, const glm::vec2& v) { glUniform2f(location, v.x, v.y); }
    void uniform(GLint location, const glm::vec3& v) { glUniform3f(location, v.x, v.y, v.z); }
    void uniform(GLint location, const glm::vec4& v) { glUniform4f(location, v.x, v.y, v.z, v.w); }

    void uniform(GLint location, const glm::dvec2& v) { glUniform2d(location, v.x, v.y); }
    void uniform(GLint location, const glm::dvec3& v) { glUniform3d(location, v.x, v.y, v.z); }
    void uniform(GLint location, const glm::dvec4& v) { glUniform4d(location, v.x, v.y, v.z, v.w); }

    void uniform(GLint location, const glm::ivec2& v) { glUniform2i(location, v.x, v.y); }
    void uniform(GLint location, const glm::ivec3& v) { glUniform3i(location, v.x, v.y, v.z); }
    void uniform(GLint location, const glm::ivec4& v) { glUniform4i(location, v.x, v.y, v.z, v.w); }

    void uniform(GLint location, const glm::uvec2& v) { glUniform2ui(location, v.x, v.y); }
    void uniform(GLint location, const glm::uvec3& v) { glUniform3ui(location, v.x, v.y, v.z); }
    void uniform(GLint location, const glm::uvec4& v) { glUniform4ui(location, v.x, v.y, v.z, v.w); }

    void uniform(GLint location, const glm::bvec2& v) { glUniform2i(location, v.x, v.y); }
    void uniform(GLint location, const glm::bvec3& v) { glUniform3i(location, v.x, v.y, v.z); }
    void uniform(GLint location, const glm::bvec4& v) { glUniform4i(location, v.x, v.y, v.z, v.w); }

    void uniform(GLint location, const glm::mat2& m) { glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::mat3& m) { glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::mat4& m) { glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m)); }

    void uniform(GLint location, const glm::mat2x3& m) { glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::mat2x4& m) { glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::mat3x2& m) { glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::mat3x4& m) { glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::mat4x2& m) { glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::mat4x3& m) { glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(m)); }

    void uniform(GLint location, const glm::dmat2& m) { glUniformMatrix2dv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::dmat3& m) { glUniformMatrix3dv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::dmat4& m) { glUniformMatrix4dv(location, 1, GL_FALSE, glm::value_ptr(m)); }

    void uniform(GLint location, const glm::dmat2x3& m) { glUniformMatrix2x3dv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::dmat2x4& m) { glUniformMatrix2x4dv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::dmat3x2& m) { glUniformMatrix3x2dv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::dmat3x4& m) { glUniformMatrix3x4dv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::dmat4x2& m) { glUniformMatrix4x2dv(location, 1, GL_FALSE, glm::value_ptr(m)); }
    void uniform(GLint location, const glm::dmat4x3& m) { glUniformMatrix4x3dv(location, 1, GL_FALSE, glm::value_ptr(m)); }

    void uniform(GLint location, GLfloat x) { glUniform1f(location, x); }
    void uniform(GLint location, GLfloat x, GLfloat y) { glUniform2f(location, x, y); }
    void uniform(GLint location, GLfloat x, GLfloat y, GLfloat z) { glUniform3f(location, x, y, z); }
    void uniform(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(location, x, y, z, w); }

    void uniform(GLint location, GLdouble x) { glUniform1d(location, x); }
    void uniform(GLint location, GLdouble x, GLdouble y) { glUniform2d(location, x, y); }
    void uniform(GLint location, GLdouble x, GLdouble y, GLdouble z) { glUniform3d(location, x, y, z); }
    void uniform(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w) { glUniform4d(location, x, y, z, w); }

    void uniform(GLint location, GLint x) { glUniform1i(location, x); }
    void uniform(GLint location, GLint x, GLint y) { glUniform2i(location, x, y); }
    void uniform(GLint location, GLint x, GLint y, GLint z) { glUniform3i(location, x, y, z); }
    void uniform(GLint location, GLint x, GLint y, GLint z, GLint w) { glUniform4i(location, x, y, z, w); }

    void uniform(GLint location, GLuint x) { glUniform1ui(location, x); }
    void uniform(GLint location, GLuint x, GLuint y) { glUniform2ui(location, x, y); }
    void uniform(GLint location, GLuint x, GLuint y, GLuint z) { glUniform3ui(location, x, y, z); }
    void uniform(GLint location, GLuint x, GLuint y, GLuint z, GLuint w) { glUniform4ui(location, x, y, z, w); }

    void uniform(GLint location, GLboolean x) { glUniform1i(location, x); }
    void uniform(GLint location, GLboolean x, GLboolean y) { glUniform2i(location, x, y); }
    void uniform(GLint location, GLboolean x, GLboolean y, GLboolean z) { glUniform3i(location, x, y, z); }
    void uniform(GLint location, GLboolean x, GLboolean y, GLboolean z, GLboolean w) { glUniform4i(location, x, y, z, w); }
}

}
