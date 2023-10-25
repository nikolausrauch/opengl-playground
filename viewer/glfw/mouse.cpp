#include "mouse.h"

#include "viewer/core/msg.h"
#include "viewer/core/msg_bus.h"
#include "viewer/core/assert.h"
#include "viewer/glfw/window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glfw
{

namespace detail
{

static constexpr auto g_glfw_buttonmap = [](){
    std::array<core::mouse::button, GLFW_MOUSE_BUTTON_LAST + 1> buttons;
    std::fill(std::begin(buttons), std::end(buttons), core::mouse::button::unknown);

    buttons[GLFW_MOUSE_BUTTON_LEFT] = core::mouse::button::left;
    buttons[GLFW_MOUSE_BUTTON_RIGHT] = core::mouse::button::right;
    buttons[GLFW_MOUSE_BUTTON_MIDDLE] = core::mouse::button::middle;
    buttons[GLFW_MOUSE_BUTTON_4] = core::mouse::button::other_1;
    buttons[GLFW_MOUSE_BUTTON_5] = core::mouse::button::other_5;
    buttons[GLFW_MOUSE_BUTTON_6] = core::mouse::button::other_3;
    buttons[GLFW_MOUSE_BUTTON_7] = core::mouse::button::other_4;
    buttons[GLFW_MOUSE_BUTTON_8] = core::mouse::button::other_5;

    return buttons;
}();

void mouse_button_cb(GLFWwindow* handle, int button, int action, int mods)
{
    auto* window = static_cast<glfw::window*>(glfwGetWindowUserPointer(handle));
    platform_assert(window, "Invalid window pointer");

    glm::dvec2 position;
    glfwGetCursorPos(handle, &position.x, &position.y);

    window->bus().broadcast(msg::mouse_button
    {
        .button = g_glfw_buttonmap[button],
        .position = position,
        .pressed = (action != GLFW_RELEASE)
    });
}

void mouse_scroll_cb(GLFWwindow* handle, double xoffset, double yoffset)
{
    auto* window = static_cast<glfw::window*>(glfwGetWindowUserPointer(handle));
    platform_assert(window, "Invalid window pointer");

    glm::dvec2 position;
    glfwGetCursorPos(handle, &position.x, &position.y);

    window->bus().broadcast(msg::mouse_scroll
    {
        .yoffset = static_cast<float>(yoffset),
        .position = position
    });
}

void mouse_position_cb(GLFWwindow* handle, double x, double y)
{
    auto* window = static_cast<glfw::window*>(glfwGetWindowUserPointer(handle));
    platform_assert(window, "Invalid window pointer");

    window->bus().broadcast(msg::mouse_position
    {
        .position = {x, y}
    });
}

}

mouse::mouse(GLFWwindow* handle)
    : m_handle(handle)
{
    std::fill(m_button_state.begin(), m_button_state.end(), false);

    glfwSetMouseButtonCallback(m_handle, detail::mouse_button_cb);
    glfwSetScrollCallback(m_handle, detail::mouse_scroll_cb);
    glfwSetCursorPosCallback(m_handle, detail::mouse_position_cb);
}

void mouse::cursor_state(core::mouse::cursor state)
{

}

glm::vec2 mouse::position() const
{
    glm::dvec2 pos;
    glfwGetCursorPos(m_handle, &pos.x, &pos.y);
    return pos;
}

bool mouse::operator[](button button) const
{
    return m_button_state[static_cast<int>(button)];
}

void mouse::glfw_button(int button, int action, int mods)
{
    m_button_state[static_cast<int>(detail::g_glfw_buttonmap[button])] = (action != GLFW_RELEASE);
}

}
