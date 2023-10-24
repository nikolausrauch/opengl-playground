#include "mouse.h"

#include <array>

namespace core
{

const char* mouse::button_name(button btn)
{
    static constexpr std::array<const char*, static_cast<int>(button::last) + 1> s_name =
    {
            "left", "right", "middle",
            "other 1", "other 2", "other 3", "other 4", "other 5",
            "unkown"
    };

    return s_name[static_cast<int>(btn)];
}

}
