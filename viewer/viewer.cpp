#include "viewer.h"

#include "core/log.h"

#include "glfw/base.h"
#include "glfw/window.h"

#include <iostream>

viewer::viewer(const window_settings& settings)
{
    if constexpr (core::g_logging)
    {
        auto& log = core::log::instance();
        log.add_sink(core::log::level::info, [](const auto& msg){ std::cout << msg.text << std::endl; });
        log.add_sink(core::log::level::warning, [](const auto& msg){ std::cout << msg.text << std::endl; });
        log.add_sink(core::log::level::error, [](const auto& msg){ std::cerr << msg.file << ":" << msg.line << ":" << msg.func << " " << msg.text << std::endl; });
    }

    /*----- event listener ----*/
    m_msg_bus.connect<msg::window_resize>(this);
    m_msg_bus.connect<msg::key>(this);
    m_msg_bus.connect<msg::mouse_button>(this);


    glfw::startup();

    m_window = std::make_unique<glfw::window>(m_msg_bus, settings.title, settings.width, settings.heigth);
    m_window->vsync(settings.vsync);

    m_glcontext.reset(new opengl::context(static_cast<glfw::window&>(*m_window)));
    m_glcontext->clear_color(0, 0, 0, 1);
    m_glcontext->clear(opengl::clear_options::all);
    m_window->display();

    m_imgui = std::make_unique<imgui::manager>(m_msg_bus, *m_window, *m_glcontext);

    m_camera.perspective(settings.width, settings.heigth, 0.25f*glm::pi<float>(), 0.01f, 20.0f);
}

viewer::~viewer()
{
    m_glcontext.reset();
    m_window.reset();

    glfw::shutdown();

    if constexpr (core::g_logging)
    {
        auto& log = core::log::instance();
        log.clear();
    }

    /*----- event listener ----*/
    m_msg_bus.disconnect<msg::window_resize>(this);
    m_msg_bus.disconnect<msg::key>(this);
    m_msg_bus.disconnect<msg::mouse_button>(this);
}

core::window& viewer::window()
{
    return *m_window;
}

opengl::context& viewer::context()
{
    return *m_glcontext;
}

core::msg_bus &viewer::msg_bus()
{
    return m_msg_bus;
}

util::camera& viewer::camera()
{
    return m_camera;
}

void viewer::on_key(const key_cb& func)
{
    m_key_cb = func;
}

void viewer::on_mouse_button(const mouse_cb& func)
{
    m_mouse_cb = func;
}

void viewer::on_resize(const resize_cb& func)
{
    m_resize_cb = func;
}

void viewer::on_render(const render_cb& func)
{
    m_render_cb = func;
}

void viewer::on_update(const update_cb& func)
{
    m_update_cb = func;
}

void viewer::on_gui(const gui_cb& func)
{
    m_gui_cb = func;
}

void viewer::run()
{
    while(!m_window->closed())
    {
        glfw::poll_events();

        double dt = 1.0 / 60.0;

        if(m_update_cb) { m_update_cb(*m_window, dt); }

        m_glcontext->clear(opengl::clear_options::all);
        {
            if(m_render_cb) { m_render_cb(*m_window, dt); }

            m_imgui->new_frame(dt);
            {
                if(m_gui_cb) { m_gui_cb(*m_window, dt); }
            }
            m_imgui->end_frame();
        }
        m_window->display();
    }
}

void viewer::receive(const msg::window_resize& msg)
{
    m_glcontext->viewport(0, 0, msg.width, msg.height);
    m_camera.size(msg.width, msg.height);

    if(m_resize_cb) { m_resize_cb(*m_window, msg.width, msg.height); }
}

void viewer::receive(const msg::mouse_button& msg)
{
    if(m_mouse_cb) { m_mouse_cb(*m_window, msg.button, msg.position, msg.pressed); }
}

void viewer::receive(const msg::key& msg)
{
    if(m_key_cb) { m_key_cb(*m_window, msg.key, msg.pressed); }
}
