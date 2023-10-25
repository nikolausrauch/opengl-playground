#pragma once

#include "mouse.h"
#include "keyboard.h"

#include <glm/vec2.hpp>

namespace msg
{

/********* WINDOW EVENTS *********/
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
    glm::vec2 position;
};


/********* MOUSE EVENTS *********/
struct mouse_position
{
    glm::vec2 position;
};

struct mouse_button
{
    core::mouse::button button;
    glm::dvec2 position;
    bool pressed;
};

struct mouse_scroll
{
    float yoffset;
    glm::dvec2 position;
};


/********* KEYBOARD EVENTS *********/
struct key
{
    core::keyboard::key key;
    int scann_code;
    bool pressed;
};

struct key_char
{
    unsigned int code;
};


}
