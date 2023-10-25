#pragma once

namespace msg
{

struct window_closed {};

struct window_resize
{
    unsigned int width;
    unsigned int height;
};

struct framebuffer_resize
{
    unsigned int width;
    unsigned int height;
};


struct window_focus
{
    bool gained;
};

struct window_position
{
    int x; int y;
};

}
