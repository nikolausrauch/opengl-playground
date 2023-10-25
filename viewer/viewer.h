#pragma once

#include "core/msg.h"
#include "core/msg_bus.h"
#include "glfw/window.h"
#include "opengl/context.h"

class viewer
{
public:
    struct window_settings
    {
        std::string title{"OpenGL Viewer"};
        int width{1280};
        int heigth{720};
        bool vsync{true};
        bool hdpi{false};
    };

private:
    core::msg_bus m_msg_bus;
    std::unique_ptr<glfw::window> m_window;
    std::unique_ptr<opengl::context> m_glcontext;

public:
    viewer(const window_settings& settings);
    ~viewer();

    glfw::window& window();
    opengl::context& context();
    core::msg_bus& msg_bus();

    void run();

    /* event callbacks */
    void receive(const msg::window_resize& msg);
};
