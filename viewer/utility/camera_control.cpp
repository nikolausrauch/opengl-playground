#include "camera_control.h"

#include "camera.h"
#include "viewer/core/keyboard.h"
#include "viewer/core/msg_bus.h"
#include "viewer/core/mouse.h"
#include "viewer/core/window.h"

#include <algorithm>

namespace util
{

camera_control::camera_control(camera& cam, core::msg_bus& bus, core::window& window)
    : m_cam(cam), m_msg_bus(bus), m_window(window)
{

}

void camera_control::ignore(bool val)
{
    m_ignore = val;
}

camera& camera_control::cam()
{
    return m_cam;
}

core::window& camera_control::window()
{
    return m_window;
}

core::mouse& camera_control::mouse()
{
    return m_window.get().mouse();
}

core::keyboard& camera_control::keyboard()
{
    return m_window.get().keyboard();
}

orbit_control::orbit_control(camera& cam, core::msg_bus& bus, core::window& window)
    : camera_control(cam, bus, window)
{
    m_msg_bus.get().connect<msg::mouse_position>(this);
    m_msg_bus.get().connect<msg::mouse_button>(this);
    m_msg_bus.get().connect<msg::mouse_scroll>(this);
}

orbit_control::~orbit_control()
{
    m_msg_bus.get().disconnect<msg::mouse_position>(this);
    m_msg_bus.get().disconnect<msg::mouse_button>(this);
    m_msg_bus.get().disconnect<msg::mouse_scroll>(this);
}

void orbit_control::update(double dt)
{

}

void orbit_control::update(const glm::vec2& diff, float zoom)
{
    auto& cam = m_cam.get();

    auto dir = cam.position() - cam.look_at();

    auto r = glm::length(dir);
    auto phi = std::atan2(dir.x, dir.z);
    auto theta = std::atan2(sqrt(dir.x * dir.x + dir.z * dir.z), dir.y);

    phi += diff.x * (glm::pi<float>() / cam.width());
    theta += diff.y * (glm::pi<float>() / cam.height());
    r += zoom * r;

    theta = std::clamp<float>(theta, 1e-4, glm::pi<float>() - 1e-4);
    r = std::max(r, 1e-4f);

    glm::vec3 coord{r * std::sin(theta) * std::sin(phi), r * std::cos(theta), r * std::sin(theta) * std::cos(phi)};

    cam.position(cam.look_at() + coord);
}

void orbit_control::receive(const msg::mouse_button& msg)
{
    if(m_ignore) { return; }

    if(msg.button == core::mouse::button::left)
    {
        m_button_pressed = msg.pressed;
        m_mouse_position = msg.position;
    }
}

void orbit_control::receive(const msg::mouse_position& msg)
{
    if(m_ignore) { return; }

    if(m_button_pressed)
    {
        auto diff = m_mouse_position - msg.position;
        update(diff, 0.0f);
        m_mouse_position = msg.position;
    }
}

void orbit_control::receive(const msg::mouse_scroll& msg)
{
    if(m_ignore) { return; }
    update({0.0f, 0.0f}, -0.1f * msg.yoffset);
}

}
