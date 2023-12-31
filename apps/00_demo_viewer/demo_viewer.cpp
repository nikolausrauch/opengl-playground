#include <cstdlib>

#include <viewer/viewer.h>

int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Minimal Viewer Example";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /***************** install callbacks *****************/
    view.on_update([](auto& window, float dt)
    {
        /* called once per frame before render call */
    });

    view.on_render([&](auto& window, float dt)
    {
        /* called once per frame (framebuffer is already cleared) */
    });

    view.on_gui([](auto& window, float dt)
    {
        /* submit imgui / implot calls from here (new frame, end frame called in viewer) */
        ImGui::ShowDemoWindow();
        ImPlot::ShowDemoWindow();
    });

    view.on_mouse_button([](auto& window, auto button, auto pos, bool pressed)
    {
        /* called on mouse button events */
    });

    view.on_resize([](auto& window, unsigned int width, unsigned int height)
    {
        /* called on window resize */
    });

    view.on_key([](auto& window, auto key, bool pressed)
    {
        /* called on key event */

        /* exit on escape */
        if(key == core::keyboard::key::escape && pressed)
        {
            window.close();
        }

        /* toggle fullscreen window */
        if(key == core::keyboard::key::f11 && pressed)
        {
            static bool toggle{false};
            toggle = !toggle;
            window.fullscreen(toggle);
        }
    });

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
