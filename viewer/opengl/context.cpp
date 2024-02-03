#include "context.h"

#include "viewer/core/assert.h"
#include "viewer/core/log.h"
#include "viewer/core/platform.h"
#include "viewer/glfw/window.h"

#include "shaderprogram.h"
#include "vertexarray.h"

#include <sstream>

namespace opengl
{

namespace detail
{

GLint primitive_size(primitives primitive)
{
    switch (primitive)
    {
        case primitives::points:       return 1;
        case primitives::lines:        return 2;
        case primitives::triangles:    return 3;
        default:
            throw std::runtime_error("Primitive size unknown!");
    }
}

GLint type_size(GLenum value)
{
    switch (value)
    {
        case GL_BYTE:			return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:	return sizeof(GLubyte);
        case GL_SHORT:			return sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);
        case GL_INT:			return sizeof(GLint);
        case GL_UNSIGNED_INT:	return sizeof(GLuint);
        case GL_FLOAT:			return sizeof(GLfloat);
        case GL_DOUBLE:			return sizeof(GLdouble);
        case GL_HALF_FLOAT:		return sizeof(GLhalf);
        case GL_FIXED:			return sizeof(GLfixed);
    default:
        throw std::runtime_error("Unsupported openGL type constant");
    }
}


GLboolean query_bool(GLenum symbol)
{
    GLboolean data = false;
    glGetBooleanv(symbol, &data);
    return data;
}

GLint query_int(GLenum symbol)
{
    GLint data = 0;
    glGetIntegerv(symbol, &data);
    return data;
}

std::array<GLint, 2> query_int2(GLenum symbol)
{
    std::array<GLint, 2> data = {0, 0};
    glGetIntegerv(symbol, data.data());
    return data;
}

std::array<GLint, 4> query_int4(GLenum symbol)
{
    std::array<GLint, 4> data = {0, 0, 0, 0};
    glGetIntegerv(symbol, data.data());
    return data;
}

float query_float(GLenum symbol)
{
    float value = 0.0f;
    glGetFloatv(symbol, &value);
    return value;
}

void set(options option, bool enabled)
{
    if (enabled)
    {
        glEnable(static_cast<GLenum>(option));
    }
    else
    {
        glDisable(static_cast<GLenum>(option));
    }
}

void set(options option, bool enabled, bool& current)
{
    if (enabled != current)
    {
        if (enabled)
        {
            glEnable(static_cast<GLenum>(option));
        }
        else
        {
            glDisable(static_cast<GLenum>(option));
        }

        current = enabled;
    }
}

void GLAPIENTRY debug_context_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    (void) length;
    (void) userParam;

    /* ignore nvidea notifications */
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    auto errorLog = std::stringstream();

    errorLog << "Debug message (" << id << "):\n";

    errorLog << "\tSource: ";
    switch (source)
    {
        case GL_DEBUG_SOURCE_API :              errorLog << "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM :    errorLog << "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER :  errorLog << "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY :      errorLog << "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION :      errorLog << "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:             errorLog << "Other"; break;
        default: break;
    }
    errorLog << "\n";

    errorLog << "\tType: ";
    switch(type)
    {
        case GL_DEBUG_TYPE_ERROR:                   errorLog << "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:     errorLog << "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:      errorLog << "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:             errorLog << "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:             errorLog << "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:                  errorLog << "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:              errorLog << "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:               errorLog << "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:                   errorLog << "Other"; break;
        default: break;
    }
    errorLog << "\n";

    errorLog << "\tSeverity: ";
    switch(severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION:    errorLog << "notification"; break;
        case GL_DEBUG_SEVERITY_LOW:             errorLog << "low"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:          errorLog << "medium"; break;
        case GL_DEBUG_SEVERITY_HIGH:            errorLog << "high"; break;
        default: break;
    }
    errorLog << "\n";

    errorLog << "\tMessage: " << message << std::endl;

    platform_log(core::log::level::warning, "{0}", errorLog.str());
}

}


const std::string &context::version() const
{
    return m_version;
}

const std::string &context::shading_language() const
{
    return m_shading_language;
}

const std::string &context::vendor() const
{
    return m_vendor;
}

const std::string &context::renderer() const
{
    return m_renderer;
}

bool context::set(options option, bool enable)
{
    auto current = m_options[option];
    detail::set(option, enable, m_options[option]);
    return current;
}

bool context::enable(options option)
{
    return set(option, true);
}

bool context::disable(options option)
{
    return set(option, false);
}

blend_equation context::set(blend_equation equation)
{
    auto previous = m_blend_equation;
    m_blend_equation = equation;

    if(previous != m_blend_equation)
    {
        glBlendEquation(static_cast<GLenum>(equation));
    }

    return previous;
}

std::pair<blend_func_factor_alpha, blend_func_factor_alpha> context::set(blend_func_factor_alpha src, blend_func_factor_alpha dst)
{
    std::pair<blend_func_factor_alpha, blend_func_factor_alpha> previous =
    {
        m_blend_func[0],
        m_blend_func[1]
    };

    m_blend_func[0] = src;
    m_blend_func[1] = dst;

    if(previous.first != src || previous.second != dst)
    {
        glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
    }

    return previous;
}

polygon_mode context::set(polygon_mode mode)
{
    auto previous = m_polygon_mode;
    m_polygon_mode = mode;

    if(previous != mode)
    {
        glPolygonMode(static_cast<GLenum>(polygon_face::front_back), static_cast<GLenum>(m_polygon_mode));
    }

    return previous;
}

polygon_face context::cull(polygon_face face)
{
    auto previous = m_polygon_face;
    m_polygon_face = face;

    if(previous != face)
    {
        glCullFace(static_cast<GLenum>(face));
    }

    return previous;
}

std::array<int, 4> context::viewport(int x, int y, int width, int height)
{
    auto previous = m_viewport;
    m_viewport[0] = x;
    m_viewport[1] = y;
    m_viewport[2] = width;
    m_viewport[3] = height;

    if(previous[0] != x || previous[1] != y || previous[2] != width || previous[3] != height)
    {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    return previous;
}

std::array<int, 4> context::scissor(int x, int y, int width, int height)
{
    auto previous = m_scissor;
    m_scissor[0] = x;
    m_scissor[1] = y;
    m_scissor[2] = width;
    m_scissor[3] = height;

    if(previous[0] != x || previous[1] != y || previous[2] != width || previous[3] != height)
    {
        glScissor(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    return previous;
}

std::array<int, 4> context::viewport(const std::array<int, 4> &rec)
{
    return viewport(rec[0], rec[1], rec[2], rec[3]);
}

std::array<int, 4> context::scissor(const std::array<int, 4> &rec)
{
    return scissor(rec[0], rec[1], rec[2], rec[3]);
}

const std::array<int, 4> &context::scissor() const
{
    return m_scissor;
}

const std::array<int, 4> &context::viewport() const
{
    return m_viewport;
}

float context::line_width(float width)
{
    auto previous = m_line_width;
    m_line_width = width;

    if(previous != width)
    {
        glLineWidth(width);
    }

    return previous;
}

float context::point_size(float size)
{
    auto previous = m_point_size;
    m_point_size = size;

    if(previous != size)
    {
        glPointSize(size);
    }

    return previous;
}

void context::clear_color(float r, float g, float b, float a)
{
    m_clear_color[0] = r;
    m_clear_color[1] = g;
    m_clear_color[2] = b;
    m_clear_color[3] = a;
    glClearColor(r, g, b, a);
}

void context::clear(clear_options buffers)
{
    glClear(static_cast<GLbitfield>(buffers));
}

void context::swap_framebuffer()
{
    m_window.display();
}

void context::bind_vertexarray(GLuint handle)
{
    glBindVertexArray(handle);
    m_vertexarray_binding = handle;
}

void context::bind_buffer(GLenum bind, GLuint handle)
{
    glBindBuffer(bind, handle);
    m_buffer_binding[bind] = handle;
}

void context::bind_texture(GLenum bind, GLuint handle, unsigned int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(bind, handle);

    m_texture_binding[GL_TEXTURE0 + unit] = handle;
}

void context::bind_renderbuffer(GLenum bind, GLuint handle)
{
    glBindRenderbuffer(bind, handle);
    m_renderbuffer_binding = handle;
}

void context::bind_framebuffer(GLenum bind, GLuint handle)
{
    glBindFramebuffer(bind, handle);
    m_framebuffer_binding[bind] = handle;
}

void context::bind_shader(GLuint handle)
{
    glUseProgram(handle);
    m_shader_binding = handle;
}

void context::gain_context()
{
    m_window.make_active_context();
}

void context::draw_elements(primitives mode, GLsizei count, type data_type, GLsizei offset)
{
    platform_assert(data_type == type::unsigned_byte_ || data_type == type::unsigned_short_ || data_type == type::unsigned_int_, "Invalid index type");

    GLvoid* off = reinterpret_cast<GLvoid*>(offset);
    glDrawElements(static_cast<GLenum>(mode), count * detail::primitive_size(mode), static_cast<GLenum>(data_type), off);
}

void context::draw_array(primitives mode, GLsizei count, GLsizei first)
{
    glDrawArrays(static_cast<GLenum>(mode), first, count * detail::primitive_size(mode));
}

std::shared_ptr<shader_program> context::make_shader()
{
    return std::shared_ptr<shader_program>(new shader_program(*this));
}

std::shared_ptr<vertexarray> context::make_vertexarray()
{
    return std::shared_ptr<vertexarray>(new vertexarray(*this));
}

context::context(glfw::window& window)
    : m_window(window)
{   
    m_window.make_active_context();

    auto err = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    platform_assert(err, "Glad failed to initialize OpenGL");
    (void) err;

    m_version = reinterpret_cast<const char*>( glGetString(GL_VERSION) );
    m_shading_language = reinterpret_cast<const char*>( glGetString(GL_SHADING_LANGUAGE_VERSION) );
    m_vendor = reinterpret_cast<const char*>( glGetString(GL_VENDOR) );
    m_renderer = reinterpret_cast<const char*>( glGetString(GL_RENDERER) );

    platform_log(core::log::level::info,
               "opengl.version = {0} \nglsl.version = {1} \ngpu.vendor = {2} \ngpu.renderer = {3}",
                m_version, m_shading_language, m_vendor, m_renderer);

    /* TODO: message callback available from 4.3 and is not supported on lower opengl versions */
    if constexpr (g_debug)
    {
        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            glDebugMessageCallback(detail::debug_context_output, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); /* no filter */
        }
        else
        {
            platform_log(core::log::level::error, "\t failed to create debug context!");
        }
    }

    m_options[options::blend] = glIsEnabled(static_cast<GLenum>(options::blend));
    m_options[options::cull_face] = glIsEnabled(static_cast<GLenum>(options::cull_face));
    m_options[options::depth_test] = glIsEnabled(static_cast<GLenum>(options::depth_test));
    m_options[options::scissor_test] = glIsEnabled(static_cast<GLenum>(options::scissor_test));
    m_options[options::stencil_test] = glIsEnabled(static_cast<GLenum>(options::stencil_test));
    m_options[options::dither] = glIsEnabled(static_cast<GLenum>(options::dither));

    m_blend_equation = static_cast<blend_equation>(detail::query_int(GL_BLEND_EQUATION_RGB));
    m_blend_func[0] = static_cast<blend_func_factor_alpha>(detail::query_int(GL_BLEND_SRC_ALPHA));
    m_blend_func[1] = static_cast<blend_func_factor_alpha>(detail::query_int(GL_BLEND_DST_ALPHA));

    m_polygon_mode = static_cast<polygon_mode>(detail::query_int2(GL_POLYGON_MODE)[0]);

    m_viewport = detail::query_int4(GL_VIEWPORT);

    m_scissor = detail::query_int4(GL_SCISSOR_BOX);
    m_line_width = detail::query_float(GL_LINE_WIDTH);
    m_point_size = detail::query_float(GL_POINT_SIZE);
}


}
