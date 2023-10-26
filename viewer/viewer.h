#pragma once

#include "core/frameclock.h"
#include "core/msg.h"
#include "core/msg_bus.h"
#include "core/window.h"
#include "imgui/manager.h"
#include "opengl/context.h"
#include "utility/camera.h"

class viewer
{
    typedef std::function<void(core::window& window, core::keyboard::key key, bool press)> key_cb;
    typedef std::function<void(core::window& window, core::mouse::button button, const glm::vec2& pos, bool press)> mouse_cb;
    typedef std::function<void(core::window& window, unsigned int width, unsigned int height)> resize_cb;
    typedef std::function<void(core::window& window, double dt)> render_cb;
    typedef std::function<void(core::window& window, double dt)> update_cb;
    typedef std::function<void(core::window& window, double dt)> gui_cb;

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
    core::frameclock m_frameclock;
    util::camera m_camera;

    std::unique_ptr<glfw::window> m_window;
    std::unique_ptr<opengl::context> m_glcontext;
    std::unique_ptr<imgui::manager> m_imgui;

    key_cb m_key_cb;
    mouse_cb m_mouse_cb;
    resize_cb m_resize_cb;
    render_cb m_render_cb;
    update_cb m_update_cb;
    gui_cb m_gui_cb;

public:
    viewer(const window_settings& settings);
    ~viewer();

    core::window& window();
    opengl::context& context();
    core::msg_bus& msg_bus();
    util::camera& camera();
    const core::frameclock& frameclock() const;

    /* install callbacks */
    void on_key(const key_cb& func);
    void on_mouse_button(const mouse_cb& func);
    void on_resize(const resize_cb& func);
    void on_render(const render_cb& func);
    void on_update(const update_cb& func);
    void on_gui(const gui_cb& func);

    void run();

    /* event callbacks */
    void receive(const msg::window_resize& msg);
    void receive(const msg::mouse_button& msg);
    void receive(const msg::key& msg);
};
