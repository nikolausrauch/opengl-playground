#pragma once

#include <functional>

#include "viewer/core/msg.h"

namespace util { class camera; }
namespace core { class msg_bus; class window; class mouse; class keyboard; }

namespace util
{

class camera_control
{
protected:
    std::reference_wrapper<camera> m_cam;
    std::reference_wrapper<core::msg_bus> m_msg_bus;
    std::reference_wrapper<core::window> m_window;
    bool m_ignore{false};

public:
    camera_control(camera& cam, core::msg_bus& bus, core::window& window);
    virtual ~camera_control() = default;

    virtual void update(double dt) = 0;
    void ignore(bool val);

    util::camera& cam();
    core::window& window();
    core::mouse& mouse();
    core::keyboard& keyboard();
};


class orbit_control final : public camera_control
{
private:
    glm::vec2 m_mouse_position{0.0f, 0.0f};
    bool m_button_pressed{false};

public:
    orbit_control(camera& cam, core::msg_bus& bus, core::window& window);
    ~orbit_control();

    void update(double dt) override;
    void update(const glm::vec2& diff, float zoom);

    void receive(const msg::mouse_button&);
    void receive(const msg::mouse_position&);
    void receive(const msg::mouse_scroll&);
};

}
