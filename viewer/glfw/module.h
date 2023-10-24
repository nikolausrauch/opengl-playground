#pragma once

#include "engine/module.h"
#include "engine/message/fwd.h"

#include <memory>
#include <string>

namespace glfw
{

class window;
class keyboard;
class mouse;
class gamepad_manager;

class module final : public module_def<module>
{
public:
    struct config
    {
        std::string window_title;
        unsigned int window_width;
        unsigned int window_height;
        bool vsync;
    };

private:
    config m_config;
    std::unique_ptr<window> m_window{nullptr};
    std::unique_ptr<keyboard> m_keyboard{nullptr};
    std::unique_ptr<mouse> m_mouse{nullptr};
    std::unique_ptr<gamepad_manager> m_gamepads{nullptr};

public:
    module(const config& conf = config{
            .window_title = "engine window",
            .window_width = 1280,
            .window_height = 720,
            .vsync = true});

    void receive(const message::frame_start&);

private:
    virtual void startup(entt::registry& registry);
    virtual void shutdown(entt::registry& registry);
};

}
