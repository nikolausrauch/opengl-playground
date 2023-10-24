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

    glfw::startup();

    m_window = std::make_unique<glfw::window>(settings.title, settings.width, settings.heigth);
    m_window->vsync(settings.vsync);

    m_glcontext.reset(new opengl::context(static_cast<glfw::window&>(*m_window)));
    m_glcontext->clear_color(0, 0, 0, 1);
    m_glcontext->clear(opengl::clear_options::all);
    m_window->display();
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
}

glfw::window& viewer::window()
{
    return *m_window;
}

opengl::context& viewer::context()
{
    return *m_glcontext;
}

void viewer::run()
{
    while(!m_window->closed())
    {
        glfw::poll_events();

        m_glcontext->clear(opengl::clear_options::all);
        {

        }
        m_window->display();
    }
}
