#include "window.h"

#include "viewer/asset/image.h"
#include "viewer/core/assert.h"
#include "viewer/core/msg_bus.h"
#include "viewer/core/msg.h"
#include "viewer/core/log.h"
#include "viewer/core/platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glfw
{

namespace detail
{

void framebuffer_size_cb(GLFWwindow* handle, int width, int height)
{
    auto bus = static_cast<core::msg_bus*>(glfwGetWindowUserPointer(handle));
    platform_assert(bus, "Invalid message bus pointer");

    bus->broadcast(msg::framebuffer_resize
    {
        .width = static_cast<unsigned int>(width),
        .height = static_cast<unsigned int>(height)
    });
}

void window_close_cb(GLFWwindow* handle)
{
    auto bus = static_cast<core::msg_bus*>(glfwGetWindowUserPointer(handle));
    platform_assert(bus, "Invalid message bus pointer");

    bus->broadcast(msg::window_closed{});
}

void window_focus_cb(GLFWwindow* handle, int state)
{
    auto bus = static_cast<core::msg_bus*>(glfwGetWindowUserPointer(handle));
    platform_assert(bus, "Invalid message bus pointer");

    bus->broadcast(msg::window_focus{ .gained = (state == GLFW_FOCUSED) });
}

void window_position_cb(GLFWwindow* handle, int x, int y)
{
    auto bus = static_cast<core::msg_bus*>(glfwGetWindowUserPointer(handle));
    platform_assert(bus, "Invalid message bus pointer");

    bus->broadcast(msg::window_position{ .x = x, .y = y });
}

void window_refresh_cb(GLFWwindow* handle)
{
    auto bus = static_cast<core::msg_bus*>(glfwGetWindowUserPointer(handle));
    platform_assert(bus, "Invalid message bus pointer");

    (void) bus;
}

void window_resize_cb(GLFWwindow* handle, int width, int height)
{
    auto bus = static_cast<core::msg_bus*>(glfwGetWindowUserPointer(handle));
    platform_assert(bus, "Invalid message bus pointer");

    bus->broadcast(msg::window_resize
    {
        .width = static_cast<unsigned int>(width),
        .height = static_cast<unsigned int>(height)
    });
}


}

window::window(core::msg_bus& bus, const std::string& title, unsigned int width, unsigned int height)
{
    if constexpr (g_render_api == graphic_api::opengl)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, g_debug ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if constexpr (g_system == osystem::macosx)
        {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        }
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER,   GL_TRUE);

    m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwGetWindowSize(m_handle, &m_size_backup.x, &m_size_backup.y);
    glfwGetWindowPos(m_handle, &m_pos_backup.x, &m_pos_backup.y);

    platform_assert(m_handle, "Couldn't create Window");

    glfwMakeContextCurrent(m_handle);

    glfwSetWindowUserPointer(m_handle, &bus);

    /* callbacks */
    glfwSetWindowSizeCallback(m_handle, &detail::window_resize_cb);
    glfwSetFramebufferSizeCallback(m_handle, &detail::framebuffer_size_cb);
    glfwSetWindowCloseCallback(m_handle, &detail::window_close_cb);
    glfwSetWindowFocusCallback(m_handle, &detail::window_focus_cb);
    glfwSetWindowPosCallback(m_handle, &detail::window_position_cb);
    glfwSetWindowRefreshCallback(m_handle, &detail::window_refresh_cb);
}

window::~window()
{
    if(m_handle)
    {
        glfwDestroyWindow(m_handle);
    }
}

GLFWwindow* window::handle() const
{
    return m_handle;
}

void window::close()
{
    glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
}

void window::show()
{
    glfwShowWindow(m_handle);
}

void window::iconify()
{
    glfwIconifyWindow(m_handle);
}

void window::title(const std::string& title)
{
    glfwSetWindowTitle(m_handle, title.c_str());
}

void window::position(const glm::ivec2& point)
{
    glfwSetWindowPos(m_handle, point.x, point.y);
}

glm::ivec2 window::position() const
{
    glm::ivec2 pos;
    glfwGetWindowPos(m_handle, &pos.x, &pos.y);
    return pos;
}

void window::size(const glm::ivec2& size)
{
    glfwSetWindowSize(m_handle, size.x, size.y);
}

glm::uvec2 window::size() const
{
    glm::ivec2 size;
    glfwGetWindowSize(m_handle, &size.x, &size.y);
    return size;
}

glm::uvec2 window::framebuffer_size() const
{
    return size(); // TODO: query actual framebuffer size
}

bool window::focused() const
{
    return (glfwGetWindowAttrib(m_handle, GLFW_FOCUSED) != 0);
}

bool window::iconified() const
{
    return (glfwGetWindowAttrib(m_handle, GLFW_ICONIFIED) != 0);
}

bool window::visible() const
{
    return (glfwGetWindowAttrib(m_handle, GLFW_VISIBLE) != 0);
}

bool window::closed() const
{
    return (glfwWindowShouldClose(m_handle) != 0);
}

void window::icon(const asset::handle<asset::image> &img)
{
    GLFWimage icons[1];
    icons[0].pixels = img->ptr();
    icons[0].width = img->size().x;
    icons[0].height = img->size().y;
    glfwSetWindowIcon(m_handle, 1, icons);
}

void window::vsync(bool enable)
{
    glfwSwapInterval(enable ? 1 : 0);
}

void window::fullscreen(bool enable)
{
    if(enable)
    {
        m_size_backup = size();
        m_pos_backup = position();

        auto monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode * mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor( m_handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate );
        /* TODO: this is borderless fullscreen? https://www.glfw.org/docs/latest/window_guide.html#window_creation, https://github.com/libgdx/libgdx/issues/4785 */
    }
    else
    {
        glfwSetWindowMonitor( m_handle, nullptr, m_pos_backup.x, m_pos_backup.y, m_size_backup.x, m_size_backup.y, 0 );
    }
}

void window::make_active_context()
{
    glfwMakeContextCurrent(m_handle);
}

void window::display()
{
    glfwSwapBuffers(m_handle);
}

}
