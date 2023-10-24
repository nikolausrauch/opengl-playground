#pragma once

#include <glm/vec2.hpp>

namespace core
{

class mouse
{
public:
    enum class button: int
    {
        left = 0,
        right,
        middle,
        other_1,
        other_2,
        other_3,
        other_4,
        other_5,

        unknown,

        last = unknown
    };

    enum class cursor
    {
        visible,
        hidden,
        disabled
    };

public:
    virtual ~mouse() = default;

    virtual bool operator[] (button button) const = 0;

    virtual void cursor_state(cursor state) = 0;
    virtual glm::vec2 position() const = 0;

    static const char* button_name(button btn);
};

}

