#pragma once

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
    std::unique_ptr<glfw::window> m_window;
    std::unique_ptr<opengl::context> m_glcontext;


public:
    viewer(const window_settings& settings);
    ~viewer();

    glfw::window& window();
    opengl::context& context();

    void run();
};
