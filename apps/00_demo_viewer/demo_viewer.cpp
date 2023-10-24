#include <cstdlib>
#include <iostream>

#include <viewer/viewer.h>


int main(int argc, char** argv)
{
    /* initial window settings */
    viewer::window_settings settings;
    settings.title = "Viewer Demo";
    settings.width = 1280;
    settings.heigth = 720;

    /* construct viewer (creates window and context) */
    viewer view(settings);

    /* access context and create opengl resources */
    auto& context = view.context();

    /* start main loop */
    view.run();

    return EXIT_SUCCESS;
}
