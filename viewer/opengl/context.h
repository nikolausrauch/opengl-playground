#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <string>
#include <unordered_map>
#include <tuple>
#include <memory>


class viewer;
namespace glfw  { class window; }
namespace opengl
{

class shader_program;
class vertexarray;
class texture;
class texture_cube;
class renderbuffer;
class framebuffer;

template<typename T> class buffer;
template<typename T> class indexbuffer;
template<typename T> class vertexbuffer;

}


namespace opengl
{

template<typename T> using handle = std::shared_ptr<T>;

enum class type : GLenum
{
    byte_            = GL_BYTE,
    unsigned_byte_   = GL_UNSIGNED_BYTE,
    short_           = GL_SHORT,
    unsigned_short_  = GL_UNSIGNED_SHORT,
    int_             = GL_INT,
    unsigned_int_    = GL_UNSIGNED_INT,
    float_           = GL_FLOAT,
    double_          = GL_DOUBLE,
    half_float_      = GL_HALF_FLOAT,
    fixed_           = GL_FIXED
};


enum class primitives : GLenum
{
    points                      = GL_POINTS,

    lines                       = GL_LINES,
    line_strip                  = GL_LINE_STRIP,
    line_loop                   = GL_LINE_LOOP,
    lines_strip_adjacency       = GL_LINE_STRIP_ADJACENCY,
    lines_adjacency             = GL_LINES_ADJACENCY,

    triangles                   = GL_TRIANGLES,
    triangle_strip              = GL_TRIANGLE_STRIP,
    triangle_fan                = GL_TRIANGLE_FAN,
    triangle_strip_adjacency    = GL_TRIANGLE_STRIP_ADJACENCY,
    triangles_adjacency         = GL_TRIANGLES_ADJACENCY,

    patches                     = GL_PATCHES
};

enum class options : GLenum
{
    blend = GL_BLEND,
    cull_face = GL_CULL_FACE,
    depth_test = GL_DEPTH_TEST,
    scissor_test = GL_SCISSOR_TEST,
    stencil_test = GL_STENCIL_TEST,
    dither = GL_DITHER,
    cube_map_seamless = GL_TEXTURE_CUBE_MAP_SEAMLESS
};

enum class polygon_face : GLenum
{
    front = GL_FRONT,
    back = GL_BACK,
    front_back = GL_FRONT_AND_BACK
};

enum class polygon_mode : GLenum
{
    point = GL_POINT,
    line = GL_LINE,
    fill = GL_FILL
};

enum class blend_equation : GLenum
{
    add = GL_FUNC_ADD,
    subtract = GL_FUNC_SUBTRACT,
    reverse_subtract = GL_FUNC_REVERSE_SUBTRACT
};

enum class blend_func_factor_alpha : GLenum
{
    zero = GL_ZERO,
    one = GL_ONE,

    src_alpha = GL_SRC_ALPHA,
    one_minus_src_alpha = GL_ONE_MINUS_SRC_ALPHA,
    dst_alpha = GL_DST_ALPHA,
    one_minus_dst_alpha = GL_ONE_MINUS_DST_ALPHA,

    constant_alpha = GL_CONSTANT_ALPHA,
    one_minuse_constant_alpha = GL_ONE_MINUS_CONSTANT_ALPHA
};

enum class clear_options : GLbitfield
{
    none = 0,
    color = GL_COLOR_BUFFER_BIT,
    depth = GL_DEPTH_BUFFER_BIT,
    stencil = GL_STENCIL_BUFFER_BIT,
    color_depth = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
    color_stencil = (GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT),
    depth_stencil = (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT),
    all = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
};

class context
{
private:
    glfw::window& m_window;
    std::string m_version;
    std::string m_shading_language;
    std::string m_vendor;
    std::string m_renderer;

    std::unordered_map<options, bool> m_options;
    blend_equation m_blend_equation;
    std::array<blend_func_factor_alpha, 2> m_blend_func;

    polygon_face m_polygon_face;
    polygon_mode m_polygon_mode;

    std::array<int, 4> m_viewport;
    std::array<int, 4> m_scissor;
    std::array<float, 4> m_clear_color;

    float m_line_width;
    float m_point_size;
    bool m_depth_mask;

    GLuint m_shader_binding;
    GLuint m_vertexarray_binding;
    GLuint m_renderbuffer_binding;
    std::unordered_map<GLenum, GLuint> m_buffer_binding;
    std::unordered_map<GLenum, GLuint> m_texture_binding;
    std::unordered_map<GLenum, GLuint> m_framebuffer_binding;


public:
    const std::string& version() const;
    const std::string& shading_language() const;
    const std::string& vendor() const;
    const std::string& renderer() const;

    bool set(options option, bool enable);
    bool enable(options option);
    bool disable(options option);

    bool depth_mask(bool enable);
    bool depth_mask() const;

    blend_equation set(blend_equation equation);
    std::pair<blend_func_factor_alpha, blend_func_factor_alpha> set(blend_func_factor_alpha src, blend_func_factor_alpha dst);

    polygon_mode set(polygon_mode mode);
    polygon_face cull(polygon_face face);

    std::array<int, 4> viewport(int x, int y, int width, int height);
    std::array<int, 4> scissor(int x, int y, int width, int height);
    std::array<int, 4> viewport(const std::array<int, 4>& rec);
    std::array<int, 4> scissor(const std::array<int, 4>& rec);

    const std::array<int, 4>& scissor() const;
    const std::array<int, 4>& viewport() const;

    float line_width(float width);
    float point_size(float size);

    void clear_color(float r, float g, float b, float a);
    void clear(clear_options buffers);
    void swap_framebuffer();

    void bind_vertexarray(GLuint handle);
    void bind_buffer(GLenum bind, GLuint handle);
    void bind_buffer_base(GLenum target, GLuint index, GLuint handle);
    void bind_texture(GLenum bind, GLuint handle, unsigned int unit = 0);
    void bind_renderbuffer(GLenum bind, GLuint handle);
    void bind_framebuffer(GLenum bind, GLuint handle);
    void bind_shader(GLuint handle);

    void gain_context();

    void draw_elements(primitives mode, GLsizei count, type data_type, GLsizei offset = 0);
    void draw_array(primitives mode, GLsizei count, GLsizei first = 0);

    handle<shader_program> make_shader();
    handle<vertexarray> make_vertexarray();

    template<typename... Args>
    handle<texture> make_texture(Args... args)
    {
        return handle<texture>( new texture(*this, std::forward<Args>(args)...) );
    }

    template<typename... Args>
    handle<texture_cube> make_texture_cube(Args... args)
    {
        return handle<texture_cube>( new texture_cube(*this, std::forward<Args>(args)...) );
    }

    template<typename... Args>
    handle<renderbuffer> make_renderbuffer(Args... args)
    {
        return handle<renderbuffer>( new renderbuffer(*this, std::forward<Args>(args)...) );
    }

    template<typename... Args>
    handle<framebuffer> make_framebuffer(Args... args)
    {
        return handle<framebuffer>( new framebuffer(*this, std::forward<Args>(args)...) );
    }

    template<typename data, typename... Args>
    handle<indexbuffer<data>> make_indexbuffer(Args... args)
    {
        return handle<indexbuffer<data>>( new indexbuffer<data>(*this, std::forward<Args>(args)...) );
    }

    template<typename data, typename... Args>
    handle<vertexbuffer<data>> make_vertexbuffer(Args... args)
    {
        return handle<vertexbuffer<data>>( new vertexbuffer<data>(*this, std::forward<Args>(args)...) );
    }

    template<typename data, typename... Args>
    handle<buffer<data>> make_buffer(Args... args)
    {
        return handle<buffer<data>>( new buffer<data>(*this, std::forward<Args>(args)...) );
    }

private:
    context(glfw::window& window);

    friend class ::viewer;
};


}
