#include "framebuffer.h"

#include "viewer/core/assert.h"
#include "viewer/core/log.h"
#include "renderbuffer.h"
#include "texture.h"
#include "texturecube.h"

namespace opengl
{

namespace detail
{

GLenum as_color_attachment(GLuint index)
{
    platform_assert(index <= 15, "Unsupported color attachment index");

    switch (index)
    {
    case 0: return GL_COLOR_ATTACHMENT0;
    case 1: return GL_COLOR_ATTACHMENT1;
    case 2: return GL_COLOR_ATTACHMENT2;
    case 3: return GL_COLOR_ATTACHMENT3;
    case 4: return GL_COLOR_ATTACHMENT4;
    case 5: return GL_COLOR_ATTACHMENT5;
    case 6: return GL_COLOR_ATTACHMENT6;
    case 7: return GL_COLOR_ATTACHMENT7;

    case 8: return GL_COLOR_ATTACHMENT8;
    case 9: return GL_COLOR_ATTACHMENT9;
    case 10: return GL_COLOR_ATTACHMENT10;
    case 11: return GL_COLOR_ATTACHMENT11;
    case 12: return GL_COLOR_ATTACHMENT12;
    case 13: return GL_COLOR_ATTACHMENT13;
    case 14: return GL_COLOR_ATTACHMENT14;
    case 15: return GL_COLOR_ATTACHMENT15;
    }

    throw std::runtime_error("Unsupported color attachment index");
}

GLenum get_attachment(renderbuffer_internal_type format)
{
    switch (format)
    {
    case renderbuffer_internal_type::rgb:
    case renderbuffer_internal_type::rgba: return GL_MAX_COLOR_ATTACHMENTS;
    case renderbuffer_internal_type::depth24_stencil8:
    case renderbuffer_internal_type::depth32f_stencil8: return GL_DEPTH_STENCIL_ATTACHMENT;
    case renderbuffer_internal_type::depth_component:
    case renderbuffer_internal_type::depth_componentf:
    case renderbuffer_internal_type::depth_component16:
    case renderbuffer_internal_type::depth_component24: return GL_DEPTH_ATTACHMENT;
    case renderbuffer_internal_type::stencil_index8: return GL_STENCIL_ATTACHMENT;
    }

    throw std::runtime_error("Couldn't find Attachment for FormatType!");
}

}


framebuffer::~framebuffer()
{
    glDeleteFramebuffers(1, &m_handle);
}

framebuffer::framebuffer(context &gl_context)
    : m_context(gl_context), m_handle(0), m_complete(false)
{
    glGenFramebuffers(1, &m_handle);
    platform_assert(m_handle != 0, "Unable to allocate a new framebuffer handle");
}

GLuint framebuffer::gl_handle() const
{
    return m_handle;
}

void framebuffer::attach_color(GLuint unit, const handle<texture> &texture)
{
    if(texture->format() == texture_format::depth || texture->format() == texture_format::depth_stencil)
    {
        platform_log(core::log::level::error, "Can't attach Texture with non color format to framebuffer (as color attachment)");
        return;
    }

    bind();

    GLenum attachment = detail::as_color_attachment(unit);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->gl_handle(), 0);
    check_completed();

    unbind();
}

void framebuffer::attach_color(GLuint unit, const handle<renderbuffer> &buffer)
{
    if(detail::get_attachment(buffer->format()) != GL_MAX_COLOR_ATTACHMENTS)
    {
        platform_log(core::log::level::error, "Can't attach Renderbuffer with non color format to framebuffer (as color attachment)");
        return;
    }

    bind();

    GLenum attachment = detail::as_color_attachment(unit);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer->gl_handle());
    check_completed();

    unbind();
}

void framebuffer::attach_color(GLuint unit, const handle<texture_cube>& texture)
{
    if(texture->format() == texture_format::depth || texture->format() == texture_format::depth_stencil)
    {
        platform_log(core::log::level::error, "Can't attach Texture Cube with non color format to framebuffer (as color attachment)");
        return;
    }

    bind();

    GLenum attachment = detail::as_color_attachment(unit);
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->gl_handle(), 0);
    check_completed();

    unbind();
}

void framebuffer::attach_depth(const handle<texture>& texture)
{
    bind();

    switch (texture->format()) {
    case texture_format::depth:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->gl_handle(), 0);
        break;

    case texture_format::depth_stencil:
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->gl_handle(), 0);
        break;

    default:
        platform_log(core::log::level::error, "Can't attach Texture with non depth/stencil format to framebuffer (as depth/stencil attachment)");
    }
    check_completed();

    unbind();
}

void framebuffer::attach_depth(const handle<renderbuffer>& buffer)
{
    auto attachment = detail::get_attachment(buffer->format());
    if(attachment == GL_MAX_COLOR_ATTACHMENTS)
    {
        platform_log(core::log::level::error, "Can't attach Renderbuffer with non depth/stencil format to framebuffer (as depth/stencil attachment)");
        return;
    }

    bind();

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer->gl_handle());
    check_completed();

    unbind();
}

void framebuffer::attach_depth(const handle<texture_cube>& texture)
{
    bind();

    switch (texture->format()) {
    case texture_format::depth:
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->gl_handle(), 0);
        break;

    case texture_format::depth_stencil:
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture->gl_handle(), 0);
        break;

    default:
        platform_log(core::log::level::error, "Can't attach Texture Cube with non depth/stencil format to framebuffer (as depth/stencil attachment)");
    }
    check_completed();

    unbind();
}

void framebuffer::bind(framebuffer_bind options)
{
    m_context.bind_framebuffer(static_cast<GLenum>(options), m_handle);
}

void framebuffer::unbind(framebuffer_bind options)
{
    m_context.bind_framebuffer(static_cast<GLenum>(options), 0);
}

void framebuffer::draw_attachment(GLuint index_0)
{
    GLenum attachments[] = {
        detail::as_color_attachment(index_0)
    };

    bind();
    glDrawBuffers(1, attachments);
    unbind();
}

void framebuffer::draw_attachment(GLuint index_0, GLuint index_1)
{
    GLenum attachments[] = {
        detail::as_color_attachment(index_0),
        detail::as_color_attachment(index_1)
    };

    bind();
    glDrawBuffers(2, attachments);
    unbind();
}

void framebuffer::draw_attachment(GLuint index_0, GLuint index_1, GLuint index_2)
{
    GLenum attachments[] = {
        detail::as_color_attachment(index_0),
        detail::as_color_attachment(index_1),
        detail::as_color_attachment(index_2)
    };

    bind();
    glDrawBuffers(3, attachments);
    unbind();
}

void framebuffer::draw_attachment(GLuint index_0, GLuint index_1, GLuint index_2, GLuint index_3)
{
    GLenum attachments[] = {
        detail::as_color_attachment(index_0),
        detail::as_color_attachment(index_1),
        detail::as_color_attachment(index_2),
        detail::as_color_attachment(index_3)
    };

    bind();
    glDrawBuffers(4, attachments);
    unbind();
}

void framebuffer::draw_attachment(GLuint index_0, GLuint index_1, GLuint index_2, GLuint index_3, GLuint index_4)
{
    GLenum attachments[] = {
        detail::as_color_attachment(index_0),
        detail::as_color_attachment(index_1),
        detail::as_color_attachment(index_2),
        detail::as_color_attachment(index_3),
        detail::as_color_attachment(index_4)
    };

    bind();
    glDrawBuffers(5, attachments);
    unbind();
}

void framebuffer::draw_buffer(color_buffer buffer)
{
    glNamedFramebufferDrawBuffer(m_handle, static_cast<GLenum>(buffer));
}

void framebuffer::read_buffer(color_buffer buffer)
{
    glNamedFramebufferReadBuffer(m_handle, static_cast<GLenum>(buffer));
}

void framebuffer::blit_default(int x0, int y0, int x1, int y1, blit_mask mask, blit_filter filter)
{
    blit_default(x0, y0, x1, y1, x0, y0, x1, y1, mask, filter);
}

void framebuffer::blit_default(int src_x0, int src_y0, int src_x1, int src_y1, int dst_x0, int dst_y0, int dst_x1, int dst_y1, blit_mask mask, blit_filter filter)
{
    bind(framebuffer_bind::read);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(src_x0, src_y0, src_x1, src_y1, dst_x0, dst_y0, dst_x1, dst_y1, static_cast<GLbitfield>(mask), static_cast<GLenum>(filter));

    unbind();
}

bool framebuffer::completed() const
{
    return m_complete;
}

void framebuffer::check_completed()
{
    m_complete = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

}
