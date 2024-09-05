#pragma once

#include "context.h"

namespace opengl
{

enum class framebuffer_bind : GLenum
{
    read        = GL_READ_FRAMEBUFFER,
    write       = GL_DRAW_FRAMEBUFFER,
    draw        = GL_DRAW_FRAMEBUFFER,
    read_write  = GL_FRAMEBUFFER
};

enum class color_buffer : GLenum
{
    none            = GL_NONE,
    front_left      = GL_FRONT_LEFT,
    front_right     = GL_FRONT_RIGHT,
    back_left       = GL_BACK_LEFT,
    back_right      = GL_BACK_RIGHT,
    front           = GL_FRONT,
    back            = GL_BACK,
    left            = GL_LEFT,
    right           = GL_RIGHT,
    front_back      = GL_FRONT_AND_BACK
};

enum class blit_mask : GLenum
{
    color = GL_COLOR_BUFFER_BIT,
    depth = GL_DEPTH_BUFFER_BIT,
    stencil = GL_STENCIL_BUFFER_BIT,
    color_depth = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
    color_depth_stencil = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
};

enum class blit_filter : GLenum
{
    nearest = GL_NEAREST,
    linear = GL_LINEAR
};

class framebuffer
{
private:
    context m_context;
    GLuint m_handle;
    bool m_complete;

public:
    ~framebuffer();

    framebuffer(const framebuffer&) = delete;
    framebuffer& operator = (const framebuffer&) = delete;

    GLuint gl_handle() const;

    void attach_color(GLuint unit, const handle<texture>& texture);
    void attach_color(GLuint unit, const handle<renderbuffer>& buffer);
    void attach_color(GLuint unit, const handle<texture_cube>& texture);

    void attach_depth(const handle<texture>& texture);
    void attach_depth(const handle<renderbuffer>& buffer);
    void attach_depth(const handle<texture_cube>& texture);

    void bind(framebuffer_bind options = framebuffer_bind::read_write);
    void unbind(framebuffer_bind options = framebuffer_bind::read_write);

    void draw_attachment(GLuint index_0);
    void draw_attachment(GLuint index_0, GLuint index_1);
    void draw_attachment(GLuint index_0, GLuint index_1, GLuint index_2);
    void draw_attachment(GLuint index_0, GLuint index_1, GLuint index_2, GLuint index_3);
    void draw_attachment(GLuint index_0, GLuint index_1, GLuint index_2, GLuint index_3, GLuint index_4);

    void draw_buffer(color_buffer buffer);
    void read_buffer(color_buffer buffer);

    void blit_default(int x0, int y0, int x1, int y1, blit_mask mask, blit_filter filter = blit_filter::nearest);
    void blit_default(int src_x0, int src_y0, int src_x1, int src_y1, int dst_x0, int dst_y0, int dst_x1, int dst_y1, blit_mask mask, blit_filter filter = blit_filter::nearest);

    bool completed() const;

private:
    void check_completed();

private:
    framebuffer(context& gl_context);

    friend context;
};

}
